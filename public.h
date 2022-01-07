#ifndef	_PUBLIC_H_
#define	_PUBLIC_H_

#include "SYSCFG.h"
#include "uart.h"
#include "string.h"

#define 	unchar     	unsigned char 
#define 	unint         unsigned int
#define  	unlong 		unsigned long



#define  	PRINTF_DEBUG		0

#if PRINTF_DEBUG
#include "printf.h"
#define	  ENABLE_PRINT_HEX		0
#define	  ENABLE_PRINT_INT		0
#define	  ENABLE_PRINT_STR		1

											//baud rat is 115200
#define 	PRINTF_IO	PB7  //PB7 used printf IO
#endif


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
extern void PA1_Level_Change_INITIAL(void);
extern  unsigned char  TM0_FLAG ;
extern void DelayUs(unsigned char Time);
extern void send_a_byte(unchar input);



#define BIT(n)                  		( 1<<(n) )
#define BIT_SET(x, n)         	((x) |=  BIT(n))
#define BIT_CLR(x, n)       	((x) &= ~ BIT(n))
#define BIT_IS_SET(x, n)   		((x) & BIT(n))
#define BIT_FLIP(x, n)   		((x) ^= BIT(n))
#define BIT_SET_HIGH(x) 		((x) |=  BIT((sizeof((x))*8-1)))				// set the highest bit
#define BIT_CLR_HIGH(x) 		((x) &= ~ BIT((sizeof((x))*8-1)))				// clr the highest bit
#define BIT_IS_SET_HIGH(x) 		((x) & BIT((sizeof((x))*8-1)))				// check the higest bit

#endif
