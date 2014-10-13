/*

	This code is released under the following license:
		- You may use the code for personal (non-commercial) projects free of charge on conditions that: (1) credit is given in any write-ups of the project, and (2) your code changes (if any) are made public at the same time as the project is.
		- You must contact Dmitry Grinberg (dmitrygr@gmail.com) if you would like to use this code in a commercial project.
		- If you do not agree to these terms, you may not use this code.

	A lot of hard work went into figuring all this out, so please respect the above.
    http://www.dmitry.co/index.php?p=.%2F04.Thoughts%2F05.%20Nike%20plus%20iPod
*/

#include "public_nike_dec.h"
#include "Arduino.h"

uint8_t getSrcFlags(const uint8_t* packet){
	return packet[0];	
}

uint8_t getDstFlags(const uint8_t* packet){
	return (getSrcFlags(packet) & 0x80) ? packet[1] : 0;
}

uint8_t getSrcTypeLen(const uint8_t* packet){
	return packet[0] & 3;	
}

uint8_t getSrcAddrLen(const uint8_t* packet){
	uint8_t ret = packet[0];
	
	if(!(ret & 3)) return 0;
	
	return ((ret & 0x0C) >> 2) + 1;
}

uint8_t getDstAddrLen(const uint8_t* packet){
	uint8_t ret = getDstFlags(packet);
	
	if(!(ret & 3)) return 0;
	
	return ((ret & 0x0C) >> 2) + 1;	
}

uint8_t getDstTypeLen(const uint8_t* packet){
	return getDstFlags(packet) & 3;
}

uint8_t* getPayloadPtr(uint8_t* packet){
	uint8_t L1, L2, L3;
	
	L1 = getSrcTypeLen(packet);
	L2 = getSrcAddrLen(packet);
	L3 = getDstTypeLen(packet) + getDstAddrLen(packet);
	L3 += L3 ? ((getDstFlags(packet) & 0x10) ? 2 : 1) : 0;

	return packet + L1 + L2 + L3 + 2;
}

uint32_t getAddr(const uint8_t* ptr, uint8_t sz){
	uint8_t bytes[4] = {0};
	switch((sz >> 2) & 3){
		case 3:
			bytes[3] = ptr[3];
		case 2:
			bytes[2] = ptr[2];
		case 1:
			bytes[1] = ptr[1];
		case 0:
			bytes[0] = ptr[0];
	}

	return (((uint32_t )(bytes[3])) << 24) | (((uint32_t )(bytes[2])) << 16) | (((uint32_t )(bytes[1])) << 8) | ((uint32_t )(bytes[0]));
}

uint32_t  getSrcAddr(const uint8_t* packet){

	uint8_t t, f = getSrcFlags(packet);
	
	t = (f & 3);
	if(!t) return 0;

	packet += t + 1;
	if(f & 0x80) packet++;

	return getAddr(packet, f);
}

uint32_t getDstAddr(const uint8_t* packet){

	uint8_t L1, L2, L3;

	L1 = getDstTypeLen(packet);
	if(!L1) return 0;

	L2 = getDstFlags(packet);
	L3 = getSrcTypeLen(packet) + getSrcAddrLen(packet);

	packet += L1 + 2 + L3;
	return getAddr(packet, L2);
}

uint16_t getType(const uint8_t* ptr, uint8_t len){
	
	uint16_t t = 0;
	switch(len){
		case 2:
			t |= ((uint16_t)ptr[1]) << 8;
		case 1:
			t |= ptr[0];
	}
	return t;
}

uint16_t getSrcType(const uint8_t* packet){

	return getType(packet + 2, getSrcTypeLen(packet) - 1);
}

uint16_t getDstType(const uint8_t* packet){

	return getType(packet + getSrcTypeLen(packet) + getSrcAddrLen(packet) + 1, getDstTypeLen(packet) - 1);
}


uint8_t getPayloadLen(uint8_t* packet){
	uint8_t* payload = getPayloadPtr(packet);
	
	return packet + 28 - payload;
}

uint8_t getTimingByte(const uint8_t* packet){
	return packet[getTimingBytePosition(packet)];
}

void setTimingByte(uint8_t* packet,uint8_t timingByte,bool init){
    //timingByte always starts with 1010
    timingByte&=0x0F;
    if(!init) timingByte|=0xa0;
    
    packet[getTimingBytePosition(packet)]=timingByte;
}

uint8_t getTimingBytePosition(const uint8_t* packet){
    int timingBytePosition=0;
    
    uint8_t L1, L2, L3;
    
	L1 = getSrcTypeLen(packet);
	L2 = getSrcAddrLen(packet);
	L3 = getDstTypeLen(packet);
	L3 += getDstAddrLen(packet);
	if(L3) L3++;
    
    timingBytePosition=L1 + L2 + L3 + 1;
    
    return(timingBytePosition);
}

void descramblePayload(uint8_t* packet){
	uint8_t* payload;
	uint16_t srcAddr, tR;
	uint8_t t2 = 0, t1 = 0, t3, t9 = 0x23, i;
	uint8_t payloadLen, timingByte;
	
	
	payload = getPayloadPtr(packet);
	srcAddr = getSrcAddr(packet);
	payloadLen = getPayloadLen(packet);
	
	timingByte = getTimingByte(packet);
		
	if(srcAddr & 0x0080UL) t2 |= 4;
	if(srcAddr & 0x2000UL) t2 |= 2;
	if(srcAddr & 0x0400UL) t2 |= 1;
	
	if(srcAddr & 0x1000UL) t1 |= 4;
	if(srcAddr & 0x0008UL) t1 |= 2;
	if(srcAddr & 0x8000UL) t1 |= 1;
	
	i = 0;
	tR = srcAddr >> t1;
	timingByte += t2;
		
	for(i = 0; i < payloadLen; i++, payload++, timingByte++){
	
		timingByte &= 0x0F;
		t3 = randomTable1[timingByte];
		
		t3 += tR;
		t3 &= 0x0F;
		t3 = randomTable2[t3];
		t3 ^= t9;

		t9 = *payload;
		*payload = t9 - t3;
		if(t1 <= 11){
			t1++;
			tR >>= 1;
		}
		else{
			t1 = 0;
			tR = srcAddr;
		}
	}
}

void scramblePayload(uint8_t* packet) {
    uint8_t* payload;
    uint16_t srcAddr, tR;
    uint8_t t2 = 0, t1 = 0, t3, t9 = 0x23, i;
    uint8_t payloadLen, timingByte;
    
    payload = getPayloadPtr(packet);
    srcAddr = getSrcAddr(packet);
    payloadLen = getPayloadLen(packet);
    
    timingByte = getTimingByte(packet);
    
    if(srcAddr & 0x0080UL) t2 |= 4;
    if(srcAddr & 0x2000UL) t2 |= 2;
    if(srcAddr & 0x0400UL) t2 |= 1;
    
    if(srcAddr & 0x1000UL) t1 |= 4;
    if(srcAddr & 0x0008UL) t1 |= 2;
    if(srcAddr & 0x8000UL) t1 |= 1;
    
    i = 0;
    tR = srcAddr >> t1;
    timingByte += t2;
    
    for(i = 0; i < payloadLen; i++, payload++, timingByte++){
        timingByte &= 0x0F;
        
        t3 = randomTable1[timingByte]; 
        t3 += tR;
        t3 &= 0x0F;
        t3 = randomTable2[t3];
        t3 ^= t9;
        
        // ONLY THIS BIT IS DIFFERENT
        *payload += t3;
        t9 = *payload;
        // AND THAT'S IT
        
        if (t1 <= 11) {
            t1++;
            tR >>= 1;
        } else {
            t1 = 0;
            tR = srcAddr;
        }
    }
}

void calcSN(uint32_t  sn, uint8_t* decodedPacket, char* dst){	//will give partial SN even without a full packet!
	uint16_t t;

	sn += 0xFF000000;

	dst[7] = serialNumChars[sn % 34];	//without the DECODED packet we can only do so much:
	sn /= 34;
	dst[6] = serialNumChars[sn % 34];
	sn /= 34;
	dst[5] = serialNumChars[sn % 34];
	sn /= 34;
	t = sn % 1156;
	sn /= 1156;
	
	dst[3] = serialNumChars[(t % 54) / 10];
	dst[4] = serialNumChars[(t % 54) % 10];
	dst[2] = serialNumChars[t / 54];
	dst[1] = serialNumChars[sn % 34];
	sn /= 34;
	
	if(decodedPacket){	//we can do more
		uint8_t* p = getPayloadPtr(decodedPacket) + 0x10;
		uint32_t  x = 0;
		
		x = (x << 8) | (p[2] & 0x1F);
		x = (x << 8) | p[1];
		x = (x << 8) | p[0];
		
		dst[10] = serialNumChars[x % 34];
		x /= 34;
		
		dst[9] = serialNumChars[x % 34];
		x /= 34;
		
		dst[8] = serialNumChars[x % 34];
		x /= 34;
		
		dst[0] = serialNumChars[x % 34];
	}
	else{
		dst[0] = '?';
		dst[8] = '?';
		dst[9] = '?';
		dst[10] = '?';
	}
	
	dst[11] = 0;
}

void setSN(uint8_t* payload, char* sn){
    //int getSerialNumChars(char c);
}

uint16_t getOnHours(uint8_t* payload){
	return (((uint16_t)(payload[18] & 0xE0)) << 3) + payload[1];	
}

void setOnHours(uint8_t* payload, uint16_t hours) {
    payload[1]  = (uint8_t)(hours & 0xff);
    payload[18] &= 0x1f;
    payload[18] |= (uint8_t)((hours >> 8) & 0x07) << 5;
}

uint32_t  getRunningStepCount(uint8_t* payload){
	uint32_t  r = 0;

	r = (r << 8) | payload[12];
	r = (r << 8) | payload[11];
	r = (r << 8) | payload[10];
	
	return r;	
}

void setRunningStepCount(uint8_t* payload,uint32_t steps){
    payload[10]= (steps >>0)   & 0xFF;
    payload[11]= (steps >>8)   & 0xFF;
    payload[12]= (steps >>16)  & 0xFF;
}

uint32_t  getWalkingStepCount(uint8_t* payload){
	uint32_t  r = 0;

	r = (r << 8) | payload[6];
	r = (r << 8) | payload[5];
	r = (r << 8) | payload[4];
	
	return r;	
}

void setWalkingStepCount(uint8_t* payload,uint32_t steps){
    payload[4]= (steps >>0)  & 0xFF;
    payload[5]= (steps >>8)  & 0xFF;
    payload[6]= (steps >>16) & 0xFF;
}

uint32_t getLifetimeRunningMiles(uint8_t* payload){ // mul by 64 and div by 18947.368115186691 to get actual miles
	uint32_t  r = 0;

	r = (r << 8) | payload[15];
	r = (r << 8) | payload[14];
	r = (r << 8) | payload[13];
	
	return r;	
}

void setLifetimeRunningMiles(uint8_t* payload,uint32_t miles){ // mul by 64 and div by 18947.368115186691 to get actual miles
    payload[13]= (miles >> 0)  & 0xFF;
    payload[14]= (miles >> 8)  & 0xFF;
    payload[15]= (miles >> 16) & 0xFF;
}

uint32_t getLifetimeWalkingMiles(uint8_t* payload){	// mul by 64 and div by 15319.148451089859 to get actual miles
	uint32_t r = 0;

	r = (r << 8) | payload[9];
	r = (r << 8) | payload[8];
	r = (r << 8) | payload[7];
	
	return r;
}

void setLifetimeWalkingMile(uint8_t* payload,uint32_t miles){ // mul by 64 and div by 15319.148451089859 to get actual miles
    payload[7]= (miles >> 0)   & 0xFF;
    payload[8]= (miles >> 8)   & 0xFF;
    payload[9]= (miles >> 16)  & 0xFF;
}

uint16_t getTc(uint8_t* payload){	//what the hell is "Tc" anyways?
	uint16_t r = 0;
	
	r = (r << 8) | payload[3];
	r = (r << 8) | payload[2];
	
	return r & 0x7FF;
}

void setTc(uint8_t* payload,uint16_t tc){
    tc= tc & 0x7FF; //this should have no effect
    
    payload[2]=tc & 0xFF;
    tc >> 8;
    payload[3]=tc & 0xFF;
}

const char* getDeviceType(uint8_t device){
	switch(device){
		case 0x01:
			return "Polar heart rate monitor";
		case 0x06:
			return "Nike+ foot sensor";
		case 0x0D:
			return "Nike+ Amp+ remote";
		default:
			return "Unknown device";
	}
}

void setDeviceType(uint8_t* payload,uint8_t device){
    payload[0]=device;
}

void printd(const uint8_t* d, uint8_t sz){
	for (int i=0; i<sz; ++i) {
		Serial.print(d[i],HEX);
	}
}

void process(uint8_t* packet){
	char sn[12];
	uint8_t* payload;
	
	descramblePayload(packet);
	payload = getPayloadPtr(packet);
	calcSN(getSrcAddr(packet), packet, sn);
	Serial.println("  FRAME:");
	Serial.print("    -> src flags:");
	Serial.println(getSrcFlags(packet),HEX);
	Serial.print("    -> src type len:");
	Serial.println(getSrcTypeLen(packet),HEX);
	Serial.print("    -> src type:");
	Serial.println(getSrcType(packet));
	Serial.print("    -> src addr len:");
	Serial.println(getSrcAddrLen(packet),HEX);
	Serial.print("    -> src addr:");
	Serial.println(getSrcAddr(packet),HEX);
	Serial.print("    -> dst flags:");
	Serial.println(getDstFlags(packet));
	Serial.print("    -> dst type len:");
	Serial.println(getDstTypeLen(packet));
	Serial.print("    -> dst type:");
	Serial.println(getDstType(packet));
	Serial.print("    -> dst addr len:");
	Serial.println(getDstAddrLen(packet));
	
	Serial.print("    -> dst addr:");
	Serial.println(getDstAddr(packet));
	Serial.print("    -> timing byte = ");
	Serial.println(getTimingByte(packet));
	Serial.print("    -> payload len = ");
	Serial.println(getPayloadLen(packet));
	Serial.println("  DATA:");
	Serial.print("    ->packet is from device type ");
	Serial.println(getDeviceType(payload[0]));
	if(payload[0] == 0x06){
		Serial.print("    ->packet is from device ");
		Serial.println(sn);
		Serial.println("    ->Nike+ foot pod data:");
		Serial.print("        ->on hours:");
		Serial.println(getOnHours(payload));
		Serial.print("        ->Tc:");
		Serial.println(getTc(payload));
		Serial.print("        ->walking steps:");
		Serial.println(getWalkingStepCount(payload));
		Serial.print("        ->running steps:");
		Serial.println(getRunningStepCount(payload));
		Serial.print("        -> lifetime walking miles:");
		Serial.println(getLifetimeWalkingMiles(payload));
		Serial.print("        -> lifetime running miles:");
		Serial.println(getLifetimeRunningMiles(payload));
	}
	Serial.print("    ->raw payload: ");
	printd(payload, getPayloadLen(packet));
	Serial.println();
}

int hexv(char c){
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'A' && c <= 'F') return c + 10 - 'A';
	if(c >= 'a' && c <= 'f') return c + 10 - 'a';
	return -1;
}

int getSerialNumChars(char c){
    int result=-1;
    
    for(int i=0;i<sizeof(serialNumChars);++i){
        if(serialNumChars[i]==c){
            result=i;
        }
    }
    
    return(result);
}