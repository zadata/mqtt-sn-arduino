////////////////////////////////////////////////////////////////////////////////
/// Arduino sketch for MQTT-SN over XBee
///
/// \author    Zvi Avraham
/// \copyright 2012-2014 ZADATA Ltd. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////

#include <XBee.h>
#include <mqtts.h>
#include <MemoryFree.h>
#include <SoftwareSerial.h>

// create the XBee object
XBee xbee = XBee();

// SH + SL Address of receiving XBee from the router on the back of the xbee two lower lines
//XBeeAddress64 gwAddr64 = XBeeAddress64(0x0013a200,0x408698F1); 
XBeeAddress64 activeGwAddr64; 
//XBeeAddress64 broadcastAdd64 = XBeeAddress64(0x00000000,0x0000FFFF); 
ZBTxRequest zbTx;
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

//TODO use FSM
enum eState{
  IDLE,
  DISCONNECTED,
  CONNECTED
} state;

int lightLevelPin   = 0;
int lightLevel;

int temperaturePin  = 1;
float temperature;

int controledLed    = 6;
int sendPacketLed   = 7;
int ackLed          = 8;
int rxPacketLed     = 9;
int gwLed           = 10;
int connectLed      = 11;
int registerLed     = 12;
int errorLed        = 13;

unsigned long searchGWDelay;
unsigned long searchGWSampleTime;

unsigned long lastConnectSampleTime = 0;
unsigned long randomConnectDelay;

unsigned long LLPublishPeriod = 3000;
unsigned long LLPublishSampleTime;

unsigned long TempPublishPeriod = 2000;
unsigned long TempPublishSampleTime;

unsigned long searchGwIncomingTime;
unsigned long randomGwInfoDelay;

uint16_t pingRequestPeriod = 15000;
unsigned long pingRequestSampleTime;
unsigned long lastPingRequestSampleTime;
unsigned int requestPingCounter = 0;
unsigned long lastadvertiseSampleTime;

uint16_t tAdvertise;
unsigned long lastAdvertiseTime;
unsigned int missedadvertise;

boolean registerT1, registerT2, subscribeCycle, allSubscribed, 
        delayedGwInfo, advertiseKey, allRegistered, registerCycle;
        
uint8_t activeGwID;
uint16_t activeGw16BitAdd = 0xfffe;
FLAGS_BITS flags;

byte buffer[256];
char clientId[23];
char topicBuffer[64];
char floatBuf[32];

uint8_t* bufData;
uint8_t bufLen;
uint8_t messageType;

unsigned int length;

char* client_id_prefix = "cl-";

char* topic1 = "light_level";
uint16_t topic1Id;

char* topic2 = "temperature";
uint16_t topic2Id;

char* topic3 = "led1";
uint16_t topic3Id;

char* willTopic = "will_topic";
char* willMsg = "will_msg";

uint8_t Radius = 10;
uint8_t ProtocolId = MQTTS_PROTOCOLID;
uint16_t MsgId;
uint16_t LastPublishedMsgID;
uint16_t lastRegisterMsgId;
uint16_t lastSubscribeMsgId;
FLAGS_BITS lastSubscribeFlags;

MQTTSCommand* mqttsCommand;
MQTTSPublish mqttsPublish;

SoftwareSerial XBeeSerial(2,3);  //RX=2, TX=3

AtCommandRequest atRequest;
AtCommandResponse atResponse = AtCommandResponse();

void setup() {
  pinMode(controledLed,   OUTPUT);
  pinMode(sendPacketLed,  OUTPUT);
  pinMode(ackLed,         OUTPUT);
  pinMode(rxPacketLed,    OUTPUT);
  pinMode(gwLed,          OUTPUT);
  pinMode(connectLed,     OUTPUT);
  pinMode(registerLed,    OUTPUT);
  pinMode(errorLed,       OUTPUT);
  randomSeed(analogRead(2)); // must be unconnected analog pin
  searchGWDelay = random(T_SEARCHGW);
  searchGWSampleTime = millis();
  state = IDLE;
  Serial.begin(9600);
  XBeeSerial.begin(9600);
  xbee.begin(XBeeSerial);
  create_clientID();
}

void loop() {
  switch(state) {
    case IDLE:
      if(millis() - searchGWSampleTime > searchGWDelay) {
        searchGWSampleTime = millis();
        searchGWDelay = random(T_SEARCHGW);
        search_gw(Radius);
      }
      break;
    case DISCONNECTED:
      if(millis() - lastAdvertiseTime > tAdvertise && advertiseKey) {
        Serial.println("s-D at");
        lastAdvertiseTime = millis();
        missedadvertise++;
        if(missedadvertise > N_ADVERTISE) {
          Serial.print("cr s-D ma-"); Serial.println(missedadvertise);
          state = IDLE;
          digitalWrite(gwLed, LOW);
          digitalWrite(connectLed, LOW);
          digitalWrite(registerLed, LOW);
        } 
      }
      else if((millis() - searchGwIncomingTime > randomGwInfoDelay) && delayedGwInfo) {
        delayedGwInfo = false;
        mqttsCommand = new MQTTSGwInfo(activeGwID, activeGwAddr64.getLsb(), activeGwAddr64.getMsb()); 
        send_message(mqttsCommand, SEND_BROADCAST);
      }
      else if(millis() - lastConnectSampleTime > randomConnectDelay){
        FLAGS_BITS flags;
        flags.bits.cleanSession = 1;  
        flags.bits.will = 0; 
        lastConnectSampleTime = millis();     
        connect_gw(flags.flags, MQTTS_PROTOCOLID, pingRequestPeriod, clientId);
      }
      break;
    case CONNECTED:
      if(!subscribeCycle & !allSubscribed) { 
        subscribeCycle = true;
        allSubscribed = true; // must be on last subscribe
        FLAGS_BITS flags;
        flags.bits.topicId = 0;
        flags.bits.qos = 0;
        flags.bits.dup = 0;
        sprintf(topicBuffer, "sensors/%s/%s", clientId, topic3);
        Serial.println("s-C st"); 
        subscribe_topic(flags.flags, MsgId, topicBuffer); 
      }
      else if(!registerCycle && !allRegistered) {
        registerCycle = true;
        if(!registerT1) {
          Serial.println("s-C rt1");
          sprintf(topicBuffer, "sensors/%s/%s", clientId, topic1);
        }
        else if(!registerT2) {
          Serial.println("s-C rt2");
          sprintf(topicBuffer, "sensors/%s/%s", clientId, topic2);
          allRegistered = true;
        }
        register_topic(0, MsgId, topicBuffer);
      }
      else if(millis() - LLPublishSampleTime > LLPublishPeriod && registerT1) {
        FLAGS_BITS flags;
        flags.bits.topicId = 0;	      //!< indicates whether the field TopicId or TopicName included in this message
        flags.bits.retain  = 0;	      //!< same meaning as with MQTT
        flags.bits.qos     = 0;	      //!< set to “0b00” for QoS level 0, “0b01” for QoS level 1, “0b10” for QoS level 2, and “0b11” for new QoS level -1
        flags.bits.dup     = 0;	      //!< set to “0” if message is sent for the first time; set to “1” if retransmitted
        LLPublishSampleTime = millis(); 
        lightLevel = analogRead(lightLevelPin);
        lightLevel = map(lightLevel, 0, 900, 0, 255);
        lightLevel = constrain(lightLevel, 0, 255);
        dtostrf(lightLevel, 7, 4, floatBuf);  // http://dereenigne.org/arduino/arduino-float-to-string
        publish(flags.flags, topic1Id, MsgId, floatBuf);    
      } 
      else if(millis() - TempPublishSampleTime > TempPublishPeriod && registerT2) {
        FLAGS_BITS flags;
        flags.bits.topicId = 0;
        flags.bits.retain  = 0;
        flags.bits.qos     = 0;
        flags.bits.dup     = 0;
        TempPublishSampleTime = millis(); 
        temperature = (getVoltage(temperaturePin) - 0.5)*100;
        dtostrf(temperature, 7, 4, floatBuf);  // http://dereenigne.org/arduino/arduino-float-to-string
        publish(flags.flags, topic2Id, MsgId, floatBuf);    
      }  
      else if(millis() - pingRequestSampleTime > pingRequestPeriod){
        Serial.println("s-C pr");
        pingRequestSampleTime = millis();
        ping_request(); 
        requestPingCounter++;
        if(requestPingCounter > NUM_OF_PINGS_TO_RECONNECT) {
          Serial.print("\ncr s-C rpc-"); Serial.println(requestPingCounter);
          if(!advertiseKey) {
            state = IDLE;
            digitalWrite(gwLed, LOW);
          } 
          else 
            state = DISCONNECTED; 
          digitalWrite(connectLed, LOW);
          digitalWrite(registerLed, LOW);   
        }      
      }
      else if(millis() - lastAdvertiseTime > tAdvertise && advertiseKey) {
        Serial.println("s-C at");
        lastAdvertiseTime = millis();
        missedadvertise++;
        if(missedadvertise > N_ADVERTISE) {
          Serial.print("cr s-C ma-"); Serial.println(missedadvertise);
          state = IDLE;
          digitalWrite(gwLed, LOW);
          digitalWrite(connectLed, LOW);
          digitalWrite(registerLed, LOW);
        } 
      }
      else if((millis() - searchGwIncomingTime > randomGwInfoDelay) && delayedGwInfo) {
        delayedGwInfo = false;
        mqttsCommand = new MQTTSGwInfo(activeGwID, activeGwAddr64.getLsb(), activeGwAddr64.getMsb()); 
        send_message(mqttsCommand, SEND_BROADCAST);
      }
      break; 
    default: 
      break;     
  }
  if(xbee.readPacket(300)) {
    if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      handle_xbee_rx_response();
    }
    Serial.print("free memory: "); Serial.println(freeMemory());
  }
}


void handle_xbee_rx_response() {
  flashLed(rxPacketLed, 1, 100);
  decode_xbee_frame();
  switch(messageType) {
    case MQTTS_ADVERTISE:
      handle_advertise();
      break;
    case MQTTS_SEARCHGW:
      handle_searchgw();
      break;
    case MQTTS_GWINFO:
      handle_gwinfo();
      break;
    case MQTTS_CONNACK:
      handle_conack();
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
  if(state != IDLE) {
    MQTTSAdvertise advertise;
    advertise.decode(bufData, bufLen);
    advertiseKey = true;
    missedadvertise = 0;
    lastAdvertiseTime = millis(); 
    tAdvertise = advertise.getDuration()*1000;
  }
  //TODO 
}

void handle_searchgw() {
  if(state == IDLE) {
    searchGWSampleTime = millis();
  } 
  else if(state != IDLE) {
    searchGwIncomingTime = millis();
    delayedGwInfo = true;
    randomGwInfoDelay = random(T_SEARCHGW);
  }
}

void handle_gwinfo() {
  if(state == IDLE) {
    state = DISCONNECTED;
    MQTTSGwInfo gwInfo;
    gwInfo.decode(bufData, bufLen); 
    activeGwID = gwInfo.getGwId();
    activeGwAddr64 = XBeeAddress64(gwInfo.getGwSlAdd(), gwInfo.getGwShAdd());
    randomConnectDelay = random(T_SEARCHGW);
    delayedGwInfo = false;
    advertiseKey = false;
    //registerT1 = false;
    //registerT2 = false;
    missedadvertise = 0;
    digitalWrite(gwLed, HIGH); 
  }
  //TODO
}

void handle_conack() {
  if(state == DISCONNECTED) {
    MQTTSConAck conack;
    conack.decode(bufData, bufLen);
    if(conack.getReturnCode() == MQTTS_RC_ACCEPTED) {
      state = CONNECTED;
      digitalWrite(connectLed, HIGH);
      pingRequestSampleTime = millis();
      requestPingCounter = 0;
      subscribeCycle = false;
      allSubscribed = false; 
      registerCycle = false;
      allRegistered = false;
      registerT1 = false; 
      registerT2 = false;
      MsgId = 0;
    }
  }
}

void handle_willTopicRequest() {
  FLAGS_BITS flags;
  flags.bits.cleanSession = 0; //TODO - initialise
  flags.bits.will         = 0; //TODO - initialise 
  mqttsCommand = new MQTTSWillTopic(flags.flags, willTopic);
  send_message(mqttsCommand, SEND_NORMAL);
}

void handle_willMessageRequest() {
  mqttsCommand = new MQTTSWillMsg(willMsg);
  send_message(mqttsCommand, SEND_NORMAL);
}

void handle_register() {
  MQTTSRegister reg;
  reg.decode(bufData, bufLen);
  uint16_t regMsgID = reg.getMsgId();
  uint16_t regTopicID = reg.getTopicId();
  MQTTSCommand* mqttsCommand = new MQTTSRegAck(regTopicID, regMsgID, MQTTS_RC_ACCEPTED);
  send_message(mqttsCommand, SEND_NORMAL);
}

void handle_regack() {
  MQTTSRegAck regAck;
  regAck.decode(bufData, bufLen);
  registerCycle = false;
  if(regAck.getMsgId() == lastRegisterMsgId) {
    uint8_t returnCode = regAck.getReturnCode();
    switch(returnCode) {
      case MQTTS_RC_ACCEPTED:
        if(!registerT1) {
          topic1Id = regAck.getTopicId();
          registerT1 = true;
        }
        else {
          topic2Id = regAck.getTopicId();
          registerT2 = true;
          LLPublishSampleTime = millis();
          TempPublishSampleTime = millis();
          pingRequestSampleTime = millis();
          digitalWrite(registerLed, HIGH); 
        }
        break;
      case MQTTS_RC_REJECTED_CONGESTION:
        allRegistered = false;
        flashLed(errorLed, 2, 30);
        delay(random(10000));
        break;
      case MQTTS_RC_REJECTED_INVALID_TOPIC_ID:
        allRegistered = false;
        flashLed(errorLed, 3, 30);
        break;
      case MQTTS_RC_REJECTED_NOT_SUPPORTED:
        allRegistered = false;
        flashLed(errorLed, 4, 30);
        break;
      default:
        allRegistered = false;
        flashLed(errorLed, 5, 30);
    }               
  } 
}

void handle_publish() {
  char* dataBuf;
  mqttsPublish.decode(bufData, bufLen);
  if(mqttsPublish.getTopicId() == topic3Id) {
    dataBuf = mqttsPublish.getData(); 
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
  subscribeCycle = false;
  subAck.decode(bufData, bufLen);
  if((subAck.getMsgId() == lastSubscribeMsgId) && (lastSubscribeFlags.bits.topicId == 0)) {
    uint8_t returnCode = subAck.getReturnCode(); 
    switch(returnCode) {
      case MQTTS_RC_ACCEPTED:
        topic3Id = subAck.getTopicId();
        break;
      case MQTTS_RC_REJECTED_CONGESTION:
        flashLed(errorLed, 2, 30);
        allSubscribed = false;
        delay(random(10000));
        break;
      case MQTTS_RC_REJECTED_INVALID_TOPIC_ID:
        flashLed(errorLed, 3, 30);
        allSubscribed = false;
        break;
      case MQTTS_RC_REJECTED_NOT_SUPPORTED:
        flashLed(errorLed, 4, 30);
        allSubscribed = false;
        break;
      default:
        flashLed(errorLed, 5, 30);
        allSubscribed = false;
    } 
  }
}

void handle_unsuback() {
  //TODO
}

void handle_pingreq() {
  mqttsCommand = new MQTTSPingResp();
  send_message(mqttsCommand, SEND_NORMAL);
}

void handle_pingresp() {
  requestPingCounter = 0;
}

void search_gw(uint8_t Radius) {
  MQTTSCommand* mqttsCommand = new MQTTSSearchGw(Radius);
  send_message(mqttsCommand, SEND_BROADCAST);     
}

void connect_gw(uint8_t Flags, uint8_t ProtocolId, uint16_t pingRequestPeriod, char* ClientId) {
  MQTTSCommand* mqttsCommand = new MQTTSConnect(Flags, ProtocolId, pingRequestPeriod, ClientId);
  send_message(mqttsCommand, SEND_NORMAL);
}

void register_topic(uint16_t TopicId, uint16_t MsgId, const char* topic) {
  MQTTSCommand* mqttsCommand = new MQTTSRegister(TopicId, MsgId, topic);
  lastRegisterMsgId = MsgId;
  send_message(mqttsCommand, SEND_NORMAL); 
}

void subscribe_topic(uint8_t Flags, uint16_t MsgId, char* topicName) {
  MQTTSCommand* mqttsCommand = new MQTTSSubscribe(Flags, MsgId, topicName);
  lastSubscribeMsgId = MsgId;
  lastSubscribeFlags.flags = Flags;  
  send_message(mqttsCommand, SEND_NORMAL);
 }

void unsubscribe_topic(uint8_t Flags, uint16_t MsgId, uint16_t TID) {
  MQTTSCommand* mqttsCommand = new MQTTSUnsubscribe(Flags, MsgId, TID); 
  send_message(mqttsCommand, SEND_NORMAL);
}

void publish(uint8_t Flags, uint16_t TopicId, uint16_t MsgId, char* Data) {
  MQTTSCommand* mqttsCommand = new MQTTSPublish(Flags, TopicId, MsgId, Data); // TODO
  LastPublishedMsgID = MsgId;
  send_message(mqttsCommand, SEND_NORMAL);
}

void ping_request() {
  MQTTSCommand* mqttsCommand = new MQTTSPingReq(); 
  send_message(mqttsCommand, SEND_NORMAL);
}

void send_message(MQTTSCommand* mqttsCommand, int sendType) {
  uint8_t messageType = mqttsCommand->getCommandType();
  mqttsCommand->encode(buffer, length);
  mqttsCommand->encodeHeader(messageType, buffer, length);
  if(sendType == SEND_BROADCAST) {
    XBeeAddress64 broadcastAdd64 = XBeeAddress64(0x00000000,0x0000FFFF); 
    zbTx = ZBTxRequest(broadcastAdd64, buffer, length); 
    xbee.send(zbTx);
    flashLed(sendPacketLed, 1, 100); 
  }   
  else {
    zbTx = ZBTxRequest(activeGwAddr64, activeGw16BitAdd, ZB_BROADCAST_RADIUS_MAX_HOPS, ZB_TX_UNICAST, buffer, length, DEFAULT_FRAME_ID);
    //zbTx = ZBTxRequest(activeGwAddr64, buffer, length);
    xbee.send(zbTx);
    flashLed(sendPacketLed, 1, 10);
    if (xbee.readPacket(500)) {
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if(txStatus.getDeliveryStatus() == SUCCESS) {
          activeGw16BitAdd = txStatus.getRemoteAddress();
          //Serial.println(activeGw16BitAdd, HEX);
          flashLed(ackLed, 1, 100);
        } 
        else {
          flashLed(errorLed, 1, 100);
        }
      } 
    } 
  }
  delete mqttsCommand;
  MsgId++;
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
  boolean clientID_created = false;
  uint8_t shCmd[] = {'S','H'};
  uint8_t slCmd[] = {'S','L'};  
  String clientString;
  do
  {
    clientString = client_id_prefix;
    atRequest = AtCommandRequest(shCmd);
    if(sendAtCommand()) {
      for (int i = 0; i < atResponse.getValueLength(); i++) {
        if(atResponse.getValue()[i]<10)
          clientString += 0;
        clientString += String(atResponse.getValue()[i], HEX);
      }
      atRequest = AtCommandRequest(slCmd);
      if(sendAtCommand()) {
        for (int i = 0; i < atResponse.getValueLength(); i++) {
          if(atResponse.getValue()[i]<10)
            clientString += 0;
          clientString += String(atResponse.getValue()[i], HEX);
          clientID_created = true;
        }
      }
    }
  } while(!clientID_created);
  
  clientString.toCharArray(clientId, 23);
  Serial.println(clientId);
}


boolean sendAtCommand() {
  xbee.send(atRequest);
  if (xbee.readPacket(5000)) {
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(atResponse);
      if (atResponse.isOk()) {
        if (atResponse.getValueLength() > 0) {
          return true;
        }
      } 
    }   
  } 
  flashLed(errorLed, 1, 100);
  return false;
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
