/*

	This code is released under the following license:
		- You may use the code for personal (non-commercial) projects free of charge on conditions that: (1) credit is given in any write-ups of the project, and (2) your code changes (if any) are made public at the same time as the project is.
		- You must contact Dmitry Grinberg (dmitrygr@gmail.com) if you would like to use this code in a commercial project.
		- If you do not agree to these terms, you may not use this code.

	A lot of hard work went into figuring all this out, so please respect the above.
*/

// ensure this library description is only included once
#ifndef public_nike_dec_h
#define public_nike_dec_h

#include "Arduino.h"

const char serialNumChars[] = "0123456789ABCDEFGHJKLMNPQRSTUVWXYZ";
const uint8_t randomTable1[] = {1, 4, 15, 12, 8, 2, 3, 6, 14, 0, 5, 7, 13, 10, 9, 11};
const uint8_t randomTable2[] = {0x67, 0x6, 0x51, 0xb8, 0x4a, 0xca, 0x35, 0x3e, 0x72, 0x85, 0x12, 0xd4, 0x8e, 0x9d, 0x1c, 0x23};

uint8_t getSrcFlags(const uint8_t* packet);
uint8_t getDstFlags(const uint8_t* packet);
uint8_t getSrcTypeLen(const uint8_t* packet);
uint8_t getSrcAddrLen(const uint8_t* packet);
uint8_t getDstAddrLen(const uint8_t* packet);
uint8_t getDstTypeLen(const uint8_t* packet);
uint8_t* getPayloadPtr(uint8_t* packet);
uint32_t getAddr(const uint8_t* ptr, uint8_t sz);
uint32_t  getSrcAddr(const uint8_t* packet);
uint32_t getDstAddr(const uint8_t* packet);
uint16_t getType(const uint8_t* ptr, uint8_t len);
uint16_t getSrcType(const uint8_t* packet);
uint16_t getDstType(const uint8_t* packet);
uint8_t getPayloadLen(uint8_t* packet);
uint8_t getTimingByte(const uint8_t* packet);
uint8_t getTimingBytePosition(const uint8_t* packet);
void setTimingByte(uint8_t* packet,uint8_t timingByte,bool init);
void descramblePayload(uint8_t* packet);
void scramblePayload(uint8_t* packet);
void calcSN(uint32_t  sn, uint8_t* decodedPacket, char* dst);
void setSN(uint8_t* payload, char* sn);
uint16_t getOnHours(uint8_t* payload);
void setOnHours(uint8_t* payload, uint16_t hours);
uint32_t  getRunningStepCount(uint8_t* payload);
void setRunningStepCount(uint8_t* payload,uint32_t steps);
uint32_t  getWalkingStepCount(uint8_t* payload);
void setWalkingStepCount(uint8_t* payload,uint32_t steps);
uint32_t getLifetimeRunningMiles(uint8_t* payload);
void setLifetimeRunningMiles(uint8_t* payload,uint32_t miles);
uint32_t getLifetimeWalkingMiles(uint8_t* payload);
void setLifetimeWalkingMile(uint8_t* payload,uint32_t miles);
uint16_t getTc(uint8_t* payload);
void setTc(uint8_t* payload,uint16_t tc);
const char* getDeviceType(uint8_t device);
void setDeviceType(uint8_t* payload,uint8_t device);
void printd(const uint8_t* d, uint8_t sz);
void process(uint8_t* packet);
int hexv(char c);
int getSerialNumChars(char c);
#endif