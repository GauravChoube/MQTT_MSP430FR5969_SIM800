/*
 * uart_a0.c
 *
 *  Created on: May 3, 2017
 *      Author: gaurav
 */

/********************************************************************************
 *     Include file
 */
#include "header.h"

/********************************************************************************
 *     Extern variable
 */

/********************************************************************************
 *     Global variable for This file
 */
char ch = 0;

#pragma PERSISTENT(GSM_Buffer)
char GSM_Buffer[GSM_Buffer_Size] ={0};
/********************************************************************************
 *    Function Defination Start
 */


void Init_Uart(unsigned int Uart_port, unsigned int Baud_rate,unsigned char clock_source,char clk_freq)
{
    //INITIALISATION OF HARDWARE SETTINGS FOR UARTA0 RX0 & TX0 PINS
    if(Uart_port==UART0_PORT)
    {
        P2SEL1 |= BIT0 | BIT1;
        P2SEL0 &= ~(BIT0 | BIT1);
        UCA0CTLW0 = UCSWRST;   // Put eUSCI in reset

        switch(Baud_rate)
        {
        case BAUD_RATE_115200:
        {

            switch(clock_source)
            {
            case UART_CLOCK_ACLK:Debug_Uart_Str("\nDB:This Clock Source Not possible for Buad 115200"); break;
            case UART_CLOCK_SMCLK:
            {
                UCA0CTL1 |= UCSSEL__SMCLK;
                switch(clk_freq)
                {
                case UART_CLOCK_SOURC_FREQ_16MHz:
                {
                    UCA0BRW = 8;
                    UCA0MCTLW = 0xF7A1;

                }break;
                case UART_CLOCK_SOURC_FREQ_8MHz:
                {
                    UCA0BRW = 4;                              // 8000000/115200 = 69.444   UCA0BRW=69.44/16=4  when U016=1
                    UCA0MCTLW = 0x5551;

                }break;
                case UART_CLOCK_SOURC_FREQ_1MHz:
                {
                    UCA0BRW = 8;                              // 8000000/115200 = 69.444   UCA0BRW=69.44/16=4  when U016=1
                    UCA0MCTLW = 0xD600;

                }break;

                }

            }break;

            }

        }break;

        case BAUD_RATE_9600:
        {

            switch(clock_source)
            {
            case UART_CLOCK_ACLK:
                UCA0CTL1 |= UCSSEL__ACLK;

                UCA0BRW =3;
                UCA0MCTLW = 0x9200;

                break;
            case UART_CLOCK_SMCLK:
            {
                UCA0CTL1 |= UCSSEL__SMCLK;
                switch(clk_freq)
                {
                case UART_CLOCK_SOURC_FREQ_16MHz:
                {
                    UCA0BRW = 104;
                    UCA0MCTLW = 0xD621;

                }break;
                case UART_CLOCK_SOURC_FREQ_8MHz:
                {
                    UCA0BRW = 52;                              // 8000000/115200 = 69.444   UCA0BRW=69.44/16=4  when U016=1
                    UCA0MCTLW = 0x4911;

                }break;
                case UART_CLOCK_SOURC_FREQ_1MHz:
                {
                    UCA0BRW = 6;                              // 8000000/115200 = 69.444   UCA0BRW=69.44/16=4  when U016=1
                    UCA0MCTLW = 0x2081;

                }break;

                }
            }


            }

        }

        }

        UCA0CTL1 &= ~UCSWRST;
        UCA0IE |= UCRXIE;
    }
    else
    {

        P2SEL1 |= BIT5 | BIT6;
        P2SEL0 &= ~(BIT5 | BIT6);
        UCA1CTLW0 = UCSWRST;

        switch(Baud_rate)
        {
        case BAUD_RATE_115200:
        {

            switch(clock_source)
            {
            case UART_CLOCK_ACLK:Debug_Uart_Str("\nDB:This Clock Source Not possible for Buad 115200"); break;
            case UART_CLOCK_SMCLK:
            {
                UCA1CTL1 |= UCSSEL__SMCLK;
                switch(clk_freq)
                {
                case UART_CLOCK_SOURC_FREQ_16MHz:
                {
                    UCA1BRW = 8;
                    UCA1MCTLW = 0xF7A1;

                }break;
                case UART_CLOCK_SOURC_FREQ_8MHz:
                {
                    UCA1BRW = 4;                              // 8000000/115200 = 69.444   UCA0BRW=69.44/16=4  when U016=1
                    UCA1MCTLW = 0x5551;

                }break;
                case UART_CLOCK_SOURC_FREQ_1MHz:
                {
                    UCA1BRW = 8;                              // 8000000/115200 = 69.444   UCA0BRW=69.44/16=4  when U016=1
                    UCA1MCTLW = 0xD600;

                }break;

                }

            }break;

            }

        }break;

        case BAUD_RATE_9600:
        {

            switch(clock_source)
            {
            case UART_CLOCK_ACLK:
                UCA1CTL1 |= UCSSEL__ACLK;

                UCA1BRW =3;
                UCA1MCTLW = 0x9200;

                break;
            case UART_CLOCK_SMCLK:
            {
                UCA1CTL1 |= UCSSEL__SMCLK;
                switch(clk_freq)
                {
                case UART_CLOCK_SOURC_FREQ_16MHz:
                {
                    UCA1BRW = 104;
                    UCA1MCTLW = 0xD621;

                }break;
                case UART_CLOCK_SOURC_FREQ_8MHz:
                {
                    UCA1BRW = 52;                              // 8000000/115200 = 69.444   UCA0BRW=69.44/16=4  when U016=1
                    UCA1MCTLW = 0x4911;

                }break;
                case UART_CLOCK_SOURC_FREQ_1MHz:
                {
                    UCA1BRW = 6;                              // 8000000/115200 = 69.444   UCA0BRW=69.44/16=4  when U016=1
                    UCA1MCTLW = 0x2081;

                }break;

                }
            }


            }

        }

        }
        UCA1CTL1 &= ~UCSWRST;
        UCA1IE |=UCRXIE;


    }

//		if(Baud_rate == BAUD_RATE_115200)
//		{
//                        //BUAD_RATE_115200
//
//                       if(clock_source == UART_CLOCK_ACLK)        //Aclk running on 32lhz or 10khz
//                       {
//                         //Debug_Msg(UART_CANT_INIT_BAUD);
//                       }
//                       else
//                       {
//                        UCA0CTL1 |= UCSSEL__SMCLK;                // CLK = SMCLK
//                        UCA0BRW = 4;                              // 8000000/115200 = 69.444   UCA0BRW=69.44/16=4  when U016=1
//                        UCA0MCTLW = 0x5551;                       // 8000000/115200 - INT(1000000/115200)=0.44,UCBRSx value = 0x55(See UG)
//
//                       }
//		}
//		else
//		{
//                    //BUAD_RATE_9600
//                    if(clock_source == UART_CLOCK_ACLK)   //aclk running on 32khz
//                    {
//                        UCA0CTL1 |= UCSSEL__ACLK;
//                        UCA0BRW =3;                          //32768/9600=3.41
//                        UCA0MCTLW = 0x9200;                  // 0.41 ~ 0x92 (UG)
//                    }
//                    else
//                    {
//                        UCA0CTL1 |= UCSSEL__SMCLK;
//                        UCA0BRW =52;
//                        UCA0MCTLW = 0x4911;
//
//                    }
//
//
//		}
//
//		 UCA0CTL1 &= ~UCSWRST;
//		 UCA0IE |= UCRXIE;
//	}
//	else if(Uart_port==UART1_PORT)
//	{
//	    P2SEL1 |= BIT5 | BIT6;
//	    P2SEL0 &= ~(BIT5 | BIT6);
//	    UCA1CTLW0 = UCSWRST;
//	    if(Baud_rate == BAUD_RATE_115200)
//	            {
//	                            //BUAD_RATE_115200
//
//	                           if(clock_source == UART_CLOCK_ACLK)        //Aclk running on 32lhz or 10khz
//	                           {
//	                             //Debug_Msg(UART_CANT_INIT_BAUD);
//	                           }
//	                           else
//	                           {
//	                            UCA1CTL1 |= UCSSEL__SMCLK;                // CLK = SMCLK
//	                            UCA1BRW = 4;                              // 8000000/115200 = 69.444   UCA0BRW=69.44/16=4  when U016=1
//	                            UCA1MCTLW = 0x5551;                       // 8000000/115200 - INT(1000000/115200)=0.44,UCBRSx value = 0x55(See UG)
//
//	                           }
//	            }
//	            else
//	            {
//	                        //BUAD_RATE_9600
//	                        if(clock_source == UART_CLOCK_ACLK)   //aclk running on 32khz
//	                        {
//	                            UCA1CTL1 |= UCSSEL__ACLK;
//	                            UCA1BRW =3;                          //32768/9600=3.41
//	                            UCA1MCTLW = 0x9200;                  // 0.41 ~ 0x92 (UG)
//	                        }
//	                        else
//	                        {
//	                            UCA1CTL1 |= UCSSEL__SMCLK;
//	                            UCA1BRW =52;
//	                            UCA1MCTLW = 0x4911;
//
//	                        }
//
//	            }
//
//	             UCA1CTL1 &= ~UCSWRST;
//	             UCA1IE |=UCRXIE;
//    }
}
/*TRANSMIT_DATA_CHAR_FUNCTION*/
//*****************************************************************************
//
//! \brief Transmits a byte from the UART Module.
//!
//! This function will place the supplied data into UART transmit data register
//! to start transmission
//!
//!\param char Tx_Data data to be transmitted from the UART module
//!
//! \return None
//
//*****************************************************************************
void Tx_Uart0_Char(unsigned char Tx_Data)
{
	// Check for empty tx buffer Status
	while(!(UCA0IFG & UCTXIFG));
	// Write 8-bit data from string to Transmit Buffer Register:UCA0TXBUF
	UCA0TXBUF = Tx_Data;
}

/*TRANSMITING_STRING_FUCTION*/
//*****************************************************************************
//
//! \brief Transmits a string from the UART Module.
//!
//! This function will place the supplied data into UART transmit data register
//! to start transmission
//!
//!\param char *Str String to be transmitted from the UART module
//!
//! \return None
//
//*****************************************************************************
void Tx_Uart0_String(char * Str)
{
	while(*Str!= '\0')
	{
		Tx_Uart0_Char(*Str);
		Str++;
	}
}

/*TRANSMIT_DATA_CHAR_FUNCTION*/
//*****************************************************************************
//
//! \brief Transmits a byte from the UART Module.
//!
//! This function will place the supplied data into UART transmit data register
//! to start transmission
//!
//!\param char Tx_Data data to be transmitted from the UART module
//!
//! \return None
//
//*****************************************************************************
void Tx_Uart1_Char(unsigned char Tx_Data)
{
	// Check for empty tx buffer Status
	while(!(UCA1IFG & UCTXIFG));
	// Write 8-bit data from string to Transmit Buffer Register:UCA0TXBUF
	UCA1TXBUF = Tx_Data;
}

/*TRANSMITING_STRING_FUCTION*/
//*****************************************************************************
//
//! \brief Transmits a string from the UART Module.
//!
//! This function will place the supplied data into UART transmit data register
//! to start transmission
//!
//!\param char *Str String to be transmitted from the UART module
//!
//! \return None
//
//*****************************************************************************
void Tx_Uart1_String(char * Str)
{
	while(*Str!= '\0')
	{
		Tx_Uart1_Char(*Str);
		Str++;
	}
}

/////////////////////////////////////////////////////////////////////////////////



#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{


    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
       {
       case USCI_NONE: break;
       case USCI_UART_UCRXIFG:
       {
           UCA1TXBUF = UCA0RXBUF;

       }
       break;
       case USCI_UART_UCTXIFG: break;
       case USCI_UART_UCSTTIFG: break;
       case USCI_UART_UCTXCPTIFG: break;
       }

}

#pragma vector = USCI_A1_VECTOR
__interrupt void GSM_UART(void)
{

    switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
    {
        ch = UCA1RXBUF;

        if(GSM_Buffer_Index < GSM_Buffer_Size)
        {
            GSM_Buffer[GSM_Buffer_Index++]= ch;
        }
//        else
//        {
//
//            if(GSM_Buffer_Excced == false)
//            {
//
//                TA0CCR0 = 4096;
//
//                GSM_Buffer_Excced = true;
//                tempsendAddr = Extended_GSMBuffer_SA;
//                SingleBytewrite(tempsendAddr++, ch);
//            }
//            else
//            {
//                if(tempsendAddr < Extended_GSMBuffer_EA )
//                    SingleBytewrite(tempsendAddr++, ch);
//            }
//
//        }
        TA0CTL |=MC_3;;
        Start_Buffering=false;

    }
   break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
}

}
