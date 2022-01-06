//Project: air_purifier.prj
// Device: FT62F13X
// Memory: Flash 3KX14b, EEPROM 128X8b, SRAM 256X8b
// Author: 
//Company: 
//Version:
//   Date: 
/*  �ļ�����test61F13x_IO.c
*	���ܣ�  FT61F13x-IO������ʾ
*   IC:    FT61F13 SOP20
*   ����  16M/2T                    
*   ˵����  ��DemoPortIn���ջ��߸ߵ�ƽʱ��
*		   DemoPortOut���25Hzռ�ձ�50%�Ĳ���
*		   ��DemoPortIn�ӵ�ʱ��DemoPortOut����ߵ�ƽ
ע�⣺��ΪPA6����¼�ڵ�CLK������PA6����¼�ڷ���ʱ���˳������Ͽ���¼�ڲ�����֤���Ͽ���Ҫ�ϵ�VCC��λ��Ƭ����
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
//�궨��****************************************************
#define 	unchar     	unsigned char 
#define 	unint         unsigned int
#define  	unlong 		unsigned long

unchar recvData;
unchar rx_buff[20] = {0};
unchar rx_cnt = 0;
/*-------------------------------------------------
 *  ��������POWER_INITIAL
 *	���ܣ�  �ϵ�ϵͳ��ʼ��
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{ 
	OSCCON = 0B01110001;	//WDT 32KHZ IRCF=111=16MHZ/2=8MHZ,0.125US/T
					 		//Bit0=1,ϵͳʱ��Ϊ�ڲ�����
					 		//Bit0=0,ʱ��Դ��FOSC<2��0>����������ѡ��ʱѡ��

	INTCON = 0;  			//�ݽ�ֹ�����ж�
	PORTA  = 0B00000000;		
	TRISA  = 0B11111110;	//PA������� 0-��� 1-����
							//PA0 ����Ϊ���
	PORTB  = 0B00000000; 	
	TRISB  = 0B01110011;	//PB������� 0-��� 1-����  
											//PB2����Ϊ���
                                            //PB3����Ϊ���
	PORTC  = 0B00000000; 	
	TRISC  = 0B11111111;	//PC������� 0-��� 1-����  
								
	WPUA   = 0B00000010;    //PA�˿��������� 1-������ 0-������
							//��PA1����
	WPUC   = 0B00000000;    //PC�˿��������� 1-������ 0-������
							//60ϵ��PC��������	
                            
	OPTION = 0B00001000;	//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
					 		//Bit7(PAPU)=0 ENABLED PULL UP PA
                             
//    PSRCA  = 0B11111111;  //Դ�����������
//    PSRCC  = 0B11111111; 
//    PSINKA = 0B11111111;    //������������
//    PSINKC = 0B11111111;
                      
//    MSCON  = 0B00110000;		   	
	//BIT5:PSRCAH4��PSRCA[4]��ͬ����Դ������00��4mA; 11: 33mA; 01��10:8mA
	//BIT4:PSRCAH3��PSRCA[3]��ͬ����Դ������00��4mA; 11: 33mA; 01��10:8mA
	//BIT3:UCFG1<1:0>Ϊ01ʱ��λ�����塣0����ֹLVR��1����LVR
	//BIT2:��ʱ�Ӳ��������ڵ�ƽ��ģʽ��0���ر�ƽ��ģʽ��1����ƽ��ģʽ
	//BIT1:0���رտ�ʱ�Ӳ��������ڣ�1���򿪿�ʱ�Ӳ���������
	//BIT0:0��˯��ʱֹͣ������1�� ˯��ʱ���ֹ�������T2ʱ�Ӳ���ѡ��ָ��ʱ�ӵ�ʱ��
    
	}
  
/*----------------------------------------------------
 *	�������ƣ�DelayUs
 *	���ܣ�   ����ʱ���� --16M-2T--��ſ�1%����.
 *	���������Time��ʱʱ�䳤�� ��ʱʱ��Time Us
 *	���ز������� 
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
 *	�������ƣ�DelayMs
 *	���ܣ�   ����ʱ����
 *	���������Time��ʱʱ�䳤�� ��ʱʱ��Time ms
 *	���ز������� 
 ----------------------------------------------------*/
void DelayMs(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<5;b++)
		{
		 	DelayUs(197); 	//��1%
		}
	}
}
/*----------------------------------------------------
 *	�������ƣ�DelayS
 *	���ܣ�   ����ʱ����
 *	���������Time ��ʱʱ�䳤�� ��ʱʱ��Time S
 *	���ز������� 
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
//Funtion name��interrupt ISR
//parameters����
//returned value����
//===========================================================
void interrupt ISR(void)
{
		  //��ʱ��0���жϴ���**********************
		if(T0IE && T0IF)                //8.192ms��תһ�Ρ�60Hz
		{
				TMR0 = TIMER0_RELOAD_VALUE;               //ע��:��TMR0���¸�ֵTMR0�����������ڲ��仯

				T0IF = 0;
				
				TM0_FLAG=1;//�崫���־
				PB2 = ~PB2; //debug
  
				/*****Receive byte******/
				recvStat++; //�ı�״̬��
				if(recvStat == COM_STOP_BIT) //�յ�ֹͣλ
				{
					 T0IE = 0; 			//�رն�ʱ��
					 T0IF = 0;
  
                     PAIE = 1;  			//����PA�ж�
                     IOCA1 =1;  			//����PA1��ƽ�仯�ж�
					 PB3 = 1;
					return; //������
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
        
	  //PA��ƽ�仯�ж�**********************
		 if(PAIE && PAIF)
		{
				ReadAPin = PORTA; 	//��ȡPORTA������PAIF��־
				PAIE = 0;  			//���Ƚ�ֹPA�ж�
				
				#if 1
				if (!(PORTA  & BIT(1))) 
                {
 
              			IOCA1 =0;  			//��ֹPA1��ƽ�仯�ж�

						if(!UART_RX) //������Ÿߵ͵�ƽ������ǵ͵�ƽ����˵����⵽������
						{
							if(recvStat == COM_STOP_BIT) //״̬Ϊֹͣλ
							{
								recvStat = COM_START_BIT; //���յ���ʼλ
								DelayUs(5); //��ʱһ��ʱ��
                                
								TMR0 = TIMER0_RELOAD_VALUE;  
                                T0IF = 0;
								T0IE = 1; //�򿪶�ʱ������������

                                 PB3 = 0; //debug
							}
							PAIF = 0;  			//��PAIF��־λ
						}      
				}
                #endif
		}
}
/*-------------------------------------------------
 *  ������: main 
 *	���ܣ�  ������
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
void main(void)
{
	POWER_INITIAL();			//ϵͳ��ʼ��
	TIMER0_INITIAL();
	PA1_Level_Change_INITIAL();

//	printf("air purifier progect init\r\n");
	GIE  = 1; 				//���ж�
	//T0IE = 0;				//����ʱ��/������0�ж�
    
    PB3 = 1; //debug
    PB2 = 1; //debug
    //===========================================================

    //===============================================================	
	while(1)
	{
		//DelayMs(1000);
		//send_a_byte(0xA5);
        if (recvData != 0 && recvStat == COM_STOP_BIT) {
                rx_buff[rx_cnt++] = recvData;
                recvData = 0;
                if (rx_cnt == 6 && rx_buff[0] == 0xaa && rx_buff[5] == 0xf6) {
                   // printf("loop %1x %1x %1x\r\n", 0xaa, 0xbb, 0xcc);
                    rx_cnt = 0;
				}
		}
        

  
	}
}
//===========================================================
