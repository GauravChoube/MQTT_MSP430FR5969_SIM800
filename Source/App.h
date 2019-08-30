/****************************************************************************
 *  File Name  : GSM.h
 *  Created on : Oct 20, 2018
 *  Author     : Gaurav Choubey
 ****************************************************************************/
#ifndef SOURCE_APP_H_
#define SOURCE_APP_H_



/*****************************************************************************
 * Varibale
 *****************************************************************************/
extern unsigned long CloudSendAddress;
extern unsigned long FlashWriteAdrress;
bool Sync_Occured;

/*****************************************************************************
 * macros => 0x7000-0x7FFF
 *****************************************************************************/
#define ConnectCallBuffer_Size  512
extern char ConnectCallBuffer[ConnectCallBuffer_Size];
/*****************************************************************************
 *  function
 *****************************************************************************/
void Init_Device();
unsigned int Connect_To_Broker();
unsigned int Send_Data_to_Cloud();


#endif /* SOURCE_APP_H_ */
