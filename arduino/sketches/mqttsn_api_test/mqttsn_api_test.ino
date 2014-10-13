////////////////////////////////////////////////////////////////////////////////
/// Arduino sketch for MQTT-SN over XBee
///
/// \author    Zvi Avraham
/// \copyright 2012-2014 ZADATA Ltd. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////

#include <XBee.h>
#include <mqttsn.h>
//#include <NewSoftSerial.h>

XBee xbee = XBee();

uint8_t payload[] = { 0, 0, 0, 0};

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200,0x408698F1);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

MQTTSMessage mqttsMessage = MQTTSMessage();
int key = MQTTS_ADVERTISE;
uint8_t Radius = 100;
uint16_t Duration = 101;
uint8_t GwId = 102;
uint8_t GwAdd = 103;
uint8_t Flags = 104;
uint8_t ProtocolId = 105;
//uint16_t ClientId = 106;
uint8_t ReturnCode = 107;
uint16_t TopicId = 108;
uint16_t MsgId = 109;

char * tmp;

void flashLed(int pin, int times, int wait) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}

void setup() {
  xbee.begin(9600);
}

void loop() 
{
  free(mqttsMessage.getMQTTSCommand()); 
  if (key == MQTTS_ADVERTISE) {
    mqttsMessage.setMQTTSCommand(new MQTTSAdvertise(Radius, Duration));
    Serial.print("MQTTSAdvertise\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
    Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
    Serial.print("MQTTSAdvertise radius = "); Serial.print(static_cast<MQTTSAdvertise*>(mqttsMessage.getMQTTSCommand())->getRadius()); Serial.println(" ");
    Serial.print("MQTTSAdvertise duration = "); Serial.print(static_cast<MQTTSAdvertise*>(mqttsMessage.getMQTTSCommand())->getDuration()); Serial.println("\n");
    key = MQTTS_SEARCHGW;
  } else if (key == MQTTS_SEARCHGW) {
      mqttsMessage.setMQTTSCommand(new MQTTSSearchGw(Radius));
      Serial.print("MQTTSSearchGw\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
      Serial.print("MQTTSSearchGw radius = "); Serial.print(static_cast<MQTTSSearchGw*>(mqttsMessage.getMQTTSCommand())->getRadius()); Serial.println(" \n");
      key = MQTTS_GWINFO;
//  } else if (key == MQTTS_GWINFO) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSGwInfo(GwId, GwAdd)));
//      Serial.print("MQTTSGwInfo\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("MQTTSGwInfo gwid = "); Serial.print(static_cast<MQTTSGwInfo*>(mqttsMessage.getMQTTSCommand())->getGwId()); Serial.println(" ");
//      Serial.print("MQTTSGwInfo gw add = "); Serial.print(static_cast<MQTTSGwInfo*>(mqttsMessage.getMQTTSCommand())->getGwAdd()); Serial.println(" \n");
//      key = MQTTS_CONNECT;
//  } else if (key == MQTTS_CONNECT) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSConnect(Flags,ProtocolId,Duration,"arduino 1")));
//      Serial.print("MQTTSConnect\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("MQTTSConnect flags = "); Serial.print(static_cast<MQTTSConnect*>(mqttsMessage.getMQTTSCommand())->getFlags().flags); Serial.println(" ");
//      Serial.print("MQTTSConnect protocol id = "); Serial.print(static_cast<MQTTSConnect*>(mqttsMessage.getMQTTSCommand())->getProtocolId()); Serial.println(" ");
//      Serial.print("MQTTSConnect duration = "); Serial.print(static_cast<MQTTSConnect*>(mqttsMessage.getMQTTSCommand())->getDuration()); Serial.println(" ");
//      tmp = strdup(static_cast<MQTTSConnect*>(mqttsMessage.getMQTTSCommand())->getClientId());
//      Serial.print("client id = "); Serial.print(tmp); Serial.println(" \n");
//      free(tmp);
//      key = MQTTS_CONNACK;
//  } else if (key == MQTTS_CONNACK) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSConAck(ReturnCode)));
//      Serial.print("MQTTSConAck\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("return code = "); Serial.print(static_cast<MQTTSConAck*>(mqttsMessage.getMQTTSCommand())->getReturnCode()); Serial.println(" \n");
//      key = MQTTS_WILLTOPICREQ;
//  } else if (key == MQTTS_WILLTOPICREQ) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSWillTopicReq()));
//      Serial.print("MQTTSWillTopicReq\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" \n");
//      key = MQTTS_WILLTOPIC;
//  } else if (key == MQTTS_WILLTOPIC) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSWillTopic(Flags, "it's work!!")));
//      Serial.print("MQTTSWillTopic\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("flags = "); Serial.print(static_cast<MQTTSWillTopic*>(mqttsMessage.getMQTTSCommand())->getFlags().flags); Serial.println(" ");
//      tmp = strdup(static_cast<MQTTSWillTopic*>(mqttsMessage.getMQTTSCommand())->getWillTopic());
//      Serial.print("willTopic = "); Serial.print(tmp); Serial.println(" \n");
//      free(tmp);
//      key = MQTTS_WILLMSGREQ;
//  } else if (key == MQTTS_WILLMSGREQ) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSWillMsgReq()));
//      Serial.print("MQTTSWillMsgReq\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" \n");
//      key = MQTTS_WILLMSG;
//  } else if (key == MQTTS_WILLMSG) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSWillMsg("will message")));
//      Serial.print("MQTTSWillMsg\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      tmp = strdup(static_cast<MQTTSWillMsg*>(mqttsMessage.getMQTTSCommand())->getWillMsg());
//      Serial.print("willMsg = "); Serial.print(tmp); Serial.println(" \n");
//      free(tmp);
//      key = MQTTS_REGISTER;
//  } else if (key == MQTTS_REGISTER) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSRegister(TopicId, MsgId, "register message")));
//      Serial.print("MQTTSRegister\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("topic id = "); Serial.print(static_cast<MQTTSRegister*>(mqttsMessage.getMQTTSCommand())->getTopicId()); Serial.println(" ");
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSRegister*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" ");
//      tmp = strdup(static_cast<MQTTSRegister*>(mqttsMessage.getMQTTSCommand())->getTopicName());
//      Serial.print("topic name = "); Serial.print(tmp); Serial.println(" \n");
//      free(tmp);
//      key = MQTTS_REGACK;
//  } else if (key == MQTTS_REGACK) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSRegAck(TopicId, MsgId, ReturnCode)));
//      Serial.print("MQTTSRegAck\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("topic id = "); Serial.print(static_cast<MQTTSRegAck*>(mqttsMessage.getMQTTSCommand())->getTopicId()); Serial.println(" ");
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSRegAck*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" ");
//      Serial.print("return code = "); Serial.print(static_cast<MQTTSRegAck*>(mqttsMessage.getMQTTSCommand())->getReturnCode()); Serial.println(" \n");
//      key = MQTTS_PUBLISH;
//      //Serial.print("key = "); Serial.print(key); Serial.println(" \n");
//  } else if (key == MQTTS_PUBLISH) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSPublish(Flags, TopicId, MsgId, "this is data")));
//      Serial.print("MQTTSPublish\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("flags = "); Serial.print(static_cast<MQTTSPublish*>(mqttsMessage.getMQTTSCommand())->getFlags().flags); Serial.println(" ");
//      Serial.print("topic id = "); Serial.print(static_cast<MQTTSPublish*>(mqttsMessage.getMQTTSCommand())->getTopicId()); Serial.println(" ");
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSPublish*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" ");
//      tmp = strdup(static_cast<MQTTSPublish*>(mqttsMessage.getMQTTSCommand())->getData());
//      Serial.print("data = "); Serial.print(tmp); Serial.println(" \n");
//      free(tmp);
//      key = MQTTS_PUBACK;
//  } else if (key == MQTTS_PUBACK) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSPubAck(TopicId, MsgId, ReturnCode)));
//      Serial.print("MQTTSPubAck\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("topic id = "); Serial.print(static_cast<MQTTSPubAck*>(mqttsMessage.getMQTTSCommand())->getTopicId()); Serial.println(" ");
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSPubAck*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" ");
//      Serial.print("return code = "); Serial.print(static_cast<MQTTSPubAck*>(mqttsMessage.getMQTTSCommand())->getReturnCode()); Serial.println(" \n");
//      key = MQTTS_PUBREC;
//  } else if (key == MQTTS_PUBREC) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSPubRec(MsgId)));
//      Serial.print("MQTTSPubRec\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSPubRec*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" \n");
//      key = MQTTS_PUBREL;
//  } else if (key == MQTTS_PUBREL) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSPubRel(MsgId)));
//      Serial.print("MQTTSPubRel\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSPubRel*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" \n");
//      key = MQTTS_PUBCOMP;
//  } else if (key == MQTTS_PUBCOMP) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSPubComp(MsgId)));
//      Serial.print("MQTTSPubComp\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSPubComp*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" \n");
//      key = MQTTS_SUBSCRIBE;
//  } else if (key == MQTTS_SUBSCRIBE) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSSubscribe(Flags, TopicId, MsgId)));
//      Serial.print("MQTTSSubscribe\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("flags = "); Serial.print(static_cast<MQTTSSubscribe*>(mqttsMessage.getMQTTSCommand())->getFlags().flags); Serial.println(" ");
//      Serial.print("topic id = "); Serial.print(static_cast<MQTTSSubscribe*>(mqttsMessage.getMQTTSCommand())->getTopicId()); Serial.println(" ");      
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSSubscribe*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" \n");
//      key = MQTTS_SUBACK;
//  } else if (key == MQTTS_SUBACK) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSSubAck(Flags, TopicId, MsgId, ReturnCode)));
//      Serial.print("MQTTSSubAck\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("flags = "); Serial.print(static_cast<MQTTSSubAck*>(mqttsMessage.getMQTTSCommand())->getFlags().flags); Serial.println(" ");
//      Serial.print("topic id = "); Serial.print(static_cast<MQTTSSubAck*>(mqttsMessage.getMQTTSCommand())->getTopicId()); Serial.println(" ");      
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSSubAck*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" ");
//      Serial.print("return code = "); Serial.print(static_cast<MQTTSSubAck*>(mqttsMessage.getMQTTSCommand())->getReturnCode()); Serial.println(" \n");
//      key = MQTTS_UNSUBSCRIBE;
//  } else if (key == MQTTS_UNSUBSCRIBE) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSUnsubscribe(Flags, TopicId, MsgId)));
//      Serial.print("MQTTSUnsubscribe\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("flags = "); Serial.print(static_cast<MQTTSUnsubscribe*>(mqttsMessage.getMQTTSCommand())->getFlags().flags); Serial.println(" ");
//      Serial.print("topic id = "); Serial.print(static_cast<MQTTSUnsubscribe*>(mqttsMessage.getMQTTSCommand())->getTopicId()); Serial.println(" ");      
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSUnsubscribe*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" \n");
//      key = MQTTS_UNSUBACK;
//  } else if (key == MQTTS_UNSUBACK) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSUnsubAck(MsgId)));
//      Serial.print("MQTTSUnsubAck\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("message id = "); Serial.print(static_cast<MQTTSUnsubAck*>(mqttsMessage.getMQTTSCommand())->getMsgId()); Serial.println(" \n");
//      key = MQTTS_PINGREQ;
//  } else if (key == MQTTS_PINGREQ) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSPingReq("arduino 2")));
//      Serial.print("MQTTSPingReq\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      tmp = strdup(static_cast<MQTTSPingReq*>(mqttsMessage.getMQTTSCommand())->getClientId());
//      Serial.print("client id = "); Serial.print(tmp); Serial.println(" \n");
//      free(tmp);
//      key = MQTTS_PINGRESP;
//  } else if (key == MQTTS_PINGRESP) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSPingResp()));
//      Serial.print("MQTTSPingResp\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" \n");
//      key = MQTTS_DISCONNECT;
//  } else if (key == MQTTS_DISCONNECT) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSDisconnect(Duration)));
//      Serial.print("MQTTSUnsubAck\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("duration = "); Serial.print(static_cast<MQTTSDisconnect*>(mqttsMessage.getMQTTSCommand())->getDuration()); Serial.println(" \n");
//      key = MQTTS_WILLTOPICUPD;
//  } else if (key == MQTTS_WILLTOPICUPD) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSWillTopicUpd(Flags, "will topic update")));
//      Serial.print("MQTTSUnsubAck\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      Serial.print("flags = "); Serial.print(static_cast<MQTTSWillTopicUpd*>(mqttsMessage.getMQTTSCommand())->getFlags().flags); Serial.println(" ");
//      tmp = strdup(static_cast<MQTTSWillTopicUpd*>(mqttsMessage.getMQTTSCommand())->getWillTopic());
//      Serial.print("will topic = "); Serial.print(tmp); Serial.println(" \n");
//      free(tmp);
//      key = MQTTS_WILLTOPICRESP;
//  } else if (key == MQTTS_WILLTOPICRESP) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSWillTopicResp()));
//      Serial.print("MQTTSWillTopicResp\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" \n");
//      key = MQTTS_WILLMSGUPD;
//  } else if (key == MQTTS_WILLMSGUPD) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSWillMsgUpd("will msg update")));
//      Serial.print("MQTTSWillMsgUpd\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" ");
//      tmp = strdup(static_cast<MQTTSWillMsgUpd*>(mqttsMessage.getMQTTSCommand())->getWillMsg());
//      Serial.print("will topic = "); Serial.print(tmp); Serial.println(" \n");
//      free(tmp);
//      key = MQTTS_WILLMSGRESP;
//  } else if (key == MQTTS_WILLMSGRESP) {
//      mqttsMessage.setMQTTSCommand(static_cast<MQTTSCommand*>(new MQTTSWillMsgResp()));
//      Serial.print("MQTTSWillMsgResp\nmessage Length = "); Serial.print(mqttsMessage.getLength()); Serial.println(" ");
//      Serial.print("command Length = "); Serial.print(mqttsMessage.getMQTTSCommand()->getCommandLength()); Serial.println(" \n");
//      key = MQTTS_ADVERTISE;
  }
  delay(100);
}
