////////////////////////////////////////////////////////////////////////////////
/// Arduino sketch for MQTT-SN over XBee
///
/// \author    Zvi Avraham
/// \copyright 2012-2014 ZADATA Ltd. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////

#include <XBee.h>
#include <mqttsn_simple.h>
#include <MemoryFree.h>
#include <SoftwareSerial.h>

// create the XBee object
XBee xbee = XBee();

// SH + SL Address of receiving XBee from the router on the back of the xbee two lower lines
XBeeAddress64 activeGwAddr64; 
ZBTxRequest zbTx;
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

//TODO use FSM
enum eState{
  IDLE,
  FOUNDGW
} state;

int lightLevelPin   = 0;
int lightLevel;

int temperaturePin  = 1;
float temperature;

unsigned long searchGWDelay;
unsigned long searchGWSampleTime;

unsigned long LLPublishPeriod = 3000;
unsigned long LLPublishSampleTime;

unsigned long TempPublishPeriod = 2000;
unsigned long TempPublishSampleTime;

unsigned long lastadvertiseSampleTime;
uint16_t tAdvertise;
unsigned long lastAdvertiseTime;
unsigned int missedadvertise;

boolean advertiseKey;
        
uint8_t activeGwID;
uint16_t activeGw16BitAdd = 0xfffe;
FLAGS_BITS flags;

byte buffer[256];
char floatBuf[32];

uint8_t* bufData;
uint8_t bufLen;
uint8_t messageType;

unsigned int length;
uint16_t predefinedTopicID = 1; // predefined topic id - /a/b/c
uint16_t shortTopicID = 0x2F41; // short topic /A

uint8_t Radius = 10;
uint16_t MsgId;

MQTTSCommand* mqttsCommand;
MQTTSPublish mqttsPublish;

SoftwareSerial XBeeSerial(2,3);  //RX=2, TX=3

void setup() {
  randomSeed(analogRead(2)); // must be unconnected analog pin
  searchGWDelay = random(T_SEARCHGW);
  searchGWSampleTime = millis();
  state = IDLE;
  Serial.begin(9600);
  XBeeSerial.begin(9600);
  xbee.begin(XBeeSerial);
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
    case FOUNDGW:
      if(millis() - LLPublishSampleTime > LLPublishPeriod) {
        FLAGS_BITS flags;
        flags.bits.topicId  = 1;  //!< indicates whether the field TopicId or TopicName included in this message
        flags.bits.retain   = 0;  //!< same meaning as with MQTT
        flags.bits.qos      = 3;  //!< set to “0b00” for QoS level 0, “0b01” for QoS level 1, “0b10” for QoS level 2, and “0b11” for new QoS level -1
        flags.bits.dup      = 0;  //!< set to “0” if message is sent for the first time; set to “1” if retransmitted
        LLPublishSampleTime = millis(); 
        lightLevel = analogRead(lightLevelPin);
        lightLevel = map(lightLevel, 0, 900, 0, 255);
        lightLevel = constrain(lightLevel, 0, 255);
        dtostrf(lightLevel, 7, 4, floatBuf);  // http://dereenigne.org/arduino/arduino-float-to-string
        publish(flags.flags, predefinedTopicID, MsgId, floatBuf);    
      } 
      else if(millis() - TempPublishSampleTime > TempPublishPeriod) {
        FLAGS_BITS flags;
        flags.bits.topicId  = 2;  //!< indicates whether the field TopicId or TopicName included in this message
        flags.bits.retain   = 0;  //!< same meaning as with MQTT
        flags.bits.qos      = 3;  //!< set to “0b00” for QoS level 0, “0b01” for QoS level 1, “0b10” for QoS level 2, and “0b11” for new QoS level -1
        flags.bits.dup      = 0;  //!< set to “0” if message is sent for the first time; set to “1” if retransmitted
        TempPublishSampleTime = millis(); 
        temperature = (getVoltage(temperaturePin) - 0.5)*100;
        dtostrf(temperature, 7, 4, floatBuf);  // http://dereenigne.org/arduino/arduino-float-to-string
        publish(flags.flags, shortTopicID, MsgId, floatBuf);    
      }  
      else if(millis() - lastAdvertiseTime > tAdvertise && advertiseKey) {
        lastAdvertiseTime = millis();
        missedadvertise++;
        if(missedadvertise > N_ADVERTISE) {
          state = IDLE;
        } 
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
  decode_xbee_frame();
  switch(messageType) {
    case MQTTS_ADVERTISE:
      handle_advertise();
      break;
    case MQTTS_GWINFO:
      handle_gwinfo();
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
}

void handle_gwinfo() {
  if(state == IDLE) {
    state = FOUNDGW;
    MQTTSGwInfo gwInfo;
    gwInfo.decode(bufData, bufLen); 
    activeGwID = gwInfo.getGwId();
    activeGwAddr64 = XBeeAddress64(gwInfo.getGwSlAdd(), gwInfo.getGwShAdd());
    advertiseKey = false;
    missedadvertise = 0;
  }
}

void search_gw(uint8_t Radius) {
  MQTTSCommand* mqttsCommand = new MQTTSSearchGw(Radius);
  send_message(mqttsCommand, SEND_BROADCAST);     
}

void publish(uint8_t Flags, uint16_t TopicId, uint16_t MsgId, char* Data) {
  MQTTSCommand* mqttsCommand = new MQTTSPublish(Flags, TopicId, MsgId, Data); 
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
  }   
  else {
    zbTx = ZBTxRequest(activeGwAddr64, activeGw16BitAdd, ZB_BROADCAST_RADIUS_MAX_HOPS, ZB_TX_UNICAST, buffer, length, DEFAULT_FRAME_ID);
    //zbTx = ZBTxRequest(activeGwAddr64, buffer, length);
    xbee.send(zbTx);
    if (xbee.readPacket(500)) {
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if(txStatus.getDeliveryStatus() == SUCCESS) {
          activeGw16BitAdd = txStatus.getRemoteAddress();
          Serial.println(activeGw16BitAdd, HEX);
        } 
        else {
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

float getVoltage(int pin){
 return (analogRead(pin) * .004882814); 
}
