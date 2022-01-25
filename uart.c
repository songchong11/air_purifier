//Project: air_purifier.prj
// Device: FT62F13X
// Memory: Flash 3KX14b, EEPROM 128X8b, SRAM 256X8b
// Author: 
//Company: 
//Version:
//   Date: 
/*  文件名 uart.c
*	功能： IO模拟uart时序
*   IC:    FT61F13 SOP20
*   晶振：  16M/2T                    
*   说明： 波特率9600, 传输1bit的时间约等于0.104ms
*		        数据由1bit起始位+8bit数据位+1bit停止位共10bit的数据帧组成，起始位为低电平，停止位为高电平
*
*
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
#include	"public.h"

//===========================================================
//Variable definition


#if CONFIG_HW_UART

void hw_uart_send_a_byte(uchar input)
{
	if(UR1TXEF)
	{
		UR1DATAL = input;
		DelayMs(1);		// TODO:check
		DelayUs(104);
	}
}
#endif

#if CONFIG_IO_UART
void io_uart_send_a_byte(uchar input)
{
		//发送启始位
		uchar i=8;

		UART_TX = 0;
        DelayUs(DELAY_104US);

		//发送8位数据位
		while(i--)
		{
				UART_TX=(input&0x01);//先传低位
				 
				DelayUs(DELAY_104US);
				 
				input=input>>1;
		}
		 
		//发送校验位(无)
		//发送结束位
		UART_TX= 1;
		DelayUs(DELAY_104US);;

}

void send_data_to_display_board(uchar *data)
{
	uchar i;
	uchar tmp_data[8] = {0};

	tmp_data[0] = UART_CMD_START;
	tmp_data[1] = WIFI_SEND_CMD_TYPE;
	tmp_data[2] = data[0];
	tmp_data[3] = data[1];
	tmp_data[4] = data[2];
	tmp_data[5] = data[3];
	tmp_data[6] = data[4];

	/*calc CRC*/
	for (i = 0; i < (CMD_LEN - 1); i++)
		tmp_data[7] += tmp_data[i];

	tmp_data[7] = ~tmp_data[7] + 1;

	for (i = 0; i < CMD_LEN; i++)
		io_uart_send_a_byte(tmp_data[i]);

}
#endif

//===========================================================
