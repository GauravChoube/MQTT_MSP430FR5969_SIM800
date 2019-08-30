/********************************************************************************
  *  File Name  : Mqtt.c
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
char Mqtt_Connection_Established_State = 1;
char MQTTsendPacket_State              = 1;
char Subscribe_State                   = 1;
char Publish_State                     = 1;

MQTTPacket_connectData ConnnectPacket = MQTTPacket_connectData_initializer;
MQTTString topicString = MQTTString_initializer;

unsigned int MQtt_Packet_response = 0xff;

#pragma PERSISTENT(Mqtt_Com_Buf)
char Mqtt_Com_Buf[Serialised_Buffer_Size] ={0};

#pragma PERSISTENT(Serialised_Buffer)
char Serialised_Buffer[Serialised_Buffer_Size] = {0};

unsigned int MQttpacketlen = 0;
char PubQos = 0;
char MQtt_Funcretry = 0;
/********************************************************************************
 *    Function Defination Start
 */
/***********************************************
 * Mqtt_Connection_Established_Success
 * TCP_CONNECTION_CLOSED
 * Failed
 * server_busy
 * ATMaxtriedOccured
 *
 */
unsigned int Mqtt_Connection_Established(char *clientid,char *Ipaddress,char *Port,char *Password,char *Username)
{
   unsigned int rval = 0xff;

    switch(Mqtt_Connection_Established_State)
    {

    case 1:
    {
        rval = TCPConnection_Status(0);

    }break;

    case 2:
    {

          ConnnectPacket.clientID.cstring = clientid;
          ConnnectPacket.username.cstring  = Username;
          ConnnectPacket.password.cstring = Password;
          ConnnectPacket.cleansession     = 0 ;
          ConnnectPacket.keepAliveInterval = 300;

          MQttpacketlen = 0;
          memset(Serialised_Buffer , 0x00, Serialised_Buffer_Size);

         MQttpacketlen =  MQTTSerialize_connect((unsigned char *)Serialised_Buffer, Serialised_Buffer_Size, &ConnnectPacket);

         Mqtt_Connection_Established_State++;
    }

    case 3:
    {
//              rval = Init_Tcp();
              rval = Init_tcp_Success;
    }break;

    case 4:
    {
        rval = SATRT_TCP_UDP_SSL_CONNECTION(0, TCP_MODE, Ipaddress, Port);

    }break;

    case 5:
    {
        rval = MQTTSendPacket(Serialised_Buffer, MQttpacketlen, ConnectBrokerPacket);
    }break;

    default:
        rval = Mqtt_Connection_Established_Success;
        Mqtt_Connection_Established_State = 0;

    }

    Mqtt_return_value(&Mqtt_Connection_Established_State, &rval);
    return rval;
}


void Mqtt_return_value(char *state_variable,unsigned int *rval)
{

    switch(*rval)
    {
    case TCP_ALREADY_CONNECTED: ATMaxtry = 0;*state_variable = 5;MQtt_Funcretry = 0;break;

    case TCP_SEND_SUCCESS:
    case MQTT_ACK_RECV: *rval = Pass_Success;
    case TCP_INITIAL:
    case Subscribe_Success:
    case MQTTSendPacket_Success:

    case Publish_Success:
    case StartTCPSuccess:
    case Init_tcp_Success:
    case Function_Success:
    case Mqtt_Connection_Established_Success:MQtt_Funcretry = 0; ATMaxtry = 0; (*state_variable)++;break;

    case TCP_CLOSED:
    case TCP_SEND_FAILED:
    case TCP_CONNECTION_CLOSED: ATMaxtry = 0; *rval = TCP_CONNECTION_CLOSED;*state_variable = 1;  Stop_GSM_MaxTimer(); break;

    case Server_Busy:MQtt_Funcretry = 0;  *state_variable = 1;break;

    case TCP_NOT_CONNNECTED : *rval = Failed;ATMaxtry = 0; break;

    case ATMaxtriedOccured:*state_variable = 1;ATMaxtry = 0; break;

    case TCP_CONNECTION_PROGRESSING:wait_for_second(2);
    case MQTT_ACK_FAILE:
    case TCP_SENDTIMEOUT_OCCURED:
                     if(++MQtt_Funcretry >= 3)
                     {
                         *rval = Server_Busy;
                         *state_variable = 1;
                     }
    case Failed:
    default:
          break;
    }



}


/*****************************************************
 *
 * Subscribe_Success
 * TCP_CONNECTION_CLOSED
 * Server_Busy
 * failed
 * ATMaxtriedOccured
 */
unsigned int Subscribe(unsigned char * Sub_Topic,int Qos)
{
    unsigned int rval = 0xff;

    switch(Subscribe_State)
    {
    case 1:
    {
        MQttpacketlen = 0;
        memset(Mqtt_Com_Buf, 0x00,sizeof(Mqtt_Com_Buf));
        memset(Serialised_Buffer, 0x00, Serialised_Buffer_Size);
        topicString.cstring = (char *)Sub_Topic;

        MQttpacketlen = MQTTSerialize_subscribe((unsigned char *)Serialised_Buffer, Serialised_Buffer_Size, 0, ++PcktID_send, 1, &topicString, &Qos);

        Subscribe_State++;

    }
    case 2:
    {
        rval = MQTTSendPacket(Serialised_Buffer, MQttpacketlen, SubscribePacket);
    }break;

    default:
        Subscribe_State = 0;
        rval = Subscribe_Success;


    }

    Mqtt_return_value(&Subscribe_State, &rval);



   return rval;
}


/*************************************************
 * Publish_Success
 * FAILED
 * TCP_CONNECTION_CLOSED
 * Server_Busy
 * ATMaxtriedOccured
 ************************************************/
unsigned int Publish(char *Pub_Topic,char * Buffer,unsigned int Buffer_len,char Qos)
{
    unsigned int rval = 0xff;
    switch(Publish_State)
    {
    case 1:
    {
        PubQos = Qos;
        topicString.cstring = Pub_Topic;
        memset(Serialised_Buffer, 0x00, sizeof(Serialised_Buffer));

        MQttpacketlen = MQTTSerialize_publish((unsigned char*)Serialised_Buffer, sizeof(Serialised_Buffer), 0, Qos, 0, ++PcktID_send, topicString, (unsigned char *)Buffer, Buffer_len);

        Publish_State++;
    }

    case 2:
    {

          rval =  MQTTSendPacket(Serialised_Buffer, MQttpacketlen, PublishPacket);

    }break;

    default:
        Publish_State = 0;
        rval = Publish_Success;

    }
    Mqtt_return_value(&Publish_State, &rval);

    return rval;
}

/*************************************************
 * MQTTSendPacket_Success
 * FAILED
 * TCP_CONNECTION_CLOSED
 * Server_Busy
 * ATMaxtriedOccured
 */

unsigned int MQTTSendPacket(char *MQttPayload,unsigned int MQttLenPacket,unsigned int TypeOFpacket)
{
    unsigned int rval = 0xff;

    switch(MQTTsendPacket_State)
    {
    case 1:
    {
        rval = TCP_SEND(0,MQttPayload, MQttLenPacket);
        Stop_GSM_MaxTimer();
        Start_GSM_MaxTimer(60);

    }break;
    case 2:
    {
        if(TypeOFpacket == PublishPacket)
        {
            if(PubQos == 0)
            {
                rval = MQTT_ACK_RECV;
                Stop_GSM_MaxTimer();
                MQtt_Packet_response =0xff;
                break;
            }
        }
        if(MQtt_Packet_response == Puback_response)
        {
            rval = MQTT_ACK_RECV;
            Stop_GSM_MaxTimer();
            MQtt_Packet_response =0xff;
        }
        else if(MQtt_Packet_response == Puback_res_failed)
        {
            Stop_GSM_MaxTimer();
            rval = MQTT_ACK_FAILE;
            MQtt_Packet_response =0xff;
        }else
            if(MQtt_Packet_response == Connack_response)
            {
                rval = MQTT_ACK_RECV;
                Stop_GSM_MaxTimer();
                MQtt_Packet_response =0xff;
            }
            else if(MQtt_Packet_response == Connack_res_failed)
            {
                Stop_GSM_MaxTimer();
                rval = MQTT_ACK_FAILE;
                MQtt_Packet_response =0xff;
            }
            else
                if(MQtt_Packet_response == Suback_response)
                {
                    rval = MQTT_ACK_RECV;
                    Stop_GSM_MaxTimer();
                    MQtt_Packet_response =0xff;
                }
                else if(MQtt_Packet_response == Suback_res_failed)
                {
                    rval = MQTT_ACK_FAILE;
                    Stop_GSM_MaxTimer();
                    MQtt_Packet_response =0xff;
                }
                else if(GSMCmdtimout == true)
                {
//                    MQTTsendPacket_State = 1;
                    rval = Server_Busy;
                    Stop_GSM_MaxTimer();
                    MQtt_Packet_response =0xff;
                }
                else if(MQtt_Packet_response == TCP_CONNECTION_CLOSED)
                {
                    Stop_GSM_MaxTimer();
                    rval = TCP_CONNECTION_CLOSED;
                    MQtt_Packet_response =0xff;
                }
                else
                    rval = Pass_Success;

    }
        break;
    default:
          rval = MQTTSendPacket_Success;
          MQTTsendPacket_State=0;

    }


    Mqtt_return_value(&MQTTsendPacket_State, &rval);

    return rval;

}


unsigned int Proccess_Unsolicated_Response()
{
    unsigned int rval =0xff;
    char *StartAddPAcket = 0;
    unsigned char SizeofMqttPacket[5] ={0};
    unsigned int LenMQTTpacket = 0;
    unsigned int TotalLenMQTTPayload = 0;
    unsigned int RemainlenBuf = 0;
    volatile unsigned int  TotalExcdLEnofSinglepayload =0;

    unsigned int tempPUR = 0;

    unsigned char MQttMsgType= 0x00;
    unsigned char sessionPresent=0;

    unsigned char MaxCount = 2;
    int Count = 0;
    unsigned short RecvPacketID = 0;
    int GrantedQoSs[4]={0};


    memset(Mqtt_Com_Buf, 0x00, sizeof(Mqtt_Com_Buf));
    memset(SizeofMqttPacket, 0x00, 5);
    LenMQTTpacket = 0;
    tempPUR = 0;
    TotalLenMQTTPayload = 0;
    unsigned char tmp_rval =0xff;









    if(strnstr((char *)GSM_Buffer,"CLOSED",GSM_Buffer_Index) ||strnstr((char *)GSM_Buffer,"+PDP: DEACT",GSM_Buffer_Index))
    {

            MQtt_Packet_response = TCP_CONNECTION_CLOSED;


//            if(strnstr((char *)GSM_Buffer,"0,CLOSED",GSM_Buffer_Index) == 0)
//            {
//                memset(&wifiCfgBuffer[0], 0x00, sizeof(WifiBuf));
//            }
//            if (strnstr((char *)GSM_Buffer,"1,CLOSED",GSM_Buffer_Index) == 0)
//            {
//                memset(&wifiCfgBuffer[1], 0x00, sizeof(WifiBuf));
//            }
//            if (strnstr((char *)GSM_Buffer,"2,CLOSED",GSM_Buffer_Index) == 0)
//            {
//                memset(&wifiCfgBuffer[2], 0x00, sizeof(WifiBuf));
//            }
//            if (strnstr((char *)GSM_Buffer,"3,CLOSED",GSM_Buffer_Index) == 0)
//            {
//                memset(&wifiCfgBuffer[3], 0x00, sizeof(WifiBuf));
//            }

            return TCP_CONNECTION_CLOSED;

    }


    while(StartAddPAcket = strnstr((char *)GSM_Buffer,"+RECEIVE",GSM_Buffer_Index))
    {

        memset(Mqtt_Com_Buf, 0x00, sizeof(Mqtt_Com_Buf));
        memset(SizeofMqttPacket, 0x00, 5);
        LenMQTTpacket = 0;

        tempPUR = 0;
        TotalLenMQTTPayload = 0;

        //it give the length of MQtt packet coming with packet
        while((StartAddPAcket[LenMQTTpacket+11] != ':')  && LenMQTTpacket < 5)
        {
            SizeofMqttPacket[LenMQTTpacket] = StartAddPAcket[LenMQTTpacket+11];
            LenMQTTpacket++;
        }


        LenMQTTpacket = 0;
        LenMQTTpacket = atoi((char *)SizeofMqttPacket);
        tempPUR = 11+ strlen((char *)SizeofMqttPacket)+3;
        TotalLenMQTTPayload = LenMQTTpacket + tempPUR; // e.g. +RECEIVE,0,124:<CR><LF> ==> lenght of one MQtt PAyload = len(+RECEIVE,0,)11+1(:)+streln(124) +2(<CR><LF)=17+124=151
        RemainlenBuf = GSM_Buffer_Size -(StartAddPAcket - GSM_Buffer);

        if(LenMQTTpacket > RemainlenBuf)
        {
            TotalExcdLEnofSinglepayload = LenMQTTpacket;
            LenMQTTpacket =    GSM_Buffer_Size-((StartAddPAcket+tempPUR)-GSM_Buffer);

        }

        memcpy(Mqtt_Com_Buf,(StartAddPAcket+tempPUR),LenMQTTpacket);


        if(TotalLenMQTTPayload > RemainlenBuf)
        {
            TotalLenMQTTPayload = RemainlenBuf-1 ;
            GSM_Buffer_Index = 1;
            memset(StartAddPAcket, 0x00, TotalLenMQTTPayload-1);
        }
        else
        {
            memset(StartAddPAcket, 0x00, TotalLenMQTTPayload);
        }



        MQttMsgType = (Mqtt_Com_Buf[0] >> 4) & 0x0f;


        switch(MQttMsgType)
        {
        case 2:
        {
            //MQttConnect acknowlege packet having connection status
            MQTTDeserialize_connack(&sessionPresent, &tmp_rval,(unsigned char *) Mqtt_Com_Buf, LenMQTTpacket);
            switch(tmp_rval)
            {
            case 0:  rval=Connack_response; MQtt_Packet_response=Connack_response; break;
            case 1:
            case 2: Debug_Uart_Str("\nMQtt client id is not accepted..changed it..\n"); PMM_trigBOR();break;

            case 4: Debug_Uart_Str("\nMQtt username and password are incorrect.\n");PMM_trigBOR();break;

            default:
            case 5:
            case 3: rval=Connack_res_failed; MQtt_Packet_response=Connack_res_failed;break;
            }
            if (tmp_rval == 0)
            {
                rval=Connack_response;
                MQtt_Packet_response=Connack_response;
            }
            else
            {
                rval=Connack_res_failed;
                MQtt_Packet_response=Connack_res_failed;
            }

        }break;

        case 9:
        {
            //MQtt Subcribe PAcket Acknowlege
            MQTTDeserialize_suback(&RecvPacketID, MaxCount, &Count, GrantedQoSs, (unsigned char *)Mqtt_Com_Buf, LenMQTTpacket);
            if(RecvPacketID == PcktID_send)
            {
                rval=Suback_response;
                MQtt_Packet_response=Suback_response;
            }
            else
            {
                rval=Suback_res_failed;
                MQtt_Packet_response=Suback_res_failed;
            }

        }break;

        case 3:
        {
            //publish Mqtt msg received mean from Cloud to Device
           rval = Process_SubcribeMsg();
//           MQtt_Packet_response = rval;

        }break;

        case 4:
        {
            //PUblish mqtt packet acknowlegement
            unsigned char PacktType=0;
            unsigned char Dup=0;
            MQTTDeserialize_ack(&PacktType, &Dup, &RecvPacketID, (unsigned char *) Mqtt_Com_Buf, LenMQTTpacket);
            if(RecvPacketID == PcktID_send)
            {
                rval=Puback_response;
                MQtt_Packet_response=Puback_response;
            }
            else
            {
                rval=Puback_res_failed;
                MQtt_Packet_response=Puback_res_failed;
            }
        }break;

        case 0:
        {
            return rval;
        }

        default:
        {



        }

        }
    }


  return rval;
}


unsigned int Process_SubcribeMsg()
{
    unsigned int rval = 0xff;
    unsigned int len=0;
    unsigned short PcktID=0;
    unsigned char dup;
    int qos;
    unsigned char retained;
    int returnCode;
    unsigned char* payload_in = 0;
    int payloadlen_in;
    char Topic[30]={0};
    char QosRespBuff[20]={0};
    MQTTString receivedTopic = MQTTString_initializer;


    returnCode = MQTTDeserialize_publish(&dup, &qos, &retained, &PcktID, &receivedTopic, &payload_in, &payloadlen_in,(unsigned char *) Mqtt_Com_Buf, strlen((char *)Mqtt_Com_Buf));

    if(returnCode)
    {

        strncpy(Topic , (char*)receivedTopic.lenstring.data , receivedTopic.lenstring.len);
        if(memcmp(Topic ,Subscribe_Topic,strlen(Subscribe_Topic)) == 0)
        {

            //            App_Response((char *)payload_in,payloadlen_in);
            Tx_Uart1_String("this is message received:");
            Tx_Uart1_String((char *)payload_in);

        }


        switch(qos)
        {

        case 0:
        {

        }break;
        case 1:
        {
            unsigned int Tmpfortimeindex = GSM_Buffer_Index;
            memset(Mqtt_Com_Buf, 0x00, sizeof(Mqtt_Com_Buf));
            strncpy((char *)Mqtt_Com_Buf,GSM_Buffer,GSM_Buffer_Size);
            ATMaxtry = 0;

            len= MQTTSerialize_puback((unsigned char *)QosRespBuff,sizeof(QosRespBuff),PcktID);

            for(returnCode = 0; returnCode < 3; returnCode++)
            {
                Processing_unsolic_msg = 1;
                rval = TCP_SEND(0,QosRespBuff,len);

                if(rval ==  TCP_SEND_SUCCESS)
                {
                    if(GSM_Buffer_Index < GSM_Buffer_Size)
                    {
                        strncat(GSM_Buffer+GSM_Buffer_Index,(char *)Mqtt_Com_Buf, GSM_Buffer_Size-GSM_Buffer_Index-1);
                        GSM_Buffer_Index+=Tmpfortimeindex;
                    }
                    break;
                }
                else
                    if(rval == TCP_CONNECTION_CLOSED || rval == TCP_SEND_FAILED)
                    {
                        GSM_Buffer_Index = 0;

                        MQtt_Packet_response =  rval = TCP_CONNECTION_CLOSED;
                        return rval;
                    }

                ATMaxtry = 0;


            }

            if(returnCode >= 4)
            {
                GSM_Buffer_Index = 0;
                rval = TCP_CONNECTION_CLOSED;
                return rval;
            }
            else
            {

            }

        }break;

        case 2:
        {

        }break;
        }



    }
    else
    {
        Tx_Uart1_String("Unsolicated msg read failed\n");

    }

    Processing_unsolic_msg = false;

    return rval;
}


//unsigned int Mqtt_PAcket_ack(char pckt_id,char*packet,int len)
//{
//    unsigned int rval = 0xff;
//
//
//    Start_Buffering=true;
//    GSMCmdtimout=false;
//    clear_buffer();
//    GSM_Buffer_Ready=false;
//
//    Tx_Uart1_String("AT+CIPSEND=");
//    Tx_Uart1_Char(pckt_id+48);
//    Tx_Uart1_String(",4\r\n");
//    Start_GSM_MaxTimer(20);
//    while(!(GSM_Buffer_Ready || GSMCmdtimout));
//
//
//    return rval;
//}
