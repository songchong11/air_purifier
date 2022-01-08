#ifndef	_PUBLIC_H_
#define	_PUBLIC_H_

#include "SYSCFG.h"
#include "uart.h"
#include "string.h"
#include "wifi.h"
#include "mcu_api.h"
#include "protocol.h"

#define 	unchar     	unsigned char 
#define 	unint         unsigned int
#define  	unlong 		unsigned long



#define 	UART_TX		PB2  //PB2 used for IO UART TX
#define 	UART_RX		PA1  //PA1 used for IO UART RX
#define		DELAY_104US	80	 //baud 9600 0.104ms

enum{
	COM_START_BIT, //ֹͣλ
	COM_D0_BIT, //bit0
	COM_D1_BIT, //bit1
	COM_D2_BIT, //bit2
	COM_D3_BIT, //bit3
	COM_D4_BIT, //bit4
	COM_D5_BIT, //bit5
	COM_D6_BIT, //bit6
	COM_D7_BIT, //bit7
	COM_STOP_BIT, //bit8
};

extern unchar ReadAPin;
extern unchar recvStat;
extern unchar recvData;


typedef struct {
  unsigned char  switcher;  //开关
  unsigned long  pm25; //PM2.5
  unsigned char  mode; //当前模式
  unsigned char  anion ; //当前负离子
  unsigned long  temp_indoor;//温度
  unsigned long  humidity_indoor;//湿度
  unsigned long  tovc_indoor;//
  unsigned char  air_quality;//空气质量
  unsigned char  unit_convert;//温标切换
} AIR_PURIFIER;

extern AIR_PURIFIER air_purif;

extern void PA1_Level_Change_INITIAL(void);
extern  unsigned char  TM0_FLAG ;
extern void DelayUs(unsigned char Time);
extern void send_a_byte(unchar input);



#define BIT(n)                  (1<<(n))
#define BIT_SET(x, n)         	((x) |=  BIT(n))
#define BIT_CLR(x, n)       	((x) &= ~ BIT(n))
#define BIT_IS_SET(x, n)   		((x) & BIT(n))


#endif
