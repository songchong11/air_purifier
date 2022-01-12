//********************************************************* 
/* 文件名：TEST_64F0Ax_UART.c
*	功能：   FT64F0Ax_UART功能演示
*  IC:   	 FT64F0Ax     TSSOP20
*  内部：   16M	                              
*	说明： 	 串口上电发送10个字符，然后等待接收10个字节数据（通过串口助手发送接收）
* 
*
*                FT64F0AX  TSSOP20
*              -------------------
* NC----------|1(PA5)   	(PA4)20|-----------NC     
* TXIO--------|2(PA6)   	(PA3)19|-----------NC 
* RXIO--------|3(PA7)   	(PA2)18|-----------NC
* NC----------|4(PC0)   	(PA1)17|-----------NC
* NC----------|5(PC1)		(PA0)16|-----------NC	
* NC----------|6(PB7)		(PB0)15|-----------NC
* GND--------|7(GND)		(PB1)14|-----------NC
* NC----------|8(PB6)		(PB2)13|-----------NC
* VDD--------|9(VDD)		(PB3)12|-----------NC
* NC----------|10(PB5)		(PB4)11|-----------NC
*				-------------------
*/ 
//*********************************************************
#include	"SYSCFG.h";
#include 	"FT64F0AX.h";
#include 	"public.h";

//*********************************************************
//***********************宏定义*****************************
#define		uchar		unsigned char
#define		uint		unsigned int
#define		ulong		unsigned long

AIR_PURIFIER air_purif;

#define		DemoPortOut	RB3
#define		DemoPortIn		RC1

volatile	uchar		receivedata[10]=0;
volatile	uchar		senddata=0;

volatile	uchar		toSend[11]={0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa};
uchar		i=0;
uchar		mmm=0;
/*-------------------------------------------------
 *	函数名：interrupt ISR
 *	功能： 	 接收中断和发送中断
 *	输入：	 无
 *	输出： 	 无
 --------------------------------------------------*/
void interrupt ISR(void)
{
    //中断处理程序
    if(UR1RXNE&&UR1RXNEF)			//接收中断
    {
    	uart_receive_input(UR1DATAL);// TODO:
        receivedata[mmm++] = UR1DATAL;
        
        if(mmm>=10)
        {
            mmm=0;
        }
        NOP();
	}
    
	if(UR1TCEN&&UR1TCF)					//发送中断
    {
        UR1TCF=1;
        
        if(i<10)
        {
            UR1DATAL =toSend[i++];
		}
        else
        {
            i=0;
		}
        NOP();
    }
}
/*-------------------------------------------------
 *	函数名：POWER_INITIAL
 *	功能： 	 上电系统初始化
 *	输入：	 无
 *	输出： 	 无
 --------------------------------------------------*/
 void POWER_INITIAL(void)
 {
	OSCCON=0B01110001;		//系统时钟选择为内部振荡器16MHz,分频比为1:1
    
	INTCON=0;							//禁止所有中断
    
    PORTA=0B00000000;
    PORTB=0B00000000;
    PORTC=0B00000000;
    
	WPUA=0B00000000;			//弱上拉的开关，0-关，1-开		
	WPUB=0B00000000;
	WPUC=0B00000000;	

	WPDA=0B00000000;			//弱下拉的开关，0-关，1-开
	WPDB=0B00000000;
	WPDC=0B00000000;
	
	TRISA=0B11011111;			//输入输出设置，0-输出，1-输入 PA5 out PA6 input
	TRISB=0B11111111;			
	TRISC=0B00000011;

	PSRC0=0B11111111;			//源电流设置最大
	PSRC1=0B11111111;
	PSRC2=0B00001111;

	PSINK0=0B11111111;			//灌电流设置最大
	PSINK1=0B11111111;
	PSINK2=0B00000011;

	ANSELA=0B00000000;			//设置对应的IO为数字IO	
 }
 /*-------------------------------------------------
 *	函数名：	DelayUs
 *	功能： 	 	短延时函数
 *	输入参数：Time延时时间长度 延时时长Time Us
 *	返回参数： 无
 --------------------------------------------------*/
 void DelayUs(uchar Time)
 {
     uchar a;
     for(a=0;a<Time;a++)
     {
         NOP();
	 } 
}
 /*-------------------------------------------------
 *	函数名：	DelayMs
 *	功能： 	 	短延时函数
 *	输入参数：Time延时时间长度 延时时长Time ms
 *	返回参数： 无
 --------------------------------------------------*/
 void DelayMs(uchar Time)
 {
     uchar a,b;
     for(a=0;a<Time;a++)
     {
         for(b=0;b<5;b++)
         {
             DelayUs(197);
         }
     }
 }
 /*-------------------------------------------------
 *	函数名：	DelayS
 *	功能： 	 	短延时函数
 *	输入参数：Time延时时间长度 延时时长Time S
 *	返回参数： 无
 --------------------------------------------------*/
 void DelayS(uchar Time)
 {
     uchar a,b;
     for(a=0;a<Time;a++)
     {
         for(b=0;b<10;b++)
         {
             DelayMs(100);
         }
     }
 }
 /*-------------------------------------------------
 *	函数名：UART_INITIAL
 *	功能： 	 初始化串口
 *	输入：	 无
 *	输出： 	 无
 --------------------------------------------------*/
 void UART_INITIAL(void)
 {
    PCKEN|=0B00100000;			//使能UART1模块时钟
    UR1IER=0B00100001;			//使能发送完成中断，使能接收数据中断
    UR1LCR=0B00000001;		//8位数据长度，1位停止位，无奇偶校验位
    UR1MCR=0B00011000	;		//使能发送和接收接口
       
    UR1DLL=104;						//波特率=Fmaster/(16*{URxDLH,URxDLL})=9600
    UR1DLH=0;  
    UR1TCF=1;
    INTCON=0B11000000;
 }

/*-------------------------------------------------
 *	函数名：main
 *	功能：	 主函数 
 *	输入：	 无
 *	输出： 	 无
 --------------------------------------------------*/
void main(void)
{
    POWER_INITIAL();		//系统初始化
  //  UART_INITIAL();
   // wifi_protocol_init();
    DelayMs(100);
  	UART_TX =   1;

	#if 0
    if(UR1TXEF)						//上电发送10+1个数据
    {
        UR1DATAL=0XAA;
    }
    #endif
    while(1)
    {
    	//wifi_uart_service();
    	DelayMs(100);
    	send_a_byte(0xA5);
		DelayMs(100);
    }
}
