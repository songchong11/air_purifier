#ifndef	_PUBLIC_H_
#define	_PUBLIC_H_

#include "SYSCFG.h"
#include "uart.h"

#define 	unchar     	unsigned char 
#define 	unint         unsigned int
#define  	unlong 		unsigned long



#define  	PRINTF_DEBUG		1

#if PRINTF_DEBUG
#include "printf.h"
											//baud rat is 115200
#define 	PRINTF_IO	PB7  //PB2 used for IO UART TX
#endif


#define 	UART_TX		PB2  //PB2 used for IO UART TX

extern  unsigned char  TM0_FLAG ;
extern void DelayUs(unsigned char Time);


extern void send_a_byte(unchar input);

#endif
