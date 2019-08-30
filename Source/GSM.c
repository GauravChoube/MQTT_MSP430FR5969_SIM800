/********************************************************************************
 *  File Name  : GSM.c
 *  Created on : Oct 20, 2018
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
#pragma PERSISTENT(Common_Buffer_G)
char Common_Buffer_G[100]={0};

char reqSock[15] = {0};

unsigned char AT_cmd_num = 1;
unsigned char CheckNet_State = 1;
unsigned char Init_Tcp_state = 1;
unsigned char Init_Http_State = 1;
unsigned char Init_Http_Tcp_State = 1;
/********************************************************************************
 *    Function Defination Start
 */



unsigned int Init_GSM()
{
    unsigned int rval=0xff;


    switch(AT_cmd_num)
    {


    case 1:
        rval= SendATCmd("AT&F",AT_ATTENTION_COMMAND,10,5);

        break;

    case 2:
        rval=SendATCmd("ATE0",AT_TURN_OFF_ECHO, 2,5);

        break;


    case 3:
        rval= SendATCmd("AT+IPR=0",AT_AUTO_BAUD_ENABLE,2,5);

        break;
    case 4:
        rval=SendATCmd("AT+GSMBUSY=1",AT_REJECT_INCOMING_CALLS,2,5);

        break;

    case 5:
        rval=SendATCmd("AT+GSN",AT_REQUEST_IMEI,2,5);

        break;
    case 6:
        rval=SendATCmd("AT+CSMINS?",AT_SIM_PRESENCE_CMD,2,5);
        if(rval == SIM_CARD_ABESENT)
        {
#ifdef Debug_Enable
            ////////DebugUart("Sim Is Not Inserted.Please Check it before power on device");
#endif
            PMM_trigBOR();
        }

        break;

    case 7:
        rval=SendATCmd("AT+CIMI",AT_REQUEST_IMSI,2,5);

        break;

    case 8:
        rval=SendATCmd("AT+CCID",AT_SIM_NUMBER,2,5);

        break;

    case 9:
        rval=SendATCmd("AT+CLTS?",AT_CHECK_AUTO_TIMESTAMP_ENABLE,2,5);

        AT_cmd_num++;


        break;
    case 10:
        rval=SendATCmd("AT+CLTS=1",AT_ENABLE_TIMESTAMP_AUTO_UPDATE,2,5);

        break;

    case 11:
        rval=SendATCmd("AT+CCLK?",AT_CLOCK,2,2);

        break;

    case 12:
        rval = SendATCmd("AT+COPS = 0", AT_ATTENTION_COMMAND, 5, 5);
        break;
    case 13:
        rval = SendATCmd("AT+STKPCIS=0", AT_ATTENTION_COMMAND, 5, 5);

        break;
    case 14:
        rval=SendATCmd("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",AT_HTTP_SET_BEARER_SETTINGS, 10,5);

        break;

    case 15:

//          rval=SendATCmd("AT+SAPBR=3,1,\"APN\",\"unlimitiot\"",AT_HTTP_BEARER_APN_SETTINGS, 10, 5);
#ifdef ATT_SIM
        rval=SendATCmd("AT+SAPBR=3,1,\"APN\",\"ibot.com.attz\"",AT_HTTP_BEARER_APN_SETTINGS, 10, 5);    //AT&T Apn
#endif
#ifdef  AERIS_SIM
        rval=SendATCmd("AT+SAPBR=3,1,\"APN\",\"iot-eu.aer.net\"",AT_HTTP_BEARER_APN_SETTINGS, 10, 5);    //AT&T Apn
#endif
#ifdef SIERRA_SIM
        rval=SendATCmd("AT+SAPBR=3,1,\"APN\",\"internet.sierrawireless.com\"",AT_HTTP_BEARER_APN_SETTINGS, 10, 5); //sierra Apn
#endif
        break;

    case 16:
        //        rval = SendATCmd("AT+CSTT=\"unlimitiot\",\"\",\"\"",AT_TCP_SET_APN, 10,5);
#ifdef ATT_SIM
        rval = SendATCmd("AT+CSTT=\"ibot.com.attz\",\"\",\"\"",AT_TCP_SET_APN, 10,5);
#endif
#ifdef  AERIS_SIM
        rval=SendATCmd("AT+CSTT=\"iot-eu.aer.net\",\"\",\"\"",AT_HTTP_BEARER_APN_SETTINGS, 10, 5);    //AT&T Apn
#endif
#ifdef SIERRA_SIM
        rval = SendATCmd("AT+CSTT=\"internet.sierrawireless.com\",\"\",\"\"",AT_TCP_SET_APN, 10,5);
#endif
            break;

    case 17:

        rval=SendATCmd("AT&W",AT_SAVE,2,5);
        break;

    default:
        AT_cmd_num = 0;
        rval = Function_Success;

    }

    Check_Return_Value(&AT_cmd_num, &rval);

    return rval;
}


unsigned int Check_network()
{
    unsigned int rval=0xff;

    switch(CheckNet_State)
    {

    case 1:
        rval=SendATCmd("AT+CREG?",AT_CHECK_NETWORK,2,5);
        break;

    case 2:

        rval=SendATCmd("AT+CGATT?",AT_CHECK_GPRS_ENABLE,10,10);
        if(rval == GPRS_DEATTACHED)
        {
            __delay_cycles(16000000);
            rval=SendATCmd("AT+CGATT=1",AT_ENABLE_GPRS,10,10);
            if(Failed == rval)
            {
                CheckNet_State = 1;
                ATMaxtry = 0;
                wait_for_second(5);
            }

        }
        break;
    case 3:
        rval = SendATCmd("AT+CSQ",AT_SIGNAL_STRENGTH, 10,5);
        break;


    default:
        rval = Function_Success;
        CheckNet_State = 0;

    }


    Check_Return_Value(&CheckNet_State, &rval);


    return rval;
}



void Check_Return_Value(unsigned char *state_variable,unsigned int *rval)
{
    switch(*rval)
    {


    case HTTP_CONNECTION_ESTABLISHED:(*state_variable)++;
    case HTTP_CONNECTION_NO_ESTABLISHED: *rval = Pass_Success;
    case Init_tcp_Success:
    case init_http_success:
    case Function_Success:
    case GPRS_ATTACHED:
    case Success: ATMaxtry = 0; (*state_variable)++;break;
     case ATMaxtriedOccured:    PMM_trigBOR(); break;

    case NET_SEARCHING: (*state_variable) = 1; ATMaxtry= 0;break;
    case Failed:
    default:break;

    }

}


unsigned int Init_Http_TCP()
{
    unsigned int rval = 0xff;

    switch(Init_Http_Tcp_State)
    {

    case 1:
        rval = ShutAllConnection();
//        rval = SendATCmd("AT+CGDCONT=1,\"IP\",\"ibot.com.attz\"", AT_CGDCONT, 10, 5);
        rval = Success;
        break;
    case 2:
//        rval = SendATCmd("AT+CSOCKSETPN", AT_CSOCKSETPN, 10, 5);
        rval = Success;
        break;
    case 3:
        rval = Init_Tcp();
        if(rval == Success )
            rval = Pass_Success;
        break;
    case 4:
       rval = Init_HTTP();
       if(rval == Success )
           rval = Pass_Success;
       break;
//    case 6:rval = AGPS();break;

    default:
        rval = Function_Success;
        Init_Http_Tcp_State = 0;
    }

    Check_Return_Value(&Init_Http_Tcp_State, &rval);

    return rval;
}

unsigned int Init_HTTP()
{
    unsigned int rval =0xff;
    switch(Init_Http_State)
    {
    case 1:
        rval=SendATCmd("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",AT_HTTP_SET_BEARER_SETTINGS, 10,5);
        break;
    case 2:
        //  rval=SendATCmd("AT+SAPBR=3,1,\"APN\",\"unlimitiot\"",AT_HTTP_BEARER_APN_SETTINGS, 10, 5);
#ifdef ATT_SIM
        rval=SendATCmd("AT+SAPBR=3,1,\"APN\",\"ibot.com.attz\"",AT_HTTP_BEARER_APN_SETTINGS, 10, 5);    //AT&T Apn
#endif
#ifdef  AERIS_SIM
        rval=SendATCmd("AT+SAPBR=3,1,\"APN\",\"iot-eu.aer.net\"",AT_HTTP_BEARER_APN_SETTINGS, 10, 5);    //AT&T Apn
#endif
#ifdef SIERRA_SIM
        rval=SendATCmd("AT+SAPBR=3,1,\"APN\",\"internet.sierrawireless.com\"",AT_HTTP_BEARER_APN_SETTINGS, 10, 5); //sierra Apn
#endif
        break;

    case 3:
        rval=SendATCmd("AT+SAPBR=2,1",AT_HTTP_BRINGUP_GPRS, 10, 5);

        break;
    case 4:
        rval=SendATCmd("AT+SAPBR=1,1",AT_HTTP_GET_LOCAL_IP, 10, 5);
        break;
    default:
        rval = init_http_success;
        ATMaxtry = 0;
        Init_Http_State = 0;

    }

    Check_Return_Value(&Init_Http_State, &rval);

    return rval;
}


/***************************************************************************************************************
 *
 *TCP section
 *
 *************************************************************************************************************/
#ifdef TCP_Enable
/*******************
 *
 * TCP_ALREADY_CONNECTED - directly step to open the socket
 * TCP_INITIAL           -  setting internet and then open socket
 * TCP_CLOSED            -  setting internet and then open socket
 * TCP_CONNECTION_PROGRESS -  wait for 2 sec and again try for 5 five time then restart
 * FAILED                  - try for 5 five time then restart.
 *
 */
unsigned int TCPConnection_Status(char socket_id)
{
    unsigned int rval=0xff;

    socket_id += 48;
    memset(reqSock, 0x00, sizeof(reqSock));
    strcpy(reqSock,"C: ");
    strcat(reqSock,&socket_id);

    rval=SendATCmd("AT+CIPSTATUS", AT_TCP_STATUS, 30, 5);

    return rval;
}



/**********************************************
 * Init_tcp_Success
 *
 * FAILED
 */
unsigned int Init_Tcp()
{
    unsigned int rval=0xff;

    switch(Init_Tcp_state)
    {
    case 1:
        rval = SendATCmd("AT+CIPSHUT",AT_TCP_SHUT, 10,5);
        break;

    case 2:
        rval = SendATCmd("AT+CIPMUX=1",AT_TCP_ENABLE_MULTIPLE, 10,5);

        break;

     case 3:
         //        rval = SendATCmd("AT+CSTT=\"unlimitiot\",\"\",\"\"",AT_TCP_SET_APN, 10,5);
 #ifdef ATT_SIM
         rval = SendATCmd("AT+CSTT=\"ibot.com.attz\",\"\",\"\"",AT_TCP_SET_APN, 10,5);
 #endif
 #ifdef  AERIS_SIM
         rval=SendATCmd("AT+CSTT=\"iot-eu.aer.net\",\"\",\"\"",AT_HTTP_BEARER_APN_SETTINGS, 10, 5);    //AT&T Apn
 #endif
 #ifdef SIERRA_SIM
         rval = SendATCmd("AT+CSTT=\"internet.sierrawireless.com\",\"\",\"\"",AT_TCP_SET_APN, 10,5);
 #endif
             break;

    case 4:
        rval = SendATCmd("AT+CIICR",AT_TCP_BRINGUP_GPRS, 85,5);
        break;

    case 5:
        rval = SendATCmd("AT+CIFSR",AT_TCP_GET_LOCAL_IP, 10,5);
        break;

    default:
        rval = Init_tcp_Success;
        Init_Tcp_state = 1;
        break;

    }

    Check_Return_Value(&Init_Tcp_state, &rval);


    return rval;
}

unsigned int ShutAllConnection()
{

   unsigned int rval = 0xff;
    rval = SendATCmd("AT+CIPSHUT", AT_TCP_SHUT, 5, 3);
    ATMaxtry = 0;
    return rval;
}



unsigned int SATRT_TCP_UDP_SSL_CONNECTION(char Socket_id,int Connection_Mode,const char *Url,const char *Port)
{

    unsigned int rval = 0xff;

    char sockt = 0xff;
    unsigned char time = 20;

    memset(reqSock, 0x00, sizeof(reqSock));
    memset(Common_Buffer_G, 0x00,sizeof(Common_Buffer_G));
    strcpy(Common_Buffer_G, "AT+CIPSTART=");
    sockt = Socket_id+48;
    memset(reqSock, 0x00, sizeof(reqSock));
    strncpy(reqSock, &sockt,1);
    strcat(reqSock, ", CONNECT OK");

     strncat(Common_Buffer_G,&sockt,1);
     strcat(Common_Buffer_G,",\"");

     switch(Connection_Mode)
     {
     case TCP_MODE:    strcat(Common_Buffer_G, "TCP\",\"");break;
     case UDP_MODE:    strcat(Common_Buffer_G, "UDP\",\"");break;

     }
     strcat(Common_Buffer_G,Url);
     strcat(Common_Buffer_G, "\",");
     strcat(Common_Buffer_G, Port);


     rval=SendATCmd(Common_Buffer_G, AT_TCP_START_CONNECTION, 30, 1);


     while(time)
     {

         switch(rval)
         {

         case TCP_CONNECTED: ATMaxtry = 0; Stop_GSM_MaxTimer();  return rval = StartTCPSuccess;
         case TCP_NOT_CONNNECTED:
         case Failed: time = 0; break;
         case Success:
         default:
             TA0CCR0 = 50;
             ATMaxtry = 0;
             ATCmd = true;
             GSM_Buffer_Ready=false;
             GSMCmdtimout =  false;
             time-=2;
             if(time <= 0)
             {
                return rval = Failed;

             }
             Start_GSM_MaxTimer(--time);

             while(!(GSM_Buffer_Ready || GSMCmdtimout));

             rval = ProcessGSMResp(AT_TCP_START_CONNECTION_PLUS);

             if(GSM_Buffer_Ready)
             {
                 GSM_Buffer_Ready = false;


             }
             else
             {
                 return  rval = Pass_Success;

             }

         }

     }

    ATCmd=false;
    TA0CCR0 = 1024;
    return rval;
}

/***************************************************
 * FAILED                    - retry 5 times then restart
 * TCP_SENDTIMEOUT_OCCURED   - retry 5 times then restart
 * TCP_SEND_SUCCESS          - successful
 * TCP_SEND_FAILED           -  restart connection
 * TCP_CONNECTION_CLOSED     - restart connection
 */
unsigned int TCP_SEND(char socket,char *data,unsigned int len)
{
    unsigned int rval=0xff;
    char len_p[5]={0};
    char time = 30;

    ltoa(len, len_p);
    GSM_Uart_Str("AT+CIPSEND=");
    GSM_Uart_char(socket+48);
    GSM_Uart_char(',');

    rval=SendATCmd((char *)len_p, AT_TCP_SEND_DATA, 10, 1);
    if(rval == Success)
    {
        TA0CCR0 =1200;
        GSMCmdtimout=false;
        ATCmd=true;
        GSM_Buffer_Ready=false;
        Start_GSM_MaxTimer(time);

        while(len)
        {

            GSM_Uart_char(*data++);
            len--;
        }

        while(!(GSM_Buffer_Ready || GSMCmdtimout));

        if(GSMCmdtimout == true )
        {
            GSMCmdtimout=false;
            rval = TCP_SENDTIMEOUT_OCCURED;
            ATMaxtry = 0;

        }
        else
        {
            do
            {


                rval=ProcessGSMResp(AT_TCP_SEND_DATA_PLUS);
                if((rval == TCP_SEND_SUCCESS )||(rval == TCP_SEND_FAILED) || (rval == TCP_CONNECTION_CLOSED))
                {
                    ATMaxtry = 0;
                    break;

                }
                else
                    if(rval == Failed)
                    {
                        time -= 2;

                        if(time == 0 )
                            break;

                    }

            }while(WaitExtraResp(time));

            if(rval == 0)
            {

                rval = Failed;
                ATMaxtry = 0;
            }


        }
    }
    else
    {

        ATMaxtry = 0;
    }
    TA0CCR0 = 1024;
    ATCmd=false;
    return rval;

}

unsigned char WaitExtraResp(unsigned char time)
{
    unsigned char rval= 0xff;
    GSM_Buffer_Ready=false;
    Start_GSM_MaxTimer(time);
    while(!(GSM_Buffer_Ready || GSMCmdtimout));

    if(GSMCmdtimout == true)
    {
        GSMCmdtimout = false;
        rval = 0;

    }
    else
    {
        GSM_Buffer_Ready = false;
        rval = Success;
    }

    return rval;
}




#endif


unsigned int SendATCmd(char *cmd,unsigned char cmd_num,unsigned char timeout,unsigned char retry)
{

    unsigned int rval=0xff;
    Start_Buffering=true;
    GSMCmdtimout=false;
    ATCmd=true;
    TA0CCR0 = 50;


    if(ATMaxtry++ < retry )
    {
        clear_buffer();
        GSM_Buffer_Ready=false;
        Tx_Uart1_String(cmd);
        Tx_Uart1_String("\r\n");

        Start_GSM_MaxTimer(timeout);

        while(!(GSM_Buffer_Ready || GSMCmdtimout));
        ATCmd=false;
        if(GSMCmdtimout == true )
        {
            GSMCmdtimout=false;
            rval= Failed;

        }
        else
        {
            rval=ProcessGSMResp(cmd_num);

        }
    }
    else
    {
        rval = ATMaxtriedOccured;
    }

    TA0CCR0 = 1024;

    return rval;
}



unsigned int ProcessGSMResp(unsigned char cmd)
{
    unsigned  int rval=0xff;
    char *ptr = NULL;
    char *Temp_Buffer = NULL;


    switch(cmd)
    {

    case AT_TCP_CLOSE_CONNECTION:
    case AT_TCP_ENABLE_MULTIPLE:
    case AT_TCP_SET_APN:
    case AT_TCP_BRINGUP_GPRS:
    case AT_CGNSPWR:
    case AT_CGNSSEQ:
    case AT_ATTENTION_COMMAND:
    case AT_SAVE:
    case AT_AUTO_BAUD_ENABLE:
    case AT_TURN_OFF_ECHO:
    case AT_REJECT_INCOMING_CALLS:
    case AT_ENABLE_TIMESTAMP_AUTO_UPDATE:
    case AT_HTTP_SET_BEARER_SETTINGS:
    case AT_HTTP_BEARER_APN_SETTINGS:
    case AT_RESTORE_IQ:
    case AT_HTTP_GET_LOCAL_IP:

        if(strnstr((char *)GSM_Buffer,"OK",GSM_Buffer_Index))
            rval=Success;
        else
            rval= Failed;
        break;

    case AT_CHECK_AUTO_TIMESTAMP_ENABLE:
         if(strnstr((char *)GSM_Buffer,"+CLTS: 1",GSM_Buffer_Index))
             rval=Success;
         else
             if(strnstr((char *)GSM_Buffer,"+CLTS: 0",GSM_Buffer_Index))
                 rval = TIMESTAMP_DISABLE;
             else
                 rval= Failed;
         break;

     case AT_CLOCK:
     {
         if(ptr=strnstr((char *)GSM_Buffer,"+CCLK:",GSM_Buffer_Index))
         {

             RTCCTL1|=RTCHOLD+RTCBCD;
             char timtemp =  AsciiToBcd((unsigned char*)(ptr+8),2);


             if(timtemp < 15)
             {

                        Debug_Uart_Str("\nInvalid time,Battery Down.Clock initialised with Production Time\n");

                        Init_RTC(0, 0, 0, 0, 0, 0);

             }
             else
             {

                 Init_RTC(AsciiToBcd((unsigned char*)(ptr+23),2), AsciiToBcd((unsigned char*)(ptr+20),2), AsciiToBcd((unsigned char*)(ptr+17),2), AsciiToBcd((unsigned char*)(ptr+14),2),AsciiToBcd((unsigned char*)(ptr+11),2), AsciiToBcd((unsigned char*)(ptr+8),2));


             }


             rval=Success;
         }
         else
             rval= Failed;
     }break;
     case AT_REQUEST_IMEI:
     {
         if(strlen(GSM_Buffer) > 15)
         {
             if((GSM_Buffer[2] >=48 )&& (GSM_Buffer[2] < 58 ))
             {
 #ifdef HardCodeIMEI
                 SPI_CheckAndWriteIntoFlash(IMEI, 15, IMEI_SA, IMEI_EA, true);
 #else
                 SPI_CheckAndWriteIntoFlash((char *)(GSM_Buffer+2), 15, IMEI_SA, IMEI_EA, true);
 #endif
                 rval=Success;
             }
             else
                 rval=Failed;
         }
         else
             rval=Failed;
     }
     break;

     case AT_SIM_NUMBER:
     {
         if(strlen(GSM_Buffer) > 15)
         {
             if((GSM_Buffer[2] >=48 ) && (GSM_Buffer[2] < 58 ))
             {
                 SPI_CheckAndWriteIntoFlash((char *)(GSM_Buffer+2), 20, CCID_SA, CCID_EA, true);
                 // strncpy(CCID_Num,(char *)GSM_Buffer[2],20);
                 rval=Success;
             }
             else
                 rval=Failed;
         }
         else
             rval=Failed;

     }
     break;

     case AT_SIM_PRESENCE_CMD:
     {
         if(strnstr((char *)GSM_Buffer,"+CSMINS: 0,0",GSM_Buffer_Index))
             rval=SIM_CARD_ABESENT;
         else
             rval= Success;

     }
     break;

     case AT_REQUEST_IMSI:
     {
         if(strlen(GSM_Buffer) > 15)
         {
             if((GSM_Buffer[2] >=48 )&& (GSM_Buffer[2] < 58 ))
             {

                 SPI_CheckAndWriteIntoFlash((char *)(GSM_Buffer+2), 15, IMSI_SA, IMSI_EA, true);
                 rval=Success;
             }
             else
                 rval=Failed;
         }
         else
             rval=Failed;
     }
     break;


     case AT_CHECK_NETWORK:
     {
         if(strnstr((char *)GSM_Buffer, "+CREG: 0,5", GSM_Buffer_Index) || strnstr((char *)GSM_Buffer, "+CREG: 0,1", GSM_Buffer_Index))
         {
             rval=Success;
         }
         else
         {
             if(strnstr((char *)GSM_Buffer, "ERROR", GSM_Buffer_Index))
                 rval=Failed;
             else
                 rval=NET_SEARCHING;
         }
     }
         break;

     case AT_SIGNAL_STRENGTH:
       {
           char i=0;
           ptr=0;


           if(ptr=strnstr((char *)GSM_Buffer, "+CSQ:", GSM_Buffer_Index))
           {
               Temp_Buffer = calloc(1, 50);
               memset(Temp_Buffer,0,sizeof(Temp_Buffer));
               while(ptr[6+i] != ',')
               {
                   Temp_Buffer[i]=ptr[6+i];
                   i++;
               }
               SPI_CheckAndWriteIntoFlash((char *)Temp_Buffer, strlen(Temp_Buffer), Signal_Strength_SA, Signal_Strength_EA, true);



               free(Temp_Buffer);
               rval=Success;
           }
           else
               rval=Failed;

       }
       break;

       case AT_CHECK_GPRS_ENABLE:
       {
           if(strnstr((char *)GSM_Buffer,"+CGATT: 1",GSM_Buffer_Index))
               rval=GPRS_ATTACHED;
           else
               if(strnstr((char *)GSM_Buffer,"+CGATT: 0",GSM_Buffer_Index))
                   rval= GPRS_DEATTACHED;
               else
                   rval=Failed;
       }
           break;

       case AT_ENABLE_GPRS:
       {
           if(strnstr((char *)GSM_Buffer,"OK",GSM_Buffer_Index))
               rval=Success;
           else
               rval= Failed;
       }
           break;


       case AT_TCP_STATUS:
       {
           if(strnstr((char *)GSM_Buffer,"OK",GSM_Buffer_Index))
           {
               if(ptr=strnstr((char *)GSM_Buffer,reqSock,GSM_Buffer_Index))
               {
                   ptr=strtok(ptr,"\r");
                   strncpy(Temp_Buffer,ptr,strlen(ptr));

                   if(strnstr(Temp_Buffer,"CONNECTED",strlen(Temp_Buffer)))
                   {
                       rval = TCP_ALREADY_CONNECTED;
                   }
                   else
                       if(strstr(Temp_Buffer,"CLOSED"))
                       {
                           rval = TCP_CLOSED;
                       }
                       else
                           if(strnstr(Temp_Buffer,"CONNECTING",strlen(Temp_Buffer)) || strnstr(Temp_Buffer,"CLOSING",strlen(Temp_Buffer)))
                           {
                               rval = TCP_CONNECTION_PROGRESSING;
                           }
                           else
                               if(strnstr((char *)GSM_Buffer,"INITIAL",GSM_Buffer_Index) || strnstr((char *)GSM_Buffer,"START",GSM_Buffer_Index))
                               {
                                   rval = TCP_INITIAL;
                               }


               }
               else
               {
                   if(strnstr((char *)GSM_Buffer,"INITIAL",GSM_Buffer_Index) || strnstr((char *)GSM_Buffer,"START",GSM_Buffer_Index))
                   {
                       rval = TCP_INITIAL;
                   }
               }

           }
           else
               rval= Failed;

       }break;

       case AT_TCP_SHUT:
         {
             if(strnstr((char *)GSM_Buffer,"SHUT OK",GSM_Buffer_Index))
                 rval=Success;
             else
                 rval= Failed;
         }
         break;

       case AT_TCP_GET_LOCAL_IP:
         {
             if(strnstr((char *)GSM_Buffer,"ERROR",GSM_Buffer_Index))
                 rval=Failed;
             else
                 rval= Success;
         }

         break;

       case AT_HTTP_BRINGUP_GPRS:
       {
           if(strnstr((char *)GSM_Buffer,"+SAPBR: 1,3",GSM_Buffer_Index))
           {
               rval=HTTP_CONNECTION_NO_ESTABLISHED;
           }
           else
               if(strnstr((char *)GSM_Buffer,"+SAPBR: 1,1",GSM_Buffer_Index))
                   rval=HTTP_CONNECTION_ESTABLISHED;
               else
                   rval= Failed;

       }break;

       case AT_TCP_START_CONNECTION:
       {
           if(strnstr((char *)GSM_Buffer,reqSock,GSM_Buffer_Index))
           {
               rval = TCP_CONNECTED;
           }else
               if(strnstr((char *)GSM_Buffer,"ALREADY CONNECT",GSM_Buffer_Index))
               {
                   wait_for_second(2);
                   rval = TCP_CONNECTED;
               }
               else
                   if(strnstr((char *)GSM_Buffer,"OK",GSM_Buffer_Index))
                       rval=Success;
                   else
                       rval= Failed;
       }
       break;

       case AT_TCP_START_CONNECTION_PLUS:
       {
           if(strnstr((char *)GSM_Buffer,reqSock,GSM_Buffer_Index))
               rval=TCP_CONNECTED;
           else
               if(strnstr((char *)GSM_Buffer,"ERROR",GSM_Buffer_Index))
                   rval=Failed;
               else
                   rval= TCP_NOT_CONNNECTED;

       }break;


       case AT_TCP_SEND_DATA:
         {
             if(strnstr((char *)GSM_Buffer,">",GSM_Buffer_Index))
                 rval=Success;
             else
                 rval= Failed;
         }break;

         case AT_TCP_SEND_DATA_PLUS:
         {
             if(strnstr((char *)GSM_Buffer,"SEND OK",GSM_Buffer_Index))
                 rval=TCP_SEND_SUCCESS;
             else
                 if(strnstr((char *)GSM_Buffer,"SEND FAIL",GSM_Buffer_Index))
                     rval=TCP_SEND_FAILED;
                 else
                     if((strnstr((char *)GSM_Buffer,"0, CLOSED",GSM_Buffer_Index)) ||(strnstr((char *)GSM_Buffer,"+PDP: DEACT",GSM_Buffer_Index)))
                           rval = TCP_CONNECTION_CLOSED;
                 else
                     rval= Failed;
         }break;



//
//    case AT_HTTP_DATA:
//    {
//        if(strnstr((char *)GSM_Buffer,"DOWNLOAD",GSM_Buffer_Index))
//            rval=Success;
//        else
//            rval= Failed;
//    }break;
//
//    case AT_HTTP_BRINGUP_GPRS:
//    {
//        if(strnstr((char *)GSM_Buffer,"+SAPBR: 1,3",GSM_Buffer_Index))
//        {
//            rval=Connection_Not_Established;
//        }
//        else
//            if(strnstr((char *)GSM_Buffer,"+SAPBR: 1,1",GSM_Buffer_Index))
//                rval=Connection_Established;
//            else
//                rval= Failed;
//
//    }break;
//
//    case AT_HTTP_ACTION_GET_POST:
//    {
//
//        if(ptr=strnstr((char *)GSM_Buffer,"+HTTPACTION:",GSM_Buffer_Index))
//        {
//            ptr=strtok((ptr+15),",");
//            switch(atoi(ptr))
//            {
//            case 200: rval=Success;    break;
//            case 300:  rval= Failed;    break;
//            case 400:
//            case 404:
//            case 603:
//            case 601:
//            default: rval= Network_error;  break;
//            }
//            ptr=0;
//        }
//        else
//            rval= Failed;
//    }break;
//
//    case AT_HTTP_READ:
//    {
//        ptr=0;
//        memset(Resp_string, 0x00,sizeof(Resp_string));
//        if(ptr=strnstr((char *)GSM_Buffer,"+HTTPREAD:",GSM_Buffer_Index))
//        {
//            ptr=strtok((ptr+11),"\r");
//            ptr=strtok(NULL,"\r");
//
//            strcpy(Resp_string,ptr+1);
//            rval=Success;
//        }
//        else
//            rval=Failed;
//
//
//
//    }break;
//
//
//
//
//    case AT_TCP_START_CONNECTION:
//    {
//        if(strnstr((char *)GSM_Buffer,reqSock,GSM_Buffer_Index))
//        {
//            rval = TCP_CONNECTED;
//        }else
//            if(strnstr((char *)GSM_Buffer,"ALREADY CONNECT",GSM_Buffer_Index))
//            {
//                rval = TCP_CONNECTED;
//            }
//            else
//                if(strnstr((char *)GSM_Buffer,"OK",GSM_Buffer_Index))
//                    rval=Success;
//                else
//                    rval= Failed;
//    }
//    break;
//
//    case AT_TCP_START_CONNECTION_PLUS:
//    {
//        if(strnstr((char *)GSM_Buffer,reqSock,GSM_Buffer_Index))
//            rval=TCP_CONNECTED;
//        else
//            if(strnstr((char *)GSM_Buffer,"ERROR",GSM_Buffer_Index))
//                rval=Failed;
//            else
//                rval= TCP_NOT_CONNNECTED;
//
//    }break;
//
//
//
//    case AT_TCP_STATUS:
//    {
//        if(strnstr((char *)GSM_Buffer,"OK",GSM_Buffer_Index))
//        {
//            if(ptr=strnstr((char *)GSM_Buffer,"C: 0,0",GSM_Buffer_Index))
//            {
//                ptr=strtok(ptr,"\r");
//                strncpy(Temp_Buffer,ptr,strlen(ptr));
//                if(strnstr(Temp_Buffer,"CONNECTED",strlen(Temp_Buffer)))
//                {
//                    rval = TCP_ALREADY_CONNECTED;
//                }
//                else
//                    if(strstr(Temp_Buffer,"CLOSED"))
//                    {
//                        rval = TCP_CLOSED;
//                    }
//
//            }
//            else
//            {
//                if(strnstr((char *)GSM_Buffer,"INITIAL",GSM_Buffer_Index))
//                {
//                    rval = TCP_INITIAL;
//                }
//            }
//
//        }
//        else
//            rval= Failed;
//
//    }break;
//
//
//
//
//    case AT_TCP_SEND_DATA:
//    {
//        if(strnstr((char *)GSM_Buffer,">",GSM_Buffer_Index))
//            rval=Success;
//        else
//            rval= Failed;
//    }break;
//
//    case AT_TCP_SEND_DATA_PLUS:
//    {
//        if(strnstr((char *)GSM_Buffer,"SEND OK",GSM_Buffer_Index))
//            rval=Success;
//        else
//            if(strnstr((char *)GSM_Buffer,"SEND FAIL",GSM_Buffer_Index))
//                rval=TCP_Send_Failed;
//            else
//                if((strnstr((char *)GSM_Buffer,"0, CLOSED",GSM_Buffer_Index)) ||(strnstr((char *)GSM_Buffer,"+PDP: DEACT",GSM_Buffer_Index)))
//                      rval = TCP_CONNECTION_CLOSED;
//            else
//                rval= Failed;
//    }break;
//
//    case AT_POWER_OFF:
//    {
//        if(strnstr((char *)GSM_Buffer,"NORMAL POWER DOWN",GSM_Buffer_Index))
//            rval=Success;
//        else
//            rval= Failed;
//    }
//    break;
//
//
//
//
//
//    case AT_CGNSINF:
//    {
//        ptr= 0;
//        //  strcpy(GSM_Buffer,"+CGNSINF: 1,1,20180227114200.000,17.456043,78.375783,566.358,0.00,11.2,2,,1.4,1.9,0.9,,13,9,,,44,,<CR><LF>");
//        if(ptr=strnstr((char *)GSM_Buffer,"+CGNSINF: 1",GSM_Buffer_Index))
//        {
//            if(ptr[12] == '1')
//            {
//                rval=Success;
//            }
//            else
//                rval = Failed;
//        }
//        else
//            rval = Failed;
//
//
//    }break;
//
//    case AT_GET_GSM_LOCATION:
//    {
//        /*
//         *  0:Success
//         *  404: Not found
//         *  408: timeout
//         *  601: NW Error
//         *  602: No Memory
//         *  603: DNS Error
//         *  604: Stack Busy
//         *  65535: Other Error.
//         *  Please refer to documentation for details.
//         */
//        ptr=0;
//        char k=0;
//
//        memset(Temp_Buffer,0,sizeof(Temp_Buffer));
//
//        if(ptr=strstr((char *)GSM_Buffer, "+CIPGSMLOC: 0"))
//        {
//            ptr=strtok((char *)ptr,",");
//
//            if(ptr)
//            {
//                for(k=0;k < 2;k++)
//                {
//                    ptr=strtok(NULL,",");
//                    if(ptr == NULL)
//                        break;
//                    strncat(Temp_Buffer,ptr,strlen(ptr));
//                    if(k == 0) strcat(Temp_Buffer,",");
//                    ptr=0;
//                }
//
//                SPI_CheckAndWriteIntoFlash((char *)Temp_Buffer, strlen(Temp_Buffer), Location_SA, Location_EA, true);
//                rval=Success;
//            }
//            else
//                rval=Failed;
//        }
//        else if(strnstr((char *)GSM_Buffer, "+CIPGSMLOC:", GSM_Buffer_Index))
//            rval=GSM_LOCATION_ERROR;
//        else
//            rval=Failed;
//    }
//    break;
//
//    case AT_GET_MOBILE_OPERATOR_NAME:
//    {
//        ptr=0;
//        if(strnstr((char *)GSM_Buffer, "+COPS:", GSM_Buffer_Index))
//        {
//            rval=Success;
//            ptr=strtok(GSM_Buffer,"\"");
//            ptr=strtok(NULL,"\"");
//
//            if(ptr)
//            {
//                //                memset(Temp_Buffer,0,sizeof(Temp_Buffer));
//                //                strncpy(Temp_Buffer,ptr,(strlen(ptr) > 20)?20:strlen(ptr));
//                SPI_CheckAndWriteIntoFlash((char *)ptr, strlen(ptr), SIM_Operator_SA, SIM_Operator_EA, true);
//
//            }
//
//        }
//        else
//            rval=Failed;
//    }
//    break;
//
//    case AT_GET_BATTERY_PARAMETERS:
//    {
//        ptr=0;
//
//        if(strnstr((char *)GSM_Buffer, "+CBC: ", GSM_Buffer_Index))
//        {
//            memset(Temp_Buffer, 0x00, sizeof(Temp_Buffer));
//            ptr=strtok(GSM_Buffer,",");
//            ptr=strtok(NULL,",");
//            strcpy(Temp_Buffer, ptr);
//            strcat(Temp_Buffer, "%,");
//            ptr=strtok(NULL,"\r");
//            strcat(Temp_Buffer,ptr);
//            SPI_CheckAndWriteIntoFlash((char *)Temp_Buffer, strlen(Temp_Buffer), Battery_SA, Battery_EA, true);
//            rval=Success;
//
//        }
//        else
//            rval=Failed;
//
//
//    }  break;
//

//
//
    default:
        break;
    }

    if(!Processing_unsolic_msg)
        Proccess_Unsolicated_Response();


    return rval;
}



void clear_buffer()
{
    memset(GSM_Buffer,0x00,GSM_Buffer_Size);
    GSM_Buffer_Index=0;
}



unsigned char TurnOn_GSM()
{
    char timeout =4;
    if(P3IN & BIT5)
    {
#ifdef Debug_Enable
        ////////DebugUart("GSM Modem is already On..Turnnig Off the Moderm");
#endif
        //        TurnOff_Gsm();
        //        wait_for_second(5);
        return 1;
    }
    else
    {
#ifdef Debug_Enable
        ////////DebugUart("GSM Modem is OFF now..Turnnig On the Moderm");
#endif
//        P4DIR |= BIT6;            // Set P4.6 to output direction
        P4OUT |= BIT6;            // Configure PORT 4 PIN 6 as output for signalling GSM PWRKEY activation.
        wait_for_second(1);  // Generate 1 second Delay
        P4OUT &= ~BIT6;           // else reset
        wait_for_second(1);   // Generate 1 second Delay
        P4OUT |= BIT6;

        while(!(P3IN & BIT5) && timeout--)            //if P3.5 is set, GSM is ON
        {
            wait_for_second(1);

        }
        if(timeout)
        {
#ifdef Debug_Enable
            ////////DebugUart("GSM Modem is ON now..Wait for Modem Goes Stable");
#endif
            Start_GSM_MaxTimer(2);
            while(GSMmaxTimerOn);
            return 1;
        }
        else
            return 0;
    }

}

unsigned char TurnOff_Gsm()
{

    char timeout =10;
//    P4DIR |= BIT6;            // Set P4.6 to output direction

    P4OUT |= BIT6;            // Configure PORT 4 PIN 6 as output for signalling GSM PWRKEY activation.
    wait_for_second(2);   // Generate 1 second Delay
    P4OUT &= ~BIT6;           // else reset
    wait_for_second(2);
    while((P3IN & BIT5) && --timeout)
    {
        wait_for_second(1);
    }

    if(timeout == 0)            //if P3.5 is set, GSM is ON
        return 0;
    else
        return 1;

}

char *strnstr( char *src, char *dest,unsigned int n)
{
    char *s=src;
    char *d=dest;
    char cp,*temp,matched=0;

    n+=1;
    while(n)
    {
        temp=s;
        d=dest;
        do
        {
            if((*s - (cp =*d)))
            {
                break;
            }
            else
            {
                matched=1;
                s++;d++;
                n--;
            }

        }while (cp && n);

        if(cp == 0 )
            return temp;

        if(matched == 0)
        {
            s++; n--;
        }
        else
            matched=0;

    }

    return 0;
}

