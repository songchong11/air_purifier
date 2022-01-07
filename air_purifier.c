//Project: air_purifier.prj
// Device: FT62F13X
// Memory: Flash 3KX14b, EEPROM 128X8b, SRAM 256X8b
// Author: 
//Company: 
//Version:
//   Date: 
/*  文件名：test61F13x_IO.c
*	功能：  FT61F13x-IO功能演示
*   IC:    FT61F13 SOP20
*   晶振：  16M/2T                    
*   说明：  当DemoPortIn悬空或者高电平时，
*		   DemoPortOut输出25Hz占空比50%的波形
*		   当DemoPortIn接地时，DemoPortOut输出高电平
注意：因为PA6是烧录口的CLK，当用PA6做烧录口仿真时，此程序必须断开烧录口测试验证，断开后要断电VCC复位单片机。
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
#include 	"public.h"
//===========================================================
//Variable definition
//===========================================================
//宏定义****************************************************
#define 	unchar     	unsigned char 
#define 	unint         unsigned int
#define  	unlong 		unsigned long

unchar recvData;
unchar rx_buff[20] = {0};
unchar rx_cnt = 0;
/*-------------------------------------------------
 *  函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{ 
	OSCCON = 0B01110001;	//WDT 32KHZ IRCF=111=16MHZ/2=8MHZ,0.125US/T
					 		//Bit0=1,系统时钟为内部振荡器
					 		//Bit0=0,时钟源由FOSC<2：0>决定即编译选项时选择

	INTCON = 0;  			//暂禁止所有中断
	PORTA  = 0B00000000;		
	TRISA  = 0B11111110;	//PA输入输出 0-输出 1-输入
							//PA0 设置为输出
	PORTB  = 0B00000000; 	
	TRISB  = 0B01110011;	//PB输入输出 0-输出 1-输入  
											//PB2配置为输出
                                            //PB3配置为输出
	PORTC  = 0B00000000; 	
	TRISC  = 0B11111111;	//PC输入输出 0-输出 1-输入  
								
	WPUA   = 0B00000010;    //PA端口上拉控制 1-开上拉 0-关上拉
							//开PA1上拉
	WPUC   = 0B00000000;    //PC端口上拉控制 1-开上拉 0-关上拉
							//60系列PC口无上拉	
                            
	OPTION = 0B00001000;	//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
					 		//Bit7(PAPU)=0 ENABLED PULL UP PA
                             
//    PSRCA  = 0B11111111;  //源电流设置最大
//    PSRCC  = 0B11111111; 
//    PSINKA = 0B11111111;    //灌电流设置最大
//    PSINKC = 0B11111111;
                      
//    MSCON  = 0B00110000;		   	
	//BIT5:PSRCAH4和PSRCA[4]共同决定源电流。00：4mA; 11: 33mA; 01、10:8mA
	//BIT4:PSRCAH3和PSRCA[3]共同决定源电流。00：4mA; 11: 33mA; 01、10:8mA
	//BIT3:UCFG1<1:0>为01时此位有意义。0：禁止LVR；1：打开LVR
	//BIT2:快时钟测量慢周期的平均模式。0：关闭平均模式；1：打开平均模式
	//BIT1:0：关闭快时钟测量慢周期；1：打开快时钟测量慢周期
	//BIT0:0：睡眠时停止工作：1： 睡眠时保持工作。当T2时钟不是选择指令时钟的时候
    
	}
  
/*----------------------------------------------------
 *	函数名称：DelayUs
 *	功能：   短延时函数 --16M-2T--大概快1%左右.
 *	输入参数：Time延时时间长度 延时时长Time Us
 *	返回参数：无 
 ----------------------------------------------------*/
void DelayUs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		NOP();
	}
}                  
/*----------------------------------------------------
 *	函数名称：DelayMs
 *	功能：   短延时函数
 *	输入参数：Time延时时间长度 延时时长Time ms
 *	返回参数：无 
 ----------------------------------------------------*/
void DelayMs(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<5;b++)
		{
		 	DelayUs(197); 	//快1%
		}
	}
}
/*----------------------------------------------------
 *	函数名称：DelayS
 *	功能：   短延时函数
 *	输入参数：Time 延时时间长度 延时时长Time S
 *	返回参数：无 
 ----------------------------------------------------*/
void DelayS(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<10;b++)
		{
		 	DelayMs(100); 
		}
	}
}
//===========================================================
//Funtion name：interrupt ISR
//parameters：无
//returned value：无
//===========================================================
void interrupt ISR(void)
{
		#if 1
		  //定时器0的中断处理**********************
		if(T0IE && T0IF)                //8.192ms翻转一次≈60Hz
		{
				TMR0 = TIMER0_RELOAD_VALUE;               //注意:对TMR0重新赋值TMR0在两个周期内不变化

				T0IF = 0;
				
				TM0_FLAG=1;//清传输标志
				//PB2 = ~PB2; //debug
  
				/*****Receive byte******/
				recvStat++; //改变状态机
				if(recvStat == COM_STOP_BIT) //收到停止位
				{
                     rx_buff[rx_cnt++] = recvData;
					 T0IE = 0; 			//关闭定时器
					 T0IF = 0;
  
                     PAIE = 1;  			//开启PA中断
                     IOCA1 =1;  			//开启PA1电平变化中断
					 PB3 = 1;
					return; //并返回
				}
  
				if(UART_RX) //'1'
				{
					recvData |= (1 << (recvStat - 1));
                    PB3 = 1;
				}
				else //'0'
				{
					recvData &= ~(1 <<(recvStat - 1));
                    PB3 = 0;
				}
				
		}
        
	  //PA电平变化中断**********************
		 if(PAIE && PAIF)
		{
				ReadAPin = PORTA; 	//读取PORTA数据清PAIF标志
				PAIE = 0;  			//暂先禁止PA中断
				
				#if 1
				if (!(PORTA  & BIT(1))) 
                {
 
              			IOCA1 =0;  			//禁止PA1电平变化中断

						if(!UART_RX) //检测引脚高低电平，如果是低电平，则说明检测到下升沿
						{
							if(recvStat == COM_STOP_BIT) //状态为停止位
							{
								recvData = 0;
								recvStat = COM_START_BIT; //接收到开始位
								DelayUs(5); //延时一定时间
                                
								TMR0 = TIMER0_RELOAD_VALUE;  
                                T0IF = 0;
								T0IE = 1; //打开定时器，接收数据

                                 PB3 = 0; //debug
							}
							PAIF = 0;  			//清PAIF标志位
						}      
				}
                #endif
		}
		#endif
}
/*-------------------------------------------------
 *  函数名: main 
 *	功能：  主函数
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void main(void)
{
	POWER_INITIAL();			//系统初始化
	TIMER0_INITIAL();
	PA1_Level_Change_INITIAL();

	//printf("air purifier progect init\r\n");
	GIE  = 1; 				//开中断
    
    UART_TX = 1; //debug
    rx_cnt = 0;
    recvData = 0;
    memset(rx_buff, 0, sizeof(rx_buff));
    //===============================================================	
	while(1)
	{

		//send_a_byte(0x1A);
#if 1
		//	if (rx_cnt == 6 && rx_buff[0] == 0xaa && rx_buff[5] == 0xf6) {
               if (rx_cnt == 1){
				for (int i = 0; i < rx_cnt; i++) {
                    send_a_byte(recvData);
					send_a_byte(rx_buff[i]);
				}

                 memset(rx_buff, 0, sizeof(rx_buff));
				rx_cnt = 0;
			}
  #endif
	}
}
//===========================================================
