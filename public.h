#ifndef	_PUBLIC_H_
#define	_PUBLIC_H_

#include "uart.h"

#define 	unchar     	unsigned char 
#define 	unint         unsigned int
#define  	unlong 		unsigned long

#define 	UART_TX		PB2  //PB2 used for IO UART TX
extern  unsigned char  TM0_FLAG ;


extern void send_a_byte(unchar input);

#endif