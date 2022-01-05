//Project: air_purifier.prj
// Device: FT62F13X
// Memory: Flash 3KX14b, EEPROM 128X8b, SRAM 256X8b
// Author: 
//Company: 
//Version:
//   Date: 
/*  文件名 uart.c
*	功能： IO模拟uart时序
*   IC:    FT61F13 SOP20
*   晶振：  16M/2T                    
*   说明： 波特率9600, 传输1bit的时间约等于0.104ms
*		        数据由1bit起始位+8bit数据位+1bit停止位共10bit的数据帧组成，起始位为低电平，停止位为高电平
*
*
*                  FT61F13  SOP20
*                 ----------------
*  VDD-----------|1(VDD)   (VSS)20|------------VSS     
*  NC------------|2(PC1)   (PA0)19|------------NC 
*  NC------------|3(PC0)   (PA1)18|------------NC
*  NC------------|4(PB7)   (PA2)17|------------DemoPortIn
*  NC------------|5(PB6)   (PA3)16|------------NC
*  NC------------|6(PB5)   (PA4)15|-------DemoPortOut
*  NC------------|7(PB4)   (PA5)14|------------NC
*  NC------------|8(PB3)   (PA6)13|------------NC
*  NC------------|9(PB2)   (PA7)12|------------NC
*  NC------------|10(PB1)  (PB0)11|------------NC
*                 ----------------   
*/
//===========================================================
//===========================================================
#include	"SYSCFG.h"
#include	"public.h"

//===========================================================
//Variable definition
//===========================================================
//宏定义****************************************************
#define 	unchar     	unsigned char 
#define 	unint         unsigned int
#define  	unlong 		unsigned long

unsigned char  TM0_FLAG  = 0;

unchar ReadAPin;
unchar recvData;
unchar recvStat = COM_STOP_BIT; //定义接收状态机
/*----------------------------------------------------
 *	函数名称：TIMER0_INITIAL
 *	功能：初始化设置定时器
 *	相关寄存器：T0CS T0SE PSA 
 *	设置TMR0定时时长8.192ms=(1/16000000)*2*256*255(16M-2T-PSA 1:256- TMR0=255溢出)
  * Bit2:0 PS2 8个预分频比 100 :32分频
 * 设置TMR0定时时长0.104ms=(1/16000000)*2*8*(104)
	255 - 104 = 151;
 ----------------------------------------------------*/
void TIMER0_INITIAL (void)  
{
	OPTION = 0B00000010;	//Bit5 T0CS Timer0时钟源选择 
							//1-外部引脚电平变化T0CKI 0-内部时钟(FOSC/2)
							//Bit4 T0CKI引脚触发方式 1-下降沿 0-上升沿
							//Bit3 PSA 预分频器分配位 0-Timer0 1-WDT 
							//Bit2:0 PS2 8个预分频比 111 - 1:256
                            
	TMR0 = TIMER0_RELOAD_VALUE;
	T0IF = 0;				//清空T0软件中断
}


/*-------------------------------------------------
 *  函数名:WaitTF0
 *	功能： //查询传输标志位
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/

static void WaitTF0( void )
{
	while(!TM0_FLAG) ;
 
	TM0_FLAG=0; //清标志位
}

/*-------------------------------------------------
 *  函数名: send_a_byte 
 *	功能：  发送一个字符
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/

void send_a_byte(unchar input)
{
		//发送启始位
		unchar i=8;
		 
	
		T0IE = 1;//启动timer0
		UART_TX =	0;
		WaitTF0();
        WaitTF0();
#if 1
		//发送8位数据位
		while(i--)
		{
				UART_TX=(input&0x01);//先传低位
				 
				WaitTF0();
				 
				input=input>>1;
		}
		 
		//发送校验位(无)
		//发送结束位
		UART_TX= 1;
		WaitTF0();
#endif
		T0IE = 0;//关闭timer0
}


/*-------------------------------------------------
 *  函数名: PA1_Level_Change_INITIAL
 *	功能：  PA端口(PA1)电平变化中断初始化
 *  输入：  无
 *  输出：  无
--------------------------------------------------*/
void PA1_Level_Change_INITIAL(void)
{
 
	TRISA1 =1; 			     //SET PA1 INPUT
	ReadAPin = PORTA;	     //清PA电平变化中断
	PAIF =0;   			     //清PA INT中断标志位
    IOCA1 =1;  			     //使能PA1电平变化中断
	PAIE =1;   			     //使能PA INT中断
    //GIE =1;    			     //使能全局中断
}

#if 0
/*-------------------------------------------------
 *  函数名: read_a_byte 
 *	功能：  接收一个字符
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
unchar read_a_byte()
{
    unchar Output = 0;
    unchar i = 8;
    T0IE = 1;               //启动Timer0

    WaitTF0();             //等过起始位
    WaitTF0();             //等过起始位
 
    //发送8位数据位
    while(i--)
    {
        Output >>= 1;
        if(UART_RX) Output |= 0x80;    //先收低位
        WaitTF0();                 		 //位间延时
    }
 
	while(!TM0_FLAG)
	{
		if(UART_RX) break;
	}

	T0IE = 0;                        //停止

	return Output;
}
#endif



//===========================================================
