/********************************************************************************************************
 * @file	printf.c
 *
 * @brief	This is the source file for b80
 *
 * @author	Driver Group
 * @date	2020
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/

#include "public.h"


#if PRINTF_DEBUG


typedef char* VA_LIST;
#define VA_START(list, param) (list = (VA_LIST)((int)&param + sizeof(param)))
#define VA_ARG(list, type) ((type *)(list += sizeof(type)))[-1]
#define VA_END(list) (list = (VA_LIST)0)

#ifndef   NULL
#define   NULL				0
#endif


/**
 * @brief      This function serves to foramt string.
 * @param[in]  byte  -  a byte need to print
 * @return     none.
 */
void io_putchar(unsigned char byte){
	unsigned char j = 0;

	unsigned char tmp_bit0 = 0;
	unsigned char tmp_bit1 = 1;
	unsigned char bit_s[10] = {0};

	bit_s[0] = tmp_bit0;
	bit_s[1] = (byte & 0x01)? tmp_bit1 : tmp_bit0;
	bit_s[2] = ((byte>>1) & 0x01)? tmp_bit1 : tmp_bit0;
	bit_s[3] = ((byte>>2) & 0x01)? tmp_bit1 : tmp_bit0;
	bit_s[4] = ((byte>>3) & 0x01)? tmp_bit1 : tmp_bit0;
	bit_s[5] = ((byte>>4) & 0x01)? tmp_bit1 : tmp_bit0;
	bit_s[6] = ((byte>>5) & 0x01)? tmp_bit1 : tmp_bit0;
	bit_s[7] = ((byte>>6) & 0x01)? tmp_bit1 : tmp_bit0;
	bit_s[8] = ((byte>>7) & 0x01)? tmp_bit1 : tmp_bit0;
	bit_s[9] = tmp_bit1;

	for(j = 0;j<10;j++)
	{
		DelayUs(5);//8.6us boud rat is 115200
		PRINTF_IO = bit_s[j];        //send bit0 ~ 10
	}
}


/**
 * @brief      This function serves to foramt string.
 * @param[in]  *out -  buffer to output
 * @param[in]  byte  -  a byte need to print
 * @return     none.
 */
void tl_putchar(char **out, char c)
{
	if(out)
	{
		**out = c;
		(*out)++;
	}
	else
	{
		io_putchar(c);
	}
}

#if ENABLE_PRINT_HEX

/**
 * @brief      This function serves to foramt string.
 * @param[in]  *out -  buffer to output
 * @param[in]  c  -  a number need to print
 * @return     none.
 */
void tl_putnum(char **out, unsigned char c) {
	unsigned char nib = c >> 4;
	if (nib > 9)	nib = nib + 87;
	else		nib = nib + 48;
	tl_putchar(out, nib);

	nib = c & 15;
	if (nib > 9)	nib = nib + 87;
	else		nib = nib + 48;
	tl_putchar(out, nib);
}

/**
 * @brief      This function serves to foramt string.
 * @param[in]  *out -  buffer to output
 * @param[in]  w  -  a integer need to print
 * @return     none.
 */
void tl_putnumber(char **out, unsigned int w,int len) {
	int i;
	int c = w;

	for(i=len-1;i>=0;i--)
	{
		c = w >>(i*8);
		tl_putnum(out, c);
	}
}
#endif

#if ENABLE_PRINT_INT
/**
 * @brief      This function serves to foramt string.
 * @param[in]  *out -  buffer to output
 * @param[in]  w  -  a integer need to print
 * @return     none.
 */
void tl_putint(char **out, int w)
{
	unsigned char buf[12],tmp,*p;
	int u;
	p = buf + 11;
	*p = '\0';
	if(w < 0)
	{
		u = -w;
	}
	else
	{
		u = w;
	}
	do{ // at least one time..
        tmp = u % 10;
		*--p = tmp + '0';
		u /= 10;
	}while(u);
	if (w < 0)
	{
		*--p = '-';
	}
	while(*p){
        tl_putchar(out, *p);
        p++;
    }

}
#endif

#if ENABLE_PRINT_STR

/**
 * @brief      This function serves to foramt string.
 * @param[in]  *out -  buffer to output
 * @param[in]  *str  -  string need to print
 * @return     none.
 */
void  tl_putstring(char **out, char * str)
{
    char *s;
	s = str;
	if(s == NULL){
        s = "(null)";
    }
	while(*s){
           tl_putchar(out, *s);
           s++;
	}
}
#endif
/**
 * @brief      This function serves to foramt string.
 * @param[in]  *out -  buffer to output
 * @param[in]  *f -  string need to format
 * @param[in]  a  -  string need to print
 * @return     none.
 */
const char *tl_format_msg(char **out, const char *f, int a)
{
	char c;
	int fieldwidth = 0;
	int flag = 0;
	//when the character is not NUL(ASCAII value =0x00)
	while ((c = *f++) != 0)
	{
		if (c >= '0' && c <= '9')
		{
			//number in character is changed to number in real
			fieldwidth = (fieldwidth * 10) + (c - '0');
		}
		else
		{
			switch (c)
			{
#if ENABLE_PRINT_HEX
				case 'x':
					flag = 16;
					break;
#endif
#if ENABLE_PRINT_INT
				case 'd':
					flag = 10;
					break;
#endif
#if ENABLE_PRINT_STR

				case 's':
					flag = 99;
					break;
#endif
				default:
					tl_putchar(out, '*');
					flag = -1;
					break;
			}
		}
		if(flag!=0)
		{
			if(fieldwidth==0)  fieldwidth=8;
			break;
		}
	}
#if ENABLE_PRINT_HEX
	if(flag == 16)
	tl_putnumber(out, a,fieldwidth);
#endif
#if ENABLE_PRINT_INT
	if(flag == 10)
	tl_putint(out, a);
#endif
#if ENABLE_PRINT_STR
	if(flag==99)
	tl_putstring(out, (char *)a);
#endif
	return f;

}

/**
 * @brief      This function serves to print string
 * @param[in]  *out     -  buffer to output
 * @param[in]  *format  -  format string need to print
 * @param[in]  list   	-  variable list
 * @return     none.
 */
void tl_print(char** out, const char *format, VA_LIST list)
{
	const char *pcStr = format;

	while (*pcStr)
	{                       /* this works because args are all ints */
    	if (*pcStr == '%')
        	pcStr = tl_format_msg(out, pcStr + 1, VA_ARG(list, int));
    	else
        	tl_putchar(out, *pcStr++);
	}

	if(out) **out = '\0';
}


/**
 * @brief      This function serves to print string
 * @param[in]  *format  -  format string need to print
 * @param[in]  ...   	-  variable number of data
 * @return     none.
 */
void tl_printf(const char *format, ...)
{
	VA_LIST list;
	VA_START(list, format);

	static unsigned char re_enter_flag = 0;
	if(re_enter_flag) return;
	re_enter_flag = 1;

	tl_print(0, format, list);
	VA_END(list);

	re_enter_flag = 0;
}


/**
 * @brief      This function serves to print string to buffer
 * @param[in]  *buff    -  buffer to print
 * @param[in]  *format  -  format string need to print
 * @param[in]  ...   	-  variable number of data
 * @return     none.
 */
void tl_sprintf(char* buff, const char *format, ...)
{
	VA_LIST list;

	VA_START(list, format);
	tl_print(&buff, format, list);
	VA_END(list);
}

#endif

