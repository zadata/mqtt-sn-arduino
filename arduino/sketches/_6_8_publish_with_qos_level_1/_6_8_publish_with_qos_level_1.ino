////////////////////////////////////////////////////////////////////////////////
/// Simple Publish with QoS -1 Arduino sketch
///
/// \author    Zvi Avraham
/// \copyright 2012-2014 ZADATA Ltd. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////

#include <XBee.h>
#include <mqtts_simple.h>
#include <MemoryFree.h>
#include <SoftwareSerial.h>

// offset of data in response
#define DATA_FRAME_OFFSET 11 

// create the XBee object
XBee xbee = XBee();

// SH + SL Address of receiving XBee from the router on the back of the xbee two lower lines
XBeeAddress64 gwAddr64 = XBeeAddress64(0x0013a200,0x408698F1); 
ZBTxRequest zbTx;
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int lightLevelPin   = 0;
int temperaturePin  = 1;

int sendPacketLed   = 7;
int ackLed          = 8;
int rxPacketLed     = 9;

unsigned long LLPublishPeriod = 2000;
unsigned long LLPublishSampleTime;

unsigned long TempPublishPeriod = 3000;
unsigned long TempPublishSampleTime;

byte buffer[256];
char floatBuf[32];

unsigned int length;

uint16_t MsgId = 0;
uint16_t predefinedTopicID = 1; // predefined topic id - /a/b/c
uint16_t shortTopicID = 0x2F41; // short topic /A

SoftwareSerial XBeeSerial(2,3);  //RX=2, TX=3

void setup() {
  pinMode(sendPacketLed, OUTPUT);
  pinMode(ackLed, OUTPUT);
  pinMode(rxPacketLed, OUTPUT);
  LLPublishSampleTime = millis();
  TempPublishSampleTime = millis();
  Serial.begin(9600);
  XBeeSerial.begin(9600);
  xbee.begin(XBeeSerial);
}

void loop() {
  if(millis() - LLPublishSampleTime > LLPublishPeriod){
    LLPublishSampleTime = millis();
    light_level_publish();
  } 
  if(millis() - TempPublishSampleTime > TempPublishPeriod){
    TempPublishSampleTime = millis();
    temperature_publish(); 
  }  
  //Serial.print("free memory: "); Serial.println(freeMemory());
}

void light_level_publish() {
  int lightLevel;
  FLAGS_BITS flags;
  flags.bits.topicId  = 1;  //!< indicates whether the field TopicId or TopicName included in this message
  flags.bits.retain   = 0;  //!< same meaning as with MQTT
  flags.bits.qos      = 3;  //!< set to “0b00” for QoS level 0, “0b01” for QoS level 1, “0b10” for QoS level 2, and “0b11” for new QoS level -1
  flags.bits.dup      = 0;  //!< set to “0” if message is sent for the first time; set to “1” if retransmitted
  lightLevel = analogRead(0);
  dtostrf(lightLevel, 8, 4, floatBuf);  // http://dereenigne.org/arduino/arduino-float-to-string
  publish(flags.flags, predefinedTopicID, MsgId, floatBuf);    
}

void temperature_publish() {
  float temperature;
  FLAGS_BITS flags;
  flags.bits.topicId  = 2;  //!< indicates whether the field TopicId or TopicName included in this message
  flags.bits.retain   = 0;  //!< same meaning as with MQTT
  flags.bits.qos      = 3;  //!< set to “0b00” for QoS level 0, “0b01” for QoS level 1, “0b10” for QoS level 2, and “0b11” for new QoS level -1
  flags.bits.dup      = 0;  //!< set to “0” if message is sent for the first time; set to “1” if retransmitted
  temperature = (getVoltage(temperaturePin) - 0.5)*100;
  dtostrf(temperature, 8, 4, floatBuf);  // http://dereenigne.org/arduino/arduino-float-to-string
  publish(flags.flags, shortTopicID, MsgId, floatBuf);      
}

void publish(uint8_t Flags, uint16_t TopicId, uint16_t MsgId, char* Data) {
  MQTTSCommand* mqttsCommand = new MQTTSPublish(Flags, TopicId, MsgId, Data); 
  send_message(mqttsCommand);
}

void send_message(MQTTSCommand* mqttsCommand) {
  uint8_t messageType = mqttsCommand->getCommandType();
  mqttsCommand->encode(buffer, length);
  mqttsCommand->encodeHeader(messageType, buffer, length);
  zbTx = ZBTxRequest(gwAddr64, buffer, length);
  xbee.send(zbTx);
  flashLed(sendPacketLed, 1, 30);
  delete mqttsCommand;
  MsgId++;
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
