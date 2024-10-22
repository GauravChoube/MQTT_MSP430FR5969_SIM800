/********************************************************************************
  *  File Name  : App.c
 *  Created on : Nov 20, 2018
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
#pragma PERSISTENT(FC)
bool FC = true;

#pragma PERSISTENT(ConnectCallBuffer)
char ConnectCallBuffer[ConnectCallBuffer_Size] = {0};

#pragma PERSISTENT(APP_Com_Buf)
char APP_Com_Buf[100] = {0};


char temp = 0;
char Connect_To_Broker_State = 1;
char Send_Data_to_Cloud_State = 1;


char failed_retry = 0;
unsigned int Packet_len = 0;
long i = 0;
/********************************************************************************
 *    Function Defination Start
 */
void Init_Device()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    PMM_unlockLPM5();

    __bis_SR_register(GIE);


    Init_Clock();

    Init_MPU();

    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);

    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN5);


    WDT_A_initWatchdogTimer(WDT_A_BASE , WDT_A_CLOCKSOURCE_SMCLK , WDT_A_CLOCKDIVIDER_2G);  //DIVIDER/SMCLK = 2G/8MHz = (2*10^9)/(8*10^6) = 250sec = 4.1min
    WDT_A_start(WDT_A_BASE);

    Init_Uart(UART0_PORT, BAUD_RATE_115200, UART_CLOCK_SMCLK, UART_CLOCK_SOURC_FREQ_16MHz);

    Init_Uart(UART1_PORT, BAUD_RATE_115200, UART_CLOCK_SMCLK, UART_CLOCK_SOURC_FREQ_16MHz);

    Tx_Uart0_String("\nHi,This is Gaurav from Uart_0\n");
    Tx_Uart1_String("\nHi,This is Gaurav from Uart_1\n");


    Init_RTC(0, 0, 0, 0, 0, 0);
    Init_Wait_timer();

    TurnOff_Gsm();

    Init_GSM_RespTimer();

    Init_GSM_MaxTimer();

    SPI_Init(SPI_CLK_SMCKL,16000000, 16000000);

    TurnOn_GSM();



    if(FC)
    {
          SPI_Erase(SFM_Chip_erase, SFM_Chip_erase_1);
          wait_for_second(30);
          FC = 0;
    }
    else
    {
        wait_for_second(10);
    }




  temp = 'G';
  SPI_SingleByteWrite(0x00000, temp);
  temp = 0;
  __delay_cycles(160);
  temp = SPI_SingleByteRead(0x00000);

  if(temp == 'G') Tx_Uart1_String("\nFalsh memory is working fine");
  else Tx_Uart1_String("\nFalsh memory is failed");





}



/************************************************
 * Function_Success
 * TCP_CONNECTION_CLOSED
 * Server_Busy
 * ATMaxtriedOccured
 */
unsigned int Connect_To_Broker()
{
    unsigned int rval =0xff;

    switch(Connect_To_Broker_State)
    {
    case 1:
    {
        memset(APP_Com_Buf, 0x00, sizeof(APP_Com_Buf));
        SPI_MultipleContinuouseRead((unsigned  char *)APP_Com_Buf,IMEI_SA, IMEI_EA-IMEI_SA);
        Connect_To_Broker_State++;
    }

    case 2:
    {
        rval = Mqtt_Connection_Established(APP_Com_Buf, Cloud_url, Cloud_port, Cloud_pass,Cloud_Username);

    }break;

    case 3:
    {
           rval =  Subscribe(Subscribe_Topic, 1);

    }break;

    default:
        rval = Function_Success;
        Connect_To_Broker_State = 0;
    }

    switch(rval)
    {
    case Subscribe_Success:
    case Mqtt_Connection_Established_Success:
    case Function_Success:Connect_To_Broker_State++;failed_retry = 0;break;

    case Server_Busy:
        if(++failed_retry > 3)
        {
            failed_retry = 0;
            Connect_To_Broker_State = 1;
            ShutAllConnection();
        }
        else
        {
            Connect_To_Broker_State = 1;
            rval = Pass_Success;
        }
    case ATMaxtriedOccured:
    case TCP_CONNECTION_CLOSED: failed_retry = 0;    Connect_To_Broker_State = 1; break;
    case Failed: if(++failed_retry > 5)
        {
            failed_retry = 0;    Connect_To_Broker_State = 1;
            rval = ATMaxtriedOccured;
            /*PMM_trigBOR();*/
        }

    }
return rval;
}


unsigned int Send_Data_to_Cloud()
{
    unsigned int rval = 0xff;

    switch(Send_Data_to_Cloud_State)
    {
    case 1:
    {
        memset(ConnectCallBuffer, 0x00, sizeof(ConnectCallBuffer));
        Packet_len = 0;

      /*  Packet_len = SPI_MultipleBBBRead((unsigned  char *)ConnectCallBuffer, CloudSendAddress, sizeof(ConnectCallBuffer));
        memset(APP_Com_Buf, 0x00, 100);
        ltoa(Packet_len, APP_Com_Buf);
        Tx_Uart0_String("\nFlash memory read length:");
        Tx_Uart0_String(APP_Com_Buf);
        Tx_Uart0_String(":");
        Tx_Uart0_String(ConnectCallBuffer);

        */
        Packet_len =  Read_from_flash(ConnectCallBuffer, ConnectCallBuffer_Size);
        if(Packet_len == Flash_Empty)
        {

            Packet_len = 0;
            return Function_Success;
        }
        else
        {
            memset(APP_Com_Buf, 0x00, 100);
            ltoa(Packet_len, APP_Com_Buf);
            Tx_Uart0_String("\nFlash memory read length:");
            Tx_Uart0_String(APP_Com_Buf);
            Tx_Uart0_String(":");
            Tx_Uart0_String(ConnectCallBuffer);


        }


        Send_Data_to_Cloud_State++;

    }

    case 2:
    {
        rval = Publish(Publish_Topic,ConnectCallBuffer,Packet_len,1);

    }break;

    default:
        Increment_CloudSendAddress(Packet_len);
        rval = Function_Success;
        Send_Data_to_Cloud_State = 0;

    }

    switch(rval)
    {
    case Publish_Success:
    case Function_Success:Send_Data_to_Cloud_State++; failed_retry = 0;break;

    case Server_Busy:
        if(++failed_retry > 3)
        {
            failed_retry = 0;
            Send_Data_to_Cloud_State = 1;
            ShutAllConnection();
        }
        else
        {
                        Connect_To_Broker_State = 1;
            rval = Pass_Success;
        }

    case ATMaxtriedOccured:
    case TCP_CONNECTION_CLOSED: failed_retry = 0;    Send_Data_to_Cloud_State = 1; break;
    case Failed:
    if(++failed_retry > 5)
    {


        failed_retry = 0;    Send_Data_to_Cloud_State = 1;
        rval = ATMaxtriedOccured;
        /*PMM_trigBOR();*/

    }


    }
    return rval;

}
