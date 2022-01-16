#ifndef	_PUBLIC_H_
#define	_PUBLIC_H_

#include "SYSCFG.h"
#include "string.h"
#include "wifi.h"
#include "mcu_api.h"
#include "protocol.h"

#define 	unchar     	unsigned char 
#define 	unint       unsigned int
#define  	unlong 		unsigned long

#define		DEBUG_IO_PB5		PB5
#define		DEBUG_IO_PA1		PA1


#define		CONFIG_HW_UART	1


#if CONFIG_HW_UART
#define 	UART_TX		PA5//PA5  //PA5 used for IO UART TX
#define 	UART_RX		RA6//PA6  //PA6 used for IO UART RX
#define		DELAY_104US	100	 //baud 9600 0.104ms
#define		T4_RELOAD_VALUE	205

enum{
	COM_START_BIT = 0, //ֹͣλ
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
#else

#endif

#define  	PRINTF_DEBUG		1

#if PRINTF_DEBUG
#include "printf.h"
#define 	BAUD_1M_DELAY				//baud rat is 1000000
#define 	PRINTF_IO				DEBUG_IO_PB5  //PB5 used printf IO
#endif



typedef struct {
  unsigned char  switcher;  //开关
  unsigned long  pm25; //PM2.5
  unsigned char  mode; //当前模式
  unsigned char  fun_speed; //当前模式
  unsigned char  anion ; //当前负离子
  unsigned long  temp_indoor;//温度
  unsigned long  humidity_indoor;//湿度
  unsigned long  tovc_indoor;//
  unsigned char  air_quality;//空气质量
  unsigned char  unit_convert;//温标切换
} AIR_PURIFIER;

extern AIR_PURIFIER air_purif;

extern void DelayUs(unsigned char Time);
extern void send_a_byte(unchar input);
extern void DelayMs(unsigned char Time);



#define BIT(n)                  (1<<(n))
#define BIT_SET(x, n)         	((x) |=  BIT(n))
#define BIT_CLR(x, n)       	((x) &= ~ BIT(n))
#define BIT_IS_SET(x, n)   		((x) & BIT(n))


#endif
