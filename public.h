#ifndef	_PUBLIC_H_
#define	_PUBLIC_H_

#include "SYSCFG.h"
#include "string.h"
#include "wifi.h"
#include "mcu_api.h"
#include "protocol.h"

#define		uchar		unsigned char
#define		uint		unsigned int
#define		ulong		unsigned long


#define		DEBUG_IO_PA0		PA0
#define		DEBUG_IO_PA1		PA1

#define		WIFI_EN_GPIO		PB1

#define		CONFIG_HW_UART		1
#define		CONFIG_IO_UART		1

#define		UART_CMD_START		0xA5
#define 	CMD_LEN				8


#if CONFIG_HW_UART
#define 	UART_TX		PB5  //PB5 used for IO UART TX
#define 	UART_RX		PB4  //PA6 used for IO UART RX
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

extern uchar ReadAPin;
extern uchar recvStat;
extern uchar recvData;
#endif

#define  	PRINTF_DEBUG		1

#if PRINTF_DEBUG
#include 	"printf.h"
#define 	BAUD_1M_DELAY			//baud rat is 1000000
#define 	PRINTF_IO				DEBUG_IO_PA0  //PA0 used printf IO
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
  unsigned char  wifi_state;
} AIR_PURIFIER;

extern AIR_PURIFIER air_purif;

enum{
	DP_TYPE_START = 0,
	DP_TYPE_SWITCH,
	DP_TYPE_PM25,
	DP_TYPE_MODE,
	DP_TYPE_FAN_SPEED_ENUM,
	DP_TYPE_ANION,
	DP_TYPE_TEMP_INDOOR,
	DP_TYPE_HUMIDITY,
	DP_TYPE_TVOC,
	DP_TYPE_AIR_QUALITY,
	DP_TYPE_TEMP_UNIT_CONVERT,
};


enum{
	CMD_TYPE_WIFI_RESET = 0xA0,
	CMD_TYPE_WIFI_STATUS,
	CMD_TYPE_WIFI_TEST,
	CMD_TYPE_WIFI_TEST_RESULT,
	CMD_TYPE_SEND_DATA_TO_WIFI,
};


enum{
	WIFI_TEST_SUCCESS = 0x10,
	WIFI_TEST_SUCCESS_BUT_WAKE,
	WIFI_TEST_FAIL_NOT_SCANED_ROUTE = 0x20,
	WIFI_TEST_FAIL_NOT_AUTHORIZED,
};

extern void DelayUs(unsigned char Time);
extern void hw_uart_send_a_byte(uchar input);
extern void io_uart_send_a_byte(uchar input);
extern void send_data_to_display_board(uchar *data);
extern void DelayMs(unsigned char Time);
extern void send_back_wifi_test_result(uchar result);



#define BIT(n)                  (1<<(n))
#define BIT_SET(x, n)         	((x) |=  BIT(n))
#define BIT_CLR(x, n)       	((x) &= ~ BIT(n))
#define BIT_IS_SET(x, n)   		((x) & BIT(n))

#endif
