////////////////////////////////////////////////////////////////////////////////
/// Arduino sketch for MQTT-SN over XBee
///
/// \author    Zvi Avraham
/// \copyright 2012-2014 ZADATA Ltd. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////

#include <XBee.h>
#include <mqttsn.h>
#include <MemoryFree.h>


// offset of data in reponse
#define DATA_FRAME_OFFSET 11 
// number of ping message failure before reconection yo gateway
#define NUM_OF_PINGS_TO_RECONNECT 3 

// create the XBee object
XBee xbee = XBee();

// SH + SL Address of receiving XBee from the router on the back of the xbee two lower lines
XBeeAddress64 gwAddr64 = XBeeAddress64(0x0013a200,0x408698F1); 
ZBTxRequest zbTx;
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

//TODO use FSM
//enum eState{
//  IDLE,
//  FOUND_GW,
//  CONNECTED
//} state;

int lightLevelPin = 0;
int lightLevel;

int temperaturePin = 1;
float temperature;

int controledLed = 6;
int sendPacketLed = 7;
int ackLed = 8;
int rxPacketLed = 9;
int gwLed = 10;
int connectLed = 11;
int registerLed = 12;
int errorLed = 13;

unsigned long LLPublishPeriod = 1000;
unsigned long LLPublishSampleTime;

unsigned long TempPublishPeriod = 2000;
unsigned long TempPublishSampleTime;

uint16_t pingRequestPeriod = 15000;
unsigned long pingRequestSampleTime;
unsigned long lastPingRequestSampleTime;
unsigned int requestPingCounter = 0;
unsigned long lastadvertiseSampleTime;

boolean gwKey, connectKey, registerT1, registerT2, registerCycle, subscribeCycle;

uint8_t gwId;
uint16_t gwAdd;
FLAGS_BITS flags;

byte buffer[256];
char clientId[23];
char topicBuffer[64];
char floatBuf[32];

uint8_t* bufData;
uint8_t bufLen;
uint8_t messageType;

unsigned int length;

char* topic1 = "light_level";
uint16_t topic1Id;

char* topic2 = "temperature";
uint16_t topic2Id;

char* topic3 = "led1";
uint16_t topic3Id;

char* willTopic = "will_topic";
char* willMsg = "will_msg";

uint8_t Radius = 102;
uint8_t ProtocolId = MQTTS_PROTOCOLID;
uint16_t MsgId;
uint16_t LastPublishedMsgID;
uint16_t lastRegisterMsgId;
uint16_t lastSubscribeMsgId;
FLAGS_BITS lastSubscribeFlags;

MQTTSCommand* mqttsCommand;
MQTTSPublish mqttsPublish;


void setup() {
  pinMode(controledLed, OUTPUT);
  pinMode(sendPacketLed, OUTPUT);
  pinMode(ackLed, OUTPUT);
  pinMode(rxPacketLed, OUTPUT);
  pinMode(gwLed, OUTPUT);
  pinMode(connectLed, OUTPUT);
  pinMode(registerLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  randomSeed(analogRead(2)); // must be unconnected analog pin
  init_proc(0);
  create_clientID();
  xbee.begin(9600);
}

void loop() {
  while(!gwKey) {
    search_gw(Radius); 
  }
  while(!connectKey && gwKey) {
    FLAGS_BITS flags;
    flags.bits.cleanSession = 1;  
    flags.bits.will = 0;      
    connect_gw(flags.flags, MQTTS_PROTOCOLID, pingRequestPeriod, clientId);
  }
  if(!registerT1 && !registerCycle) {
    registerCycle = true;
    sprintf(topicBuffer, "sensors/%s/%s", clientId, topic1);
    register_topic(0, MsgId, topicBuffer);
  }
  else if(registerT1 && !registerT2 && !registerCycle) {
    registerCycle = true;
    sprintf(topicBuffer, "sensors/%s/%s", clientId, topic2);
    register_topic(0, MsgId, topicBuffer);
  }
  else if(!subscribeCycle) {
    subscribeCycle = true;
    FLAGS_BITS flags;
    flags.bits.topicId = 0;
    flags.bits.qos = 0;
    flags.bits.dup = 0;
    sprintf(topicBuffer, "sensors/%s/%s", clientId, topic3);
    subscribe_topic(flags.flags, MsgId, topicBuffer); 
  }
  else if(millis() - LLPublishSampleTime > LLPublishPeriod){
    FLAGS_BITS flags;
    flags.bits.topicId = 0;	      //!< indicates whether the field TopicId or TopicName included in this message
    flags.bits.cleanSession = 0;  //!< same meaning as with MQTT, however extended for Will topic and Will message
    flags.bits.will = 0;	        //!< if set, indicates that client is asking for Will topic and Will message prompting
    flags.bits.retain = 0;	      //!< same meaning as with MQTT
    flags.bits.qos = 0;	          //!< set to “0b00” for QoS level 0, “0b01” for QoS level 1, “0b10” for QoS level 2, and “0b11” for new QoS level -1
    flags.bits.dup = 0;	          //!< set to “0” if message is sent for the first time; set to “1” if retransmitted
    LLPublishSampleTime = millis(); 
    lightLevel = analogRead(0);
    lightLevel = map(lightLevel, 0, 900, 0, 255);
    lightLevel = constrain(lightLevel, 0, 255);
    dtostrf(lightLevel, 7, 4, floatBuf);  // http://dereenigne.org/arduino/arduino-float-to-string
    publish(flags.flags, topic1Id, MsgId, floatBuf);    
  } 
  else if(millis() - TempPublishSampleTime > TempPublishPeriod){
    FLAGS_BITS flags;
    flags.bits.topicId = 0;
    flags.bits.cleanSession = 0;
    flags.bits.will = 0;
    flags.bits.retain = 0;
    flags.bits.qos = 0;
    flags.bits.dup = 0;
    TempPublishSampleTime = millis(); 
    temperature = (getVoltage(temperaturePin) - 0.5)*100;
    dtostrf(temperature, 7, 4, floatBuf);  // http://dereenigne.org/arduino/arduino-float-to-string
    publish(flags.flags, topic2Id, MsgId, floatBuf);    
  }  
  else if(millis() - pingRequestSampleTime > pingRequestPeriod){
    pingRequestSampleTime = millis(); 
    requestPingCounter++;
    if(requestPingCounter < NUM_OF_PINGS_TO_RECONNECT) {
      ping_request();  
    }
    else {
      init_proc(1);
    }      
  }
  if(xbee.readPacket(500)) {
    if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      handle_xbee_rx_response();
    }
  } 
  Serial.print("\n\nfree memory: "); Serial.print(freeMemory()); Serial.print("\n\n");
}

void handle_xbee_rx_response() {
  flashLed(rxPacketLed, 1, 100);
  decode_xbee_frame();
  switch(messageType) {
    case MQTTS_ADVERTISE:
      handle_advertise();
      break;
    case MQTTS_GWINFO:
      handle_gwinfo();
      break;
    case MQTTS_WILLTOPICREQ:
      handle_willTopicRequest();
      break;
    case MQTTS_WILLMSGREQ:
      handle_willMessageRequest();
      break;
    case MQTTS_REGISTER:
      handle_register();
      break;
    case MQTTS_REGACK:
      handle_regack();
      break;
    case MQTTS_PUBLISH:
      handle_publish();
      break;
    case MQTTS_PUBACK:
      handle_puback();
      break;
    case MQTTS_SUBACK:
      handle_suback();
      break;
    case MQTTS_UNSUBACK:
      handle_unsuback();
      break;
    case MQTTS_PINGREQ:
      handle_pingreq();
      break;
    case MQTTS_PINGRESP:
      handle_pingresp();
      break;
    case MQTTS_WILLTOPICRESP:
      break;
    case MQTTS_WILLMSGRESP:
      break;
    default:
      Serial.print("\nloop");
  } 
}

void handle_advertise() {
  //TODO 
}

void handle_gwinfo() {
  //TODO
}

void handle_willTopicRequest() {
  FLAGS_BITS flags;
  flags.bits.cleanSession = 0; //TODO - initialise
  flags.bits.will = 0;         //TODO - initialise 
  mqttsCommand = new MQTTSWillTopic(flags.flags, willTopic);
  send_message(mqttsCommand);
}

void handle_willMessageRequest() {
  mqttsCommand = new MQTTSWillMsg(willMsg);
  send_message(mqttsCommand);
}

void handle_register() {
  MQTTSRegister reg;
  reg.decode(bufData, bufLen);
  uint16_t regMsgID = reg.getMsgId();
  uint16_t regTopicID = reg.getTopicId();
  MQTTSCommand* mqttsCommand = new MQTTSRegAck(regTopicID, regMsgID, MQTTS_RC_ACCEPTED);
  send_message(mqttsCommand);
}

void handle_regack() {
  MQTTSRegAck regAck;
  regAck.decode(bufData, bufLen);
  if(regAck.getMsgId() == lastRegisterMsgId) {
    uint8_t returnCode = regAck.getReturnCode();
    switch(returnCode) {
      case MQTTS_RC_ACCEPTED:
        registerCycle = false;
        if(!registerT1) {
          topic1Id = regAck.getTopicId();
          registerT1 = true;
        }else {
          topic2Id = regAck.getTopicId();
          registerT2 = true;
          LLPublishSampleTime = millis();
          TempPublishSampleTime = millis();
          pingRequestSampleTime = millis();
          digitalWrite(registerLed, HIGH); 
        }
        break;
      case MQTTS_RC_REJECTED_CONGESTION:
        flashLed(errorLed, 2, 30);
        delay(random(10000));
        break;
      case MQTTS_RC_REJECTED_INVALID_TOPIC_ID:
        flashLed(errorLed, 3, 30);
        break;
      case MQTTS_RC_REJECTED_NOT_SUPPORTED:
        flashLed(errorLed, 4, 30);
        break;
      default:
        flashLed(errorLed, 5, 30);
        Serial.print("\n\nregister_topic return code, not valid value \n\n");
    }               
  } 
}

void handle_publish() {
  char* dataBuf;
  mqttsPublish.decode(bufData, bufLen);
  if(mqttsPublish.getTopicId() == topic3Id) {
    dataBuf = mqttsPublish.getData(); 
    Serial.println(dataBuf);
    if(strcmp (dataBuf, "on") == 0) {
      digitalWrite(controledLed, HIGH);
    } else if(strcmp (dataBuf, "off") == 0) {
        digitalWrite(controledLed, LOW);
    }    
  }
}

void handle_puback() {
  //TODO
}

void handle_suback() {
  MQTTSSubAck subAck;
  subAck.decode(bufData, bufLen);
  if((subAck.getMsgId() == lastSubscribeMsgId) && (lastSubscribeFlags.bits.topicId == 0)) {
    uint8_t returnCode = subAck.getReturnCode(); 
    switch(returnCode) {
      case MQTTS_RC_ACCEPTED:
        topic3Id = subAck.getTopicId();
        break;
      case MQTTS_RC_REJECTED_CONGESTION:
        flashLed(errorLed, 2, 30);
        subscribeCycle = false;
        delay(random(10000));
        //Serial.print("\n\npublish return code = MQTTS_RC_REJECTED_CONGESTION \n\n");
        break;
      case MQTTS_RC_REJECTED_INVALID_TOPIC_ID:
        flashLed(errorLed, 3, 30);
        subscribeCycle = false;
        //Serial.print("\n\npublish return code = MQTTS_RC_REJECTED_INVALID_TOPIC_ID \n\n");
        break;
      case MQTTS_RC_REJECTED_NOT_SUPPORTED:
        flashLed(errorLed, 4, 30);
        subscribeCycle = false;
        //Serial.print("\n\npublish return code = MQTTS_RC_REJECTED_NOT_SUPPORTED \n\n");
        break;
      default:
        flashLed(errorLed, 5, 30);
        subscribeCycle = false;
        Serial.print("\n\nsubscribe return code, not valid value \n\n");
    } 
  }
}

void handle_unsuback() {
  //TODO
}

void handle_pingreq() {
  mqttsCommand = new MQTTSPingResp();
  send_message(mqttsCommand);
}

void handle_pingresp() {
  requestPingCounter = 0;
}

void search_gw(uint8_t Radius) {
  delay(2000);
  MQTTSCommand* mqttsCommand = new MQTTSSearchGw(Radius);
  send_message(mqttsCommand);
  if(xbee.readPacket(500) && (xbee.getResponse().getApiId() == ZB_RX_RESPONSE)) {
    decode_xbee_frame();
    if(messageType == MQTTS_GWINFO) {
      flashLed(rxPacketLed, 1, 100);
      MQTTSGwInfo gwInfo;
      gwInfo.decode(bufData, bufLen); 
      gwId = gwInfo.getGwId();
      gwAdd = gwInfo.getGwAdd();
      gwKey = true;
      digitalWrite(gwLed, HIGH);
    } 
  }       
}

void connect_gw(uint8_t Flags, uint8_t ProtocolId, uint16_t pingRequestPeriod, char* ClientId) {
  delay(2000);
  MQTTSCommand* mqttsCommand = new MQTTSConnect(Flags, ProtocolId, pingRequestPeriod, ClientId);
  send_message(mqttsCommand);
  if(xbee.readPacket(1000) && (xbee.getResponse().getApiId() == ZB_RX_RESPONSE)) {
    decode_xbee_frame();
    if(messageType == MQTTS_CONNACK) {
      flashLed(rxPacketLed, 1, 100);
      MQTTSConAck conAck;
      conAck.decode(bufData, bufLen); 
      uint8_t returnCode = conAck.getReturnCode();
      switch(returnCode) {
        case MQTTS_RC_ACCEPTED:
          connectKey = true;
          digitalWrite(connectLed, HIGH);
          break;
        case MQTTS_RC_REJECTED_CONGESTION:
          flashLed(errorLed, 2, 30);
          break;
        case MQTTS_RC_REJECTED_INVALID_TOPIC_ID:
          flashLed(errorLed, 3, 30);
          break;
        case MQTTS_RC_REJECTED_NOT_SUPPORTED:
          flashLed(errorLed, 4, 30);
          break;
        default:
          flashLed(errorLed, 5, 30);
          Serial.print("\n\nconnect_gw return code, not valid value \n\n");
      }
    } else init_proc(1);
  } else init_proc(1);
}

void register_topic(uint16_t TopicId, uint16_t MsgId, const char* topic) {
  //delay(2000);
  MQTTSCommand* mqttsCommand = new MQTTSRegister(TopicId, MsgId, topic);
  lastRegisterMsgId = MsgId;
  send_message(mqttsCommand); 
}

void subscribe_topic(uint8_t Flags, uint16_t MsgId, char* topicName) {
  delay(2000);
  MQTTSCommand* mqttsCommand = new MQTTSSubscribe(Flags, MsgId, topicName);
  lastSubscribeMsgId = MsgId;
  lastSubscribeFlags.flags = Flags;  
  send_message(mqttsCommand);
 }


void unsubscribe_topic(uint8_t Flags, uint16_t MsgId, uint16_t TID) {
  MQTTSCommand* mqttsCommand = new MQTTSUnsubscribe(Flags, MsgId, TID); 
  send_message(mqttsCommand);
}

void publish(uint8_t Flags, uint16_t TopicId, uint16_t MsgId, char* Data) {
  MQTTSCommand* mqttsCommand = new MQTTSPublish(Flags, TopicId, MsgId, Data); // TODO
  LastPublishedMsgID = MsgId;
  send_message(mqttsCommand);
}

void ping_request() {
  MQTTSCommand* mqttsCommand = new MQTTSPingReq(); 
  send_message(mqttsCommand);
//  if (xbee.readPacket(500)) {
//    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
//      xbee.getResponse().getZBTxStatusResponse(txStatus);
//      if (txStatus.getDeliveryStatus() == SUCCESS) {
//        flashLed(ackLed, 1, 100);
//        if (xbee.readPacket(500)) {
//          if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
//            flashLed(rxPacketLed, 1, 100);
//            uint8_t* bufData = xbee.getResponse().getFrameData() + DATA_FRAME_OFFSET;
//            uint8_t bufLen = xbee.getResponse().getFrameDataLength() - DATA_FRAME_OFFSET;
//            uint8_t messageType = decode_header(bufData, bufLen);
//            if(messageType == MQTTS_PINGRESP) {
//              flashLed(ackLed, 5, 200);
//            } else init_proc(); 
//          } else init_proc();
//        } else init_proc();
//      } else init_proc();
//    } else init_proc();
//  } else init_proc();
}


void send_message(MQTTSCommand* mqttsCommand) {
  uint8_t messageType = mqttsCommand->getCommandType();
  mqttsCommand->encode(buffer, length);
  mqttsCommand->encodeHeader(messageType, buffer, length);
  zbTx = ZBTxRequest(gwAddr64, buffer, length);
  xbee.send(zbTx);
  flashLed(sendPacketLed, 1, 100);
  delete mqttsCommand;
  MsgId++;
  if (xbee.readPacket(500)) {
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        flashLed(ackLed, 1, 100);
      } else {
        flashLed(errorLed, 1, 100);
      }
    }
  }
}

uint8_t decode_header(uint8_t* buffer, uint8_t length) {
  return buffer[1];
}

void decode_xbee_frame() {
  bufData = xbee.getResponse().getFrameData() + DATA_FRAME_OFFSET;
  bufLen = xbee.getResponse().getFrameDataLength() - DATA_FRAME_OFFSET;
  messageType = decode_header(bufData, bufLen);
}

void create_clientID() {
  String clientString = "Client-";
  clientString += random(100000000);
  clientString.toCharArray(clientId, 23);
}

void init_proc(unsigned int type) {
  if(type == 0) {
    digitalWrite(gwLed, LOW);
    gwKey = false;
  } 
  digitalWrite(connectLed, LOW);
  digitalWrite(registerLed, LOW);
  connectKey = false; 
  registerT1 = false; 
  registerT2 = false;
  subscribeCycle = false; 
  registerCycle = false; 
  requestPingCounter = 0;
  MsgId = 0;
}

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

float getVoltage(int pin){
 return (analogRead(pin) * .004882814); 
}
