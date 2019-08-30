/********************************************************************************
 *  File Name  : Header.h
 *  Created on : Nov 20, 2018
 *  Author     : Gaurav Choubey
 *
 ********************************************************************************/
#ifndef SOURCE_HEADER_H_
#define SOURCE_HEADER_H_
/***************************************************************************************************
 *
 * User define macros.
 * Set each parameter carefully here otherwise MQTT code will not work
 ***************************************************************************************************/

#define Debug_Uart_Str          Tx_Uart0_String
#define Debug_Uart_Char         Tx_Uart0_Char

#define GSM_Uart_Str            Tx_Uart1_String
#define GSM_Uart_char           Tx_Uart1_Char


//#define Thingsboard
#define HIVEMQ


#ifdef HIVEMQ
#define Cloud_url              "broker.hivemq.com"
#define Cloud_port             "1883"
#define Cloud_Username          NULL
#define Cloud_pass              NULL
#define Publish_Topic          "iBotSend"
#define Subscribe_Topic        "iBotRecv"
#endif

#ifdef Thingsboard
#define Cloud_url              "demo.thingsboard.io"
#define Cloud_port             "1883"
#define Cloud_Username          "gc"
#define Cloud_pass              NULL

#define Publish_Topic         "v1/devices/me/telementry"
#define Subscribe_Topic       "v1/devices/me/attributes"
#endif

#define Project_Name "MQTT_SAMPLE_20Nov18"

/********************************************END HERE************************************************************/


/********************************************************************************
 *     Include file
 ********************************************************************************/
#include <msp430.h>
#include <string.h>
#include <stdlib.h>
#include "driverlib.h"
#include "Uart.h"
#include "Timer.h"
#include "rtc.h"
#include "clock.h"
#include "Mpu.h"
#include "GSM.h"
#include "SPi_flash_Reg.h"
#include "App.h"
#include "Mqtt.h"
#include <MQTTPacket.h>
#include <MQTTFormat.h>
#include <MQTTConnect.h>
#include <MQTTPublish.h>




/********************************************************************************
 *     return value for each file
 ********************************************************************************
 *  This  is Common for all project.
 *
 *    GSM.c   => 0x1000-0x1FFF
 *    Uart.c  => 0x2000-0x2FFF
 *    Timer.c => 0x3000-0x3FFF
 *    Mpu.c   => 0x4000=0x4FFF
 *    RTC.c   => 0x5000-0x5FFF
 *    Clokc.c =>  0x6000-0x6FFF
 *    App.c   => 0x7000-0x7FFF
 *    Spi_flash.c => 0x8000-0x8FFF
 *
 *  This is going to Vary with project.
 *    Mqtt.c  => 0xA000-0xAFFF
 *
 *
 *
*********************************************************************************/
/********************************************************************************
 *     Macros
 ********************************************************************************/
//SPI flash Memory related address
/***********************************************************FLASH STORAGE INFOMATION**************************************************************************/
                                    /* Flash Storing Parameters with their Addresses     =>       SECTOR 1   (0x00000 to 0x0FFFF)
                                     *
                                     * SELF UPGRADE(IQ firmware)                         =>       SECTOR 3,4 (0x20000 to 0x3FFFF)
                                     *
                                     * BaseBoard UPGRADE                                 =>       SECTOR 5,6 (0x40000 to 0x6FFFF)
                                     *
                                     * Application Demo                                  =>       SECTOR 8 to SECTOR 16 (0x70000 to 0xFFFFF)
                                     *
 ***************************************************************************************************************************************************************/

/*****************SELF UPGRADE*********************************************/
//SECTOR 3 : 0x20000 to 0x2FFFF
//SECTOR 4 : 0x30000 to 0x3FFFF

#define LengthOFHEx                 0x20000
#define SELF_UPGRADE_SA             0x2004E    //its should be 0x20064 but with security gateway i have to stored file from '{' rather '@'
#define SELF_UPGRADE_EA             0x3FFFF

/*****************BASEBOARD UPGRADE*********************************************/
//SECTOR 5 : 0x40000 to 0x4FFFF
//SECTOR 6 : 0x50000 to 0x5FFFF
//SECTOR 7 : 0x60000 to 0x6FFFF

#define BASEBOARD_UPGRADE_SA        0x40000
#define BASEBOARD_UPGRADE_EA        0x6FFFF


/************SECTOR 1**********************************************************/
//SECTOR 1 : 0x0000 to 0xFFFF
/****************************************/
//PARAMETER         : Flash Woking Status
//BYTES ALLOCATED   : 3
/****************************************/
#define Flash_Working_Status_SA     0x0000      //Starting Address
#define Flash_Working_Status_EA     0x0002      //End Address

/****************************************/
//PARAMETER         : IQ Firmware Version
//BYTES ALLOCATED   : 50
/****************************************/
#define IQ_Firmware_Version_SA      0x0003      //Starting Address
#define IQ_Firmware_Version_EA      0x0034      //End Address

/****************************************/
//PARAMETER         : GSM Signal Strength
//BYTES ALLOCATED   : 5
/****************************************/
#define Signal_Strength_SA          0x0035      //Starting Address
#define Signal_Strength_EA          0x0039      //End Address

/****************************************/
//PARAMETER         : GPS Location
//BYTES ALLOCATED   : 30
/****************************************/
#define Location_SA                 0x003A      //Starting Address
#define Location_EA                 0x0057      //End Address

/****************************************/
//PARAMETER         : GSM IO Pin Voltage(Battery)
//BYTES ALLOCATED   : 10
/****************************************/
#define Battery_SA                  0x0058      //Starting Address
#define Battery_EA                  0x0061      //End Address

/****************************************/
//PARAMETER         : GSM IMEI
//BYTES ALLOCATED   : 20
/****************************************/
#define IMEI_SA                     0x0061      //Starting Address
#define IMEI_EA                     0x0075      //End Addresss

/****************************************/
//PARAMETER         : GSM IMSI
//BYTES ALLOCATED   : 20
/****************************************/
#define IMSI_SA                     0x0076      //Starting Address
#define IMSI_EA                     0x0089     //End Addresss

/******************************************/
//PARAMETER         : SIM CCID Number
//BYTES ALLOCATED   : 30
/******************************************/
#define CCID_SA                     0x008A
#define CCID_EA                     0x00A8


/****************************************/
//PARAMETER         : SIM Operator Name
//BYTES ALLOCATED   : 30
/****************************************/
#define SIM_Operator_SA             0x00A9      //Starting Address
#define SIM_Operator_EA             0x00C7     //End Addresss


/****************************************/
//PARAMETER         : Owner Name
//BYTES ALLOCATED   : 30
/****************************************/
#define Owner_Name_SA               0x00C8      //Starting Address
#define Owner_Name_EA               0x00E5      //End Addresss

/****************************************/
//PARAMETER         : Application Name
//BYTES ALLOCATED   : 50
/****************************************/
#define APP_Name_SA                 0x00E6       //Starting Address
#define APP_Name_EA                 0x0104      //End Addresss

/****************************************/
//PARAMETER         : NSA
//BYTES ALLOCATED   : 20
/****************************************/
#define NSA_SA                      0x0105      //Starting Address
#define NSA_EA                      0x0118      //End Addresss

/****************************************/
//PARAMETER         : EDA
//BYTES ALLOCATED   : 20
/****************************************/
#define EDA_SA                      0x0119      //Starting Address
#define EDA_EA                      0x012C      //End Addresss

/****************************************/
//PARAMETER         : Version
//BYTES ALLOCATED   : 20
/****************************************/
#define Version_SA                  0x012D      //Starting Address
#define Version_EA                  0x0140      //End Addresss


/****************************************/
//PARAMETER         : Alias Name (Machine Name)
//BYTES ALLOCATED   : 20
/****************************************/
#define Alias_Name_SA               0x0141      //Starting Address
#define Alias_Name_EA               0x0154     //End Addresss



/******************************************/
//PARAMETER         : server url
//BYTES ALLOCATED   : 100
/******************************************/
#define SER_URL_SA                 0x0155
#define SER_URL_EA                 0x01B8


/******************************************/
//PARAMETER         : Devcie Id
//BYTES ALLOCATED   :50
/******************************************/
#define DEV_ID_SA                  0x01B9
#define DEV_ID_EA                  0x01EA


/******************************************/
//PARAMETER         : RecvSecreKey
//BYTES ALLOCATED   :300
/******************************************/
#define RecvSecreKey_SA            0x01EB
#define RecvSecreKey_EA            0x0316


/******************************************/
//PARAMETER         : Topic name 1
//BYTES ALLOCATED   : 50
/******************************************/
#define Publish_Topic_1_SA         0x0317
#define Publish_Topic_1_EA         0x0348


/******************************************/
//PARAMETER         : Organisation id
//BYTES ALLOCATED   : 30
/******************************************/
#define OrgnisationID_Dev_Confg_SA     0x0349
#define OrgnisationID_Dev_Confg_EA     0x0366


/******************************************/
//PARAMETER         : Type of Communciation(Device type)
//BYTES ALLOCATED   : 30
/******************************************/
#define DeviceType_Dev_Confg_SA       0x0367
#define DeviceType_Dev_Confg_EA       0x0384

/******************************************/
//PARAMETER         : Auth_Token_Dev_Confg(mqtt password)
//BYTES ALLOCATED   : 30
/******************************************/
#define Auth_Token_Dev_Confg_SA       0x0385
#define Auth_Token_Dev_Confg_EA       0x03A2


/******************************************/
//PARAMETER         :AuthMethod_Dev_Confg(MQtt username)
//BYTES ALLOCATED   : 30
/******************************************/
#define Auth_Method_Dev_Confg_SA      0x03A3
#define Auth_Method_Dev_Confg_EA      0x03C0


/******************************************/
//PARAMETER         : Topic name 2
//BYTES ALLOCATED   : 50
/******************************************/
#define Publish_Topic_2_SA           0x03C1
#define Publish_Topic_2_EA           0x03F2

/******************************************/
//PARAMETER         : device id from setting
//BYTES ALLOCATED   : 50
/******************************************/
#define Dev_ID_Setting_SA           0x03F3
#define Dev_ID_Setting_EA           0x0424

/******************************************/
//PARAMETER         : subcribertopic
//BYTES ALLOCATED   : 50
/******************************************/
#define Subscribe_Topic_1_SA        0x0425
#define Subscribe_Topic_1_EA        0x0456

/******************************************/
//PARAMETER         : Cyperkey
//BYTES ALLOCATED   : 150
/******************************************/
#define Cyper_key_SA                0x0457
#define Cyper_key_EA                0x04EC


/******************************************/
//PARAMETER         : PPL
//BYTES ALLOCATED   : 30
/******************************************/
#define PPL_SA                      0x04ED
#define PPL_EA                      0x050A

/********************************************************************************************************/
                                    //PARAMETER         : BaseDataBuffer(Decoded Received)
                                    //BYTES ALLOCATED   : 4096
/*******************************************************************************************************/
#define BaseDataBuffer_SA               0xFD001
#define BaseDataBuffer_EA               0xFDFFF

/********************************************************************************************************/
                                    //PARAMETER         : Extened GsmBuffer(encoded Received)
                                    //BYTES ALLOCATED   : 4096
/*******************************************************************************************************/
#define Extended_GSMBuffer_SA           0xFE000
#define Extended_GSMBuffer_EA           0xFEFFF

/********************************************************************************************************/
                                    //PARAMETER         : DecodedExtened GsmBuffer
                                    //BYTES ALLOCATED   : 4096
/*******************************************************************************************************/
#define DecodeExtended_GSMBuffer_SA     0xFF000
#define DecodeExtended_GSMBuffer_EA     0xFFFFF

/****************BASEBOARD UPGRADE**************************************************/

/*****************Application data*********************************************/
//SECTOR 8 to SECTOR 15 : 0x70000 to 0xEFFFF

#define Application_Data_SA            0x70000
#define Application_Data_EA            0x8FFFF

#define MAXIMUM_ALLOWABLE_LENGTH_TO_WRITE_IN_FLASH  0x8FFFF //(0xFFFFF - 1024)


/********************************************************************************
 * FLASH memory address
 ********************************************************************************/


#define Success             1
#define Failed              0
#define Function_Success    0xf0
#define Function_Success1   0xf1
#define Init_tcp_Success    0xf2
#define StartTCPSuccess     0xf3
#define Pass_Success        0xf4
#define Server_Busy         0xf5



#endif /* SOURCE_HEADER_H_ */
