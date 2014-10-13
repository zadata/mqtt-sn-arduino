//David Chatting (david.chatting@network.rca.ac.uk)
//Royal College of Art - Design Interactions - http://www.design-interactions.rca.ac.uk/david-chatting
//12th March 2012
//---
//Example for use with the Sparkfun Nike+iPod Serial to USB Adapter - http://www.sparkfun.com/products/8245
//NB the board is powered by 3.3V supply

#include <public_nike_dec.h>
#include <SoftwareSerial.h>

SoftwareSerial nikePlusReceiver(8,9);  //RX=8, TX=9

void setup() {
  Serial.begin(57600);
  initNikePlusReceiver();
}

void loop() {
  if (nikePlusReceiver.available()){
    readNikePlusPacket();
  }
}

void initNikePlusReceiver() {
    nikePlusReceiver.begin(57600);

    byte a[] = { 0xFF, 0x55, 0x04, 0x09, 0x07, 0x00, 0x25, 0xC7 };
    byte b[8];  //FF 55 04 09 00 00 07 EC
    byte c[] = { 0xFF, 0x55, 0x02, 0x09, 0x05, 0xF0 };
    byte d[8];  //FF 55 04 09 06 00 25 C8
  
    int i=0;
    Serial.println(sizeof(a));
    for(i = 0 ; i < sizeof(a) ; i++) nikePlusReceiver.write(a[i]);
    for(i = 0 ; i < sizeof(b) ; i++) {
      while(nikePlusReceiver.available()==0);
      b[i]=nikePlusReceiver.read();
    }
    for(i = 0 ; i < sizeof(c) ; i++) nikePlusReceiver.write(c[i]);
    for(i = 0 ; i < sizeof(d) ; i++) {
      while(nikePlusReceiver.available()==0);
      d[i]=nikePlusReceiver.read();
    }
  }
  
  void readNikePlusPacket() {
    if(nikePlusReceiver.available()>0) {
      byte header[5];
      for(int i=0;i<sizeof(header);i++) {
        while(nikePlusReceiver.available()==0);
        header[i]=nikePlusReceiver.read();
      }
      byte data[29];
      for(int i=0;i<sizeof(data);i++) {
        while(nikePlusReceiver.available()==0);
        data[i]=nikePlusReceiver.read();
      }
      
      descramblePayload(data);
      uint8_t* payload=getPayloadPtr(data);
      
      char sn[12];
      calcSN(getSrcAddr(data), data, sn);
      Serial.print("Serial number: ");
      Serial.println(sn);
      
      float walkingMiles=(64*getLifetimeWalkingMiles(payload))/15319.148451089859f;
      Serial.print("Lifetime Walking Miles: ");
      Serial.println(walkingMiles,3);
      
      float runningMiles=(64*getLifetimeRunningMiles(payload))/18947.368115186691f;
      Serial.print("Lifetime Running Miles: ");
      Serial.println(runningMiles,3);
    }
  }
