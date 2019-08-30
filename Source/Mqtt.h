
/****************************************************************************
 *  File Name  : Mqtt.h
 *  Created on : Oct 20, 2018
 *  Author     : Gaurav Choubey
 ****************************************************************************/
#ifndef SOURCE_MQTT_H_
#define SOURCE_MQTT_H_

/*****************************************************************************
 * Varibale
 *****************************************************************************/
unsigned char PcktID_send;
bool Processing_unsolic_msg;
/*****************************************************************************
 * macros => 0x7000-0x7FFF
 *****************************************************************************/
#define Serialised_Buffer_Size    512

#define ConnectBrokerPacket    0x7000
#define SubscribePacket        0x7001
#define PublishPacket          0x7002

#define  MQTT_ACK_RECV        0x7003
#define  MQTT_ACK_FAILE       0x7004

#define Connack_response     0x7005
#define Connack_res_failed   0x7006
#define Suback_response      0x7007
#define Suback_res_failed    0x7008
#define Puback_response      0x7009
#define Puback_res_failed    0x700A

#define Mqtt_Connection_Established_Success 0x700B
#define MQTTSendPacket_Success              0x700C
#define Subscribe_Success                    0x700D
#define Publish_Success                      0x700E
/*****************************************************************************
 *  function
 *****************************************************************************/

void Mqtt_return_value(char *state_variable,unsigned int *rval);
unsigned int MQTTSendPacket(char *MQttPayload,unsigned int MQttLenPacket,unsigned int TypeOFpacket);
unsigned int Mqtt_Connection_Established(char *clientid,char *Ipaddress,char *Port,char *Password,char *Username);
unsigned int Subscribe(unsigned char * Sub_Topic,int Qos);
unsigned int Publish( char *Pub_Topic,char * Buffer,unsigned int Buffer_len,char Qos);
unsigned int Proccess_Unsolicated_Response();
unsigned int Process_SubcribeMsg();





#endif /* SOURCE_MQTT_H_ */
