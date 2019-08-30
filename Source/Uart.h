/****************************************************************************
 *  File Name  : GSM.h
 *  Created on : Oct 20, 2018
 *  Author     : Gaurav Choubey
 ****************************************************************************/

#ifndef __UART_A0_H_
#define __UART_A0_H_
/*****************************************************************************
 *  function
 *****************************************************************************/




/*****************************************************************************
 * Varibale
 *****************************************************************************/
#define GSM_Buffer_Size    512
unsigned int GSM_Buffer_Index;


bool Start_Buffering;
bool GSM_Buffer_Excced;
long tempsendAddr;

extern char GSM_Buffer[GSM_Buffer_Size];
/*****************************************************************************
 * macros => 0x6000-0x6FFF
 *****************************************************************************/
#define UART0_PORT 				0
#define UART1_PORT 				1

#define BAUD_RATE_9600 			0
#define BAUD_RATE_115200 		1

#define UART_CLOCK_SMCLK 	    1
#define UART_CLOCK_ACLK 	    0

#define UART_CLOCK_SOURC_FREQ_16MHz   0x10
#define UART_CLOCK_SOURC_FREQ_8MHz    0x11
#define UART_CLOCK_SOURC_FREQ_1MHz   0x12
#define UART_CLOCK_SOURC_FREQ_32KHz  0x13

/*****************************************************************************
 * Function Declaration Required in Uart Pheripheral
 *****************************************************************************/
void Init_Uart(unsigned int Uart_port, unsigned int Baud_rate,unsigned char clock_source,char clk_freq);
void Tx_Uart0_Char(unsigned char Tx_Data);
void Tx_Uart0_String(char * Str);
void Tx_Uart1_Char(unsigned char Tx_Data);
void Tx_Uart1_String(char * Str);


#endif
