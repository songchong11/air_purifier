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


unchar ReadAPin;

unchar recvStat = COM_STOP_BIT; //定义接收状态机

#if CONFIG_RX_USE_T0
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

#else
/*-------------------------------------------------
 * 函数名称：   TIMER2_INITIAL
 * 功能：      初始化设置定时器2 
 * 相关寄存器： T2CON TMR2 PR2 TMR2IE TMR2IF PEIE GIE 
 -------------------------------------------------*/
void TIMER2_INITIAL (void) 
{    
    T2CON0  = 0B00000000; 			//T2预分频1:1，后分频1：4
    //BIT7: 0：无意义； 1：把PR2/P1xDTy缓冲值分别更新到PR2寄存器和P1xDTy_ACT
    //BIT6~BIT3: 定时器2输出后分频比选择 0000:1:1;0001:1:2;……1:16
    //BIT2:0:关闭定时器2；1：打开定时器2
    //BIT1~0:定时器2预分频选择 00:1;01:4;1x:16
    
	T2CON1  = 0B00000000;		   //T2时钟来自系统时钟,PWM1连续模式
	//BIT4: PWM模式选择 0:连续模式；1：单脉冲模式
    //BIT3: 0:PWM模式；1：蜂鸣器模式	
    //Timer2时钟源选择：000：指令时钟；001：系统时钟；010：HIRC的2倍频；100：HIRC；101：LIRC
    						
    TMR2H  	= T2_RELOAD_VALUE_H;					//定时器2计数寄存器  =1/16*2*4*200
    TMR2L  	= T2_RELOAD_VALUE_L;//104us
    
    
	PR2H    = T2_RELOAD_VALUE_H; 					//周期=（PR+1）*Tt2ck*TMR2预分频(蜂鸣器模式周期*2)
	PR2L    = T2_RELOAD_VALUE_L;	  
    
    P1ADTH	= 0;					//脉宽=P1xDT*Tt2ck*TMR2预分频(蜂鸣器模式没用到)
    P1ADTL	= 50;
    
    P1OE 	= 0B00000000;			//充许P1A0输出PWM（配置成timer定时器时这位清零）
    //BIT7: 0:禁止P1C输出到管脚;1:充许P1C输出到管脚
    //BIT6: 0:禁止P1B输出到管脚;1:充许P1B输出到管脚
    //BIT5~BIT0: 0:禁止P1Ax输出到管脚;1:充许P1Ax输出到管脚
    
    P1POL 	= 0B00000000;			//高电平有效
    //BIT7: 0:P1C高电平有效;1:P1C低电平有效
    //BIT6: 0:P1B高电平有效;1:P1B低电平有效
    //BIT5~BIT0: 0:P1Ax高电平有效;1:P1Ax低电平有效
    
    P1CON	= 0B00000000;
    //BIT7:PWM1 重启使能位
	//1 = 故障刹车时，P1BEVT位在退出关闭事件时自动清零，PWM1自动重启
	//0 = 故障刹车时，必须用软件将P1BEVT清零以重启PWM1
    //BIT6~0:PWM1死区时间设置
	//P1DCn = 预定MPWM信号应转变为有效与PWM信号实际转为有效之间的T2CK周期数
        
    MSCON0   = 0B00110000;		    //bit0: 0:T2睡眠时停止工作	
    //BIT5:PSRCAH4和PSRCA[4]共同决定源电流。00：4mA; 11: 33mA; 01、10:8mA
    //BIT4:PSRCAH3和PSRCA[3]共同决定源电流。00：4mA; 11: 33mA; 01、10:8mA
    //BIT3:UCFG1<1:0>为01时此位有意义。0：禁止LVR；1：打开LVR
    //BIT2:快时钟测量慢周期的平均模式。0：关闭平均模式；1：打开平均模式
    //BIT1:0：关闭快时钟测量慢周期；1：打开快时钟测量慢周期
    //BIT0:0：睡眠时停止工作：1： 睡眠时保持工作。当T2时钟不是选择指令时钟的时候
     
				 					//设置TMR2输出比较值定时15us=(1/4000000)*60(PR2)
	TMR2IF  = 0;					//清TMER2中断标志
	TMR2IE = 1;						//使能TMER2的中断（配置成timer定时器时不注释）
	TMR2ON  = 1;					//使能TMER2启动
	PEIE    = 1;    				//使能外设中断
	GIE     = 1;   					//使能全局中断
}
#endif

void send_a_byte(unchar input)
{
		//发送启始位
		unchar i=8;

		UART_TX =	0;
        DelayUs(DELAY_104US);
#if 1
		//发送8位数据位
		while(i--)
		{
				UART_TX=(input&0x01);//先传低位
				 
				DelayUs(DELAY_104US);
				 
				input=input>>1;
		}
		 
		//发送校验位(无)
		//发送结束位
		UART_TX= 1;
		DelayUs(DELAY_104US);;
#endif

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




//===========================================================
