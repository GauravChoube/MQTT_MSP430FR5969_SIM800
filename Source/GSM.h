/****************************************************************************
 *  File Name  : GSM.h
 *  Created on : Oct 20, 2018
 *  Author     : Gaurav Choubey
 ****************************************************************************/
#ifndef SOURCE_GSM_H_
#define SOURCE_GSM_H_

/*****************************************************************************
 * Varibale
 *****************************************************************************/
bool    ATCmd;
char    ATMaxtry;
bool    Resp_received;
bool    GSMCmdtimout;
bool    GSM_Buffer_Ready;
/*****************************************************************************
 * macros => 0x6000-0x6FFF
 *****************************************************************************/
//GSM Cmd for initialisation of GSMmodem

#define Init_GSM_CMD                                        0
#define AT_RESTORE_IQ                                       55
#define AT_ATTENTION_COMMAND                                0
#define AT_DISABLE_VERBOSE                                  1
#define AT_TURN_OFF_ECHO                                    2
#define AT_AUTO_BAUD_ENABLE                                 3
#define AT_REJECT_INCOMING_CALLS                            4
#define AT_CHECK_AUTO_TIMESTAMP_ENABLE                      5
#define AT_ENABLE_TIMESTAMP_AUTO_UPDATE                     6
#define AT_CLOCK                                            50
#define AT_REQUEST_IMEI                                     7
#define AT_REQUEST_IMSI                                     8
#define AT_SIM_NUMBER                                       9
#define AT_SAVE                                             10
#define AT_GET_GSM_LOCATION                                 35
#define AT_GET_MOBILE_OPERATOR_NAME                         36
#define AT_GET_BATTERY_PARAMETERS                           37
#define AT_CHECK_STK_SWITCH                                 38
#define AT_TURN_ON_STK                                      39

//GSM Cmd for check for network
#define AT_SIM_PRESENCE_CMD                                 11
#define AT_CHECK_NETWORK                                    12
#define AT_CHECK_GPRS_ENABLE                                13
#define AT_ENABLE_GPRS                                      14
#define AT_SIGNAL_STRENGTH                                  15

//GSM CMD For TCP
#define AT_TCP_SHUT                                         16
#define AT_TCP_ENABLE_MULTIPLE                              17
#define AT_TCP_SET_APN                                      18
#define AT_TCP_BRINGUP_GPRS                                 19
#define AT_TCP_GET_LOCAL_IP                                 20
#define AT_TCP_START_CONNECTION                             21
#define AT_TCP_START_CONNECTION_PLUS                        80
#define AT_TCP_SEND_DATA                                    22
#define AT_TCP_CLOSE_CONNECTION                             23
#define AT_TCP_STATUS                                       81
#define AT_TCP_SEND_DATA_PLUS                               82

//GSM Cmd For HTTP
#define AT_HTTP_SET_BEARER_SETTINGS                         24
#define AT_HTTP_BEARER_APN_SETTINGS                         25
#define AT_HTTP_GET_LOCAL_IP                                26
#define AT_HTTP_BRINGUP_GPRS                                27
#define HTTP_METHOD_GET                                      0
#define HTTP_METHOD_POST                                     1
#define AT_HTTPINIT           106
#define AT_HTTPPARA           107
#define AT_HTTPDATA           108
#define AT_HTTPACTION         109
#define AT_HTTPTERM            110
#define AT_HTTPREAD           111

//GPS CMD
#define AT_CGNSPWR_CHECK                                    90
#define AT_CGNSPWR                                          91
#define AT_CGNSINF                                          92
#define AT_CGNSSEQ                                          93






/******************************************************************************
 * Micro: return code for AT command
 *1) success on successful command
 *2) failed on error
 *3) value on particular command suitation
 *
 ******************************************************************************/
//AT+SAPBR
#define HTTP_CONNECTION_NO_ESTABLISHED              0x1100
#define HTTP_CONNECTION_ESTABLISHED                 0x1101
//AT+CIMINS
#define SIM_CARD_ABESENT                            0x1102
#define SIM_CARD_PRESENT                            0x1103

//AT+CGATT
#define GPRS_ATTACHED                               0x1104
#define GPRS_DEATTACHED                             0x1105

//AT+CLK?
#define RTC_VALID_TIME                              0x1104
#define GPRS_DEATTACHED                             0x1105

//AT_CHECK_NETWORK
#define NET_REGISTERED                              0x1106
#define NET_SEARCHING                               0x1107
#define NET_DENIED                                  0x1108

//AT+CIPGSMLOC
#define GSM_LOCATION_ERROR                          0x1109

#define TIMESTAMP_DISABLE                           0x110A

//AT+CIPSTATUS
#define TCP_ALREADY_CONNECTED                       0x110B
#define TCP_CLOSED                                  0x110C
#define TCP_INITIAL                                 0x110D
#define TCP_CONNECTION_PROGRESSING                  0x2222

//AT+CIPSRAT
#define TCP_CONNECTED                               0x110E
#define TCP_NOT_CONNNECTED                          0x110F
#define TCP_ERROR                                   0x1110


#define ATMaxtriedOccured                           0x1111

#define Gps_Enable                                  0x1112
#define Gps_Disable                                 0x1113
#define TCP_SENDTIMEOUT_OCCURED                     0x1114
#define TCP_SEND_SUCCESS                             0x1115
#define TCP_CONNECTION_CLOSED                       0x1116
#define TCP_SEND_FAILED                             0x111A
#define ConnectionClosed_SessionLOst                0x1117


#define TCP_MODE                                    0x1118
#define UDP_MODE                                    0x1119
#define init_http_success                            0x111A




#define TCP_Enable

/*****************************************************************************
 *  function
 *****************************************************************************/

unsigned int Init_GSM();
unsigned int Check_network();
unsigned int SendATCmd(char *cmd,unsigned char cmd_num,unsigned char timeout,unsigned char retry);
unsigned int ProcessGSMResp(unsigned char cmd);
void clear_buffer();
char *strnstr( char *src, char *dest,unsigned int n);
unsigned char TurnOff_Gsm();
unsigned char TurnOn_GSM();

unsigned int TCPConnection_Status(char socket_id);
unsigned int Init_Tcp();
unsigned int SATRT_TCP_UDP_SSL_CONNECTION(char Socket_id,int Connection_Mode,const char *Url,const char *Port);
unsigned int ShutAllConnection();
unsigned char WaitExtraResp(unsigned char time);
unsigned int TCP_SEND(char socket,char *data,unsigned int len);
void Check_Return_Value(unsigned char *state_variable,unsigned int *rval);


unsigned int Init_HTTP();
unsigned int Init_Http_TCP();

#endif /* SOURCE_GSM_H_ */
