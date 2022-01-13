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

unchar ReadAPin;
unchar recvData = 0;


/*-------------------------------------------------
 *	函数名：interrupt ISR
 *	功能： 	 接收中断和发送中断
 *	输入：	 无
 *	输出： 	 无
 --------------------------------------------------*/
void interrupt ISR(void)
{

    //定时器4的中断处理程序
    if(T4UIE&&T4UIF)			
    {
        T4UIF=1;							//写1清零标志位
        DEBUG_IO_PB5 = ~DEBUG_IO_PB5;		//翻转电平
		
		/*****Receive byte******/
		recvStat++; //改变状态机
		if(recvStat == COM_STOP_BIT) //收到停止位
		{
			// rx_buff[rx_cnt++] = recvData;
			 uart_receive_input(recvData);
			 //T4UIE = 0;	//关定时器4 
			 TIM4IER=0B00000000;

			 EPIF0|=0X40;				//写1清零标志位
			 EPIE0=0B01000000;		//禁止中断6
			 //BIT_SET(EPIE0, 6);		//开启外部中断PA6
			 ReadAPin = PORTA;		 //清PA电平变化中断 must
			 DEBUG_IO_PA1 = 1;//debug
			 return; //并返回
		}
		
		if(UART_RX) //'1'
		{
			recvData |= (1 << (recvStat - 1));
			DEBUG_IO_PA1 = 1;
		}
		else //'0'
		{
			recvData &= ~(1 <<(recvStat - 1));
			DEBUG_IO_PA1 = 0;
		}
    }



	if(EPIF0&0X40)					
    {
       EPIE0=0B00000000;		//禁止中断6

       EPIF0|=0X40;				//写1清零6标志位
       // DEBUG_IO_PB5 = ~DEBUG_IO_PB5;		//翻转电平
	   ReadAPin = PORTA;

	   if (!(PORTA	& BIT(6))) 
	   {

		   if(!UART_RX) //检测引脚高低电平，如果是低电平，则说明检测到下升沿
		   {
			   if(recvStat == COM_STOP_BIT) //状态为停止位
			   {
				   recvData = 0;
				   recvStat = COM_START_BIT; //接收到开始位
				   TIM4ARR= T4_RELOAD_VALUE;  	//自动装载值
				   T4UIF =1; //清T4标志
				   //T4UIE = 1;//开启T4中断
				   TIM4IER=0B00000001;
				   DEBUG_IO_PA1 = 0; //debug
			   }
			  EPIF0|=0X40;				//写1清PA6标志位
		   }	  
	   }
    }
	#if 0
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
	#endif
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
	
	TRISA=0B11011101;			//输入输出设置，0-输出，1-输入:             PA1 out PA5 out PA6 input
	TRISB=0B11011111;			//PB5 输出
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
 *	函数名：IO_INT_INITIAL
 *	功能：	 IO中断初始化 
 *	输入：	 无
 *	输出： 	 无
 --------------------------------------------------*/
 void IO_INT_INITIAL(void)
 {
     EPS0=0B00000000;		
     EPS1=0B00000000;		//选择PA6管脚中断
     ITYPE0=0B00000000;		//双边沿中断
     ITYPE1=0B00100000;
     EPIE0=0B01000000;		//使能中断6
     INTCON=0B11000000;		//使能总中断和外设中断
 }

 /*-------------------------------------------------
 *	函数名：TIM4_INITIAL
 *	功能： 	 初始化TIM4
 *	输入：	 无
 *	输出： 	 无
 --------------------------------------------------*/
 void TIM4_INITIAL(void)
 {
    PCKEN|=0B00001000;			//使能TIMER4模块时钟
    TIM4CR1=0B00000101;			//允许自动装载，使能计数器
    TIM4IER=0B00000001;			//不允许更新中断
    TIM4SR=0B00000000;	
    TIM4EGR=0B00000000;	
    TIM4CNTR=0;  
    TIM4PSCR=0B00000011;		//预分频器的值 //4分频
    TIM4ARR= T4_RELOAD_VALUE;  	//自动装载值
    INTCON|=0B11000000;		//开总中断和外设中断
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
  	TIM4_INITIAL();
	IO_INT_INITIAL();
 
    DelayMs(100);
   // wifi_protocol_init();
  	UART_TX =   1;

	DEBUG_IO_PA1 = 1;

    while(1)
    {
    	//wifi_uart_service();
    	NOP();
    	#if 0
    	DelayMs(100);
    	send_a_byte(0xA5);
    	send_a_byte(0xA7);
    	send_a_byte(0xb5);
    	send_a_byte(0xc9);
		DelayMs(100);
    	send_a_byte(0x14);
    	send_a_byte(0x7a);
    	send_a_byte(0x1d);
    	send_a_byte(0x94);
		#endif
    }
}
