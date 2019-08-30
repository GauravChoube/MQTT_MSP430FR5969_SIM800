/********************************************************************************
 *  File Name  :main.c
 *  Created on : Nov 23, 2018
 *  Author     : Gaurav Choubey
 *
 ********************************************************************************/
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

#pragma PERSISTENT(FlashWriteAdrress)
unsigned long FlashWriteAdrress= SECTOR8_ADDRESSS;

#pragma PERSISTENT(CloudSendAddress)
unsigned long CloudSendAddress= SECTOR8_ADDRESSS;

unsigned int Main_State=0;
unsigned int rval=0xff;

#pragma PERSISTENT(main_Com_Buf)
char main_Com_Buf[100] = {0};

unsigned long Sending_cnt = 0;
char tmp[15] = {0};

unsigned int l = 0,ii;
/********************************************************************************
 *    Function Defination Start
 */
int main(void)
{
	
    Init_Device();


	while(1)
	{

	    WDT_A_resetTimer(WDT_A_BASE);

	    if(Resp_received)
	    {
	        rval = Proccess_Unsolicated_Response();
	        if(Main_State == 5)
	        {
	            if(rval == TCP_CONNECTION_CLOSED)
	            {
	                Main_State = 1;
	            }
	        }
	        Resp_received = false;

	    }

//
//	    if(Sync_Occured)
//	    {
//	        Sync_Occured = 0;
//	        ii = 0;
//	    }
//
//	  if(++ii < 20)
//	    {
//
//	        l =  Read_from_flash(ConnectCallBuffer, ConnectCallBuffer_Size);
//
//	       if(l == Flash_Empty)
//	       {
////	           Sync_Occured = 0;
//	           l = 0;
//	       }
//	       else
//	       {
//	        memset(main_Com_Buf, 0x00, 100);
//	        ltoa(l, main_Com_Buf);
//	        Tx_Uart0_String("\n//*******//Flash memory read length:");
//	        Tx_Uart0_String(main_Com_Buf);
//	        Tx_Uart0_String(":");
//	        Tx_Uart0_String(ConnectCallBuffer);
//
//	        Increment_CloudSendAddress(l);
//
//	       }
//
//	    }

//	    if(Sync_Occured)
	    {
	        memset(main_Com_Buf, 0x00, sizeof(main_Com_Buf));
	        strcpy(main_Com_Buf, "Hi,Sync Occured Of a Min=>");
	        ltoa(++Sending_cnt, tmp);
	        strcat(main_Com_Buf, tmp);

	        Tx_Uart0_String("\nFlash memory write Data:");

	       l = Insert_into_flash(main_Com_Buf, strlen((char *)main_Com_Buf));

//	        l = SPI_ModifyWrite_Flash(( char *)main_Com_Buf, FlashWriteAdrress, strlen((char *)main_Com_Buf), true);
//	        FlashWriteAdrress += strlen(main_Com_Buf)+1;
//	       __delay_cycles(16000000);
	       memset(main_Com_Buf, 0x00, sizeof(main_Com_Buf));
	       SPI_MultipleContinuouseRead((unsigned char *)main_Com_Buf, FlashWriteAdrress-l-1, sizeof(main_Com_Buf));
	       Tx_Uart0_String(main_Com_Buf);
	       memset(main_Com_Buf, 0x00, sizeof(main_Com_Buf));
	       ltoa(l,main_Com_Buf);
	       Tx_Uart0_String(":length:");
	       Tx_Uart0_String(main_Com_Buf);


	        Sync_Occured = 0;
	    }

	    switch(Main_State)
	    {
	    case 0:
	    {
	        rval = Init_GSM();
	        if(rval == Function_Success)
	        {
	            Sync_Occured = 1;
	            Main_State = 1;
	        }
	    }
	    break;

	    case 1:
	    {
	        if(GSMmaxTimerOn == false)
	        {
	            rval = Check_network();
	            switch(rval)
	            {
	            case Function_Success: Main_State = 2; Stop_GSM_MaxTimer(); break;
	            case NET_SEARCHING:  Start_GSM_MaxTimer(30);
	            default:break;
	            }
	        }
	    }
	    break;

	    case 2:
	    {
	        rval =  Init_Http_TCP();
	        switch(rval)
	        {
	        case Function_Success: Main_State = 3;break;
	        case ATMaxtriedOccured:
	        case TCP_CONNECTION_CLOSED:  ShutAllConnection();
	        case Server_Busy: Main_State = 1;break;



	        }

	    }break;

	    case 3:
	    {
	        rval = Connect_To_Broker();
	        switch(rval)
	        {
	        case Function_Success :Main_State = 5;  break;
	        case ATMaxtriedOccured:
	        case TCP_CONNECTION_CLOSED:  ShutAllConnection();
	        case Server_Busy: Main_State = 1;break;

	        }

	    }break;

	    case 4:
	    {

	        rval = Send_Data_to_Cloud();
	        switch(rval)
	        {
	        case Function_Success :Main_State = 5;break;
	        case ATMaxtriedOccured:
	        case TCP_CONNECTION_CLOSED:  ShutAllConnection();
	        case Server_Busy: Main_State = 1;break;

	        }

	    }break;

	    case 5:
	    {
	        if(CloudSendAddress != FlashWriteAdrress)
	        {
	            Main_State = 4;
	        }

	    }break;

	    default:
	        break;

	    }
	}


}
