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
uchar	value_chang_type;


#if CONFIG_IO_UART
	unchar recvData = 0;
	unchar recvStat; //定义接收状态机
#endif

/*-------------------------------------------------
 *	函数名：interrupt ISR
 *	功能： 	 接收中断和发送中断
 *	输入：	 无
 *	输出： 	 无
 --------------------------------------------------*/
void interrupt ISR(void)
{
#if CONFIG_HW_UART
	//中断处理程序
	if(UR1RXNE&&UR1RXNEF)			//接收中断
	{
		uart_receive_input(UR1DATAL);
	}

	if(UR1TCEN&&UR1TCF) 				//发送中断
	{
		UR1TCF=1;						//清发送中断
		//UR1DATAL =toSend[i++];
		//NOP();
	}
#endif

#if 1
    //定时器4的中断处理程序
    if(T4UIE&&T4UIF)			
    {
        T4UIF=1;							//写1清零标志位

		/*****Receive byte******/
		recvStat++; //改变状态机
		if(recvStat == COM_STOP_BIT) //收到停止位
		{
			// rx_buff[rx_cnt++] = recvData;
			uart_receive_input(recvData);
			T4UIE = 0;	//关定时器4 

			EPIF0|=0X40; 			//写1清零6标志位
			EPIE0=0B01000000;		//开启中断6
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
       
	   EPIF0|=0X40;			 				//写1清零6标志位

	   if(!UART_RX) //检测引脚高低电平，如果是低电平，则说明检测到下升沿
	   {
		   if(recvStat == COM_STOP_BIT) //状态为停止位
		   {
			   recvData = 0;
			   recvStat = COM_START_BIT; //接收到开始位

			   EPIE0=0B00000000;	   //禁止中断6

			   TIM4CNTR=0;
			   T4UIF=1;							//写1清零标志位，否则启动timer后会立马进一次中断
			   TIM4IER=0B00000001;
			   DEBUG_IO_PA1 = 0; //debug
		   }
	   }
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
	
	TRISA=0B10111100;			//输入输出设置，0-输出，1-输入:PA0 out             PA1 out PA6 out

	TRISB=0B11011101;			// PB1 输出   PB4输入      PB5 输出
	TRISC=0B00000011;

	PSRC0=0B11111111;			//源电流设置最大
	PSRC1=0B11111111;
	PSRC2=0B00001111;

	PSINK0=0B11111111;			//灌电流设置最大
	PSINK1=0B11111111;
	PSINK2=0B00000011;

#if CONFIG_HW_UART
	AFP0 = 0B00000000; //USART1 TX select  AFP0[7:6] 00:PA6
	AFP1 = 0B00000000; //USART1 RX select  AFP1[7:6] 00:PA7
#endif
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

#if CONFIG_HW_UART
/*-------------------------------------------------
*  函数名：UART_INITIAL
*  功能：	初始化串口
*  输入：	无
*  输出：	无
--------------------------------------------------*/
void UART_INITIAL(void)
{
   PCKEN|=0B00100000;		//使能UART1模块时钟
   UR1IER=0B00100001;		//使能发送完成中断，使能接收数据中断
   UR1LCR=0B00000001;	  	//8位数据长度，1位停止位，无奇偶校验位
   UR1MCR=0B00011000;	   	//使能发送和接收接口
	  
   UR1DLL=104;				//波特率=Fmaster/(16*{URxDLH,URxDLL})=9600
   UR1DLH=0;  
   UR1TCF=1;
   INTCON=0B11000000;
}

#endif

#if CONFIG_IO_UART
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
    TIM4IER=0B00000000;			//不允许更新中断
    TIM4SR=0B00000000;	
    TIM4EGR=0B00000000;	
    TIM4CNTR=0;
    TIM4PSCR=0B00000011;		//预分频器的值 //4分频
    TIM4ARR= T4_RELOAD_VALUE;  	//自动装载值
    INTCON|=0B11000000;		//开总中断和外设中断
 }
#endif

/*-------------------------------------------------
 *	函数名：main
 *	功能：	 主函数 
 *	输入：	 无
 *	输出： 	 无
 --------------------------------------------------*/
void main(void)
{
    POWER_INITIAL();		//系统初始化
#if CONFIG_HW_UART
	UART_INITIAL();
#endif

#if CONFIG_IO_UART
	IO_INT_INITIAL();
  	TIM4_INITIAL();
  	UART_TX =  1;
	recvStat = COM_STOP_BIT;
 #endif

 	
 	WIFI_EN_GPIO = 1;
    wifi_protocol_init();
	DEBUG_IO_PA1 = 1;
	printf("air_purifier init\n");
	DelayMs(100);

    while(1)
    {
    	wifi_uart_service();

		// TODO: received wifi config cmd
#if 0
		if(wifi_config_cmd) {
			mcu_reset_wifi();
		}
#endif

#if 0
		// TODO: wifi work state indicate
		air_purif.wifi_state = mcu_get_wifi_work_state();

		switch(air_purif.wifi_state) {
			case SMART_CONFIG_STATE:
				//处 于 Smart 配 置 状 态， 即 LED 快 闪 间隔250ms
				printf("wifi is smart config state\n");
			break;

			case AP_STATE:
				//处 于 AP 配 置 状 态， 即 LED 慢 闪 间隔1500ms
				printf("wifi is AP config state\n");
			break;

			case WIFI_NOT_CONNECTED:
				//Wi-Fi 配 置 完 成， 正 在 连 接 路 由 器， 即 LED 常 暗
				printf("wifi not connected\n");
				
			break;

			case WIFI_CONNECTED:
				//路 由 器 连 接 成 功， 即 LED 常 亮
				printf("wifi connected\n");
			break;

			default:
			break;
		}
#endif


		/*****************************************************************************/
		// TODO: check 当用户操作面板改变相关模式时或者MCU检测到传感器数据变化
		// TODO:时，mcu需要主动上报数据，可以只单独上报变化的数据一次，也可以所有数据上报一次

		#if 0

		if (air_purif.wifi_state == WIFI_CONNECTED || air_purif.wifi_state == WIFI_CONN_CLOUD) {

			//value_chang_type = mcu_check_status_changed();
			value_chang_type = 0;

			switch(value_chang_type) {
				case DP_TYPE_SWITCH:
					mcu_dp_bool_update(DPID_SWITCH,air_purif.switcher); //BOOL型数据上报;
				break;

				case DP_TYPE_PM25:
					mcu_dp_value_update(DPID_PM25, air_purif.pm25); //VALUE型数据上报;
				break;

				case DP_TYPE_MODE:
					mcu_dp_enum_update(DPID_MODE,air_purif.mode); //枚举型数据上报;
				break;

				case DP_TYPE_TEMP_INDOOR:
					 mcu_dp_value_update(DPID_TEMP_INDOOR,air_purif.temp_indoor); //VALUE型数据上报;
				break;

				case DP_TYPE_HUMIDITY:
					 mcu_dp_value_update(DPID_HUMIDITY,air_purif.humidity_indoor); //VALUE型数据上报;
				break;

				case DP_TYPE_TVOC:
					mcu_dp_value_update(DPID_TVOC, air_purif.tovc_indoor); //VALUE型数据上报;
				break;

				case DP_TYPE_ANION:
					mcu_dp_bool_update(DPID_ANION,air_purif.anion); //BOOL型数据上报;
				break;

				case DP_TYPE_FAN_SPEED_ENUM:
					mcu_dp_enum_update(DPID_FAN_SPEED_ENUM,air_purif.fun_speed); //枚举型数据上报;
				break;

				case DP_TYPE_AIR_QUALITY:
					mcu_dp_enum_update(DPID_AIR_QUALITY,air_purif.air_quality); //枚举型数据上报;
				break;

				case DP_TYPE_TEMP_UNIT_CONVERT:
					mcu_dp_enum_update(DPID_TEMP_UNIT_CONVERT,air_purif.unit_convert); //枚举型数据上报;
				break;

				default:
				break;
			}

		}


	    #endif


#ifdef WIFI_TEST_ENABLE
		// TODO:使用不常用组合键长按2s进产测模式,产测命令由显示板发送过来
#if 0
		if (wifi_product_test_cmd) {
			// TODO: 产测时需要一个名字为tuya_mdev_test的路由器距离工位5m
			mcu_start_wifitest();
		}
#endif
#endif
		/*****************************************************************************/
	}
}
