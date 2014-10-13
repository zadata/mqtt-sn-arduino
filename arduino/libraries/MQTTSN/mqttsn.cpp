/*************************************************************************************
** File: mqtts.cpp
** Date: 05/09/2012
**
** @doc constants, classes related to MQTT-SN protocol handling
**
** @author Zvi Avraham
** @copyright 2012-2014 ZADATA Ltd. All Rights Reserved.
** @end
*************************************************************************************/

#include <string.h>

#include "mqtts.h"

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"


#endif

#include "HardwareSerial.h"



//////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSCommand
//////////////////////////////////////////////////////////////////////////////////

MQTTSCommand::MQTTSCommand() {

}

uint8_t MQTTSCommand::getCommandLength(){
	return 0;
}

void MQTTSCommand::encode(byte buffer[], unsigned int &length){

}

void MQTTSCommand::decode(byte buffer[], const unsigned int length){

}

void MQTTSCommand::encodeHeader(uint8_t type, byte buffer[], unsigned int length) {
	buffer[0] = length;
	buffer[1] = type;
}


////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSAdvertise
////////////////////////////////////////////////////////////////////////////////


MQTTSAdvertise::MQTTSAdvertise() {

}

MQTTSAdvertise::MQTTSAdvertise(uint8_t gwId, uint16_t duration) {
	_gwId = gwId;
	_duration = duration;
}

uint8_t MQTTSAdvertise::getCommandLength(){
	return 3;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSAdvertise::encode(byte buffer[], unsigned int &length ) { // out
	buffer[2] = _gwId;
	buffer[3] = _duration >> 8;
	buffer[4] = _duration & 0xFF;

	// total buffer length
	length = 5;
}

void MQTTSAdvertise::decode(byte buffer[], const unsigned int length) {
	_gwId = buffer[2];
	_duration = (buffer[3] << 8) | buffer[4];
}

void MQTTSAdvertise::setGwId(uint8_t gwId) {
	_gwId = gwId;
}

uint8_t MQTTSAdvertise::getGwId() {
	return _gwId;
}

void MQTTSAdvertise::setDuration(uint16_t duration) {
	_duration = duration;
}

uint16_t MQTTSAdvertise::getDuration() {
	return _duration;
}



////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSSearchGw
////////////////////////////////////////////////////////////////////////////////

MQTTSSearchGw::MQTTSSearchGw() {

}

MQTTSSearchGw::MQTTSSearchGw(uint8_t radius) {
	_radius = radius;
}

uint8_t MQTTSSearchGw::getCommandLength(){
	return 1;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSSearchGw::encode(byte buffer[], unsigned int &length){
	buffer[2] = _radius;

	// total buffer length
	length = 3;
}

void MQTTSSearchGw::decode(byte buffer[], const unsigned int length){
	_radius = buffer[2];
}

void MQTTSSearchGw::setRadius(uint8_t radius) {
	_radius = radius;
}

uint8_t MQTTSSearchGw::getRadius() {
	return _radius;
}


////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSGwInfo
////////////////////////////////////////////////////////////////////////////////

MQTTSGwInfo::MQTTSGwInfo() {

}

MQTTSGwInfo::MQTTSGwInfo(uint8_t gwId, uint32_t slAdd, uint32_t shAdd) {
	_gwId 	= gwId;
	_slAdd 	= slAdd;
	_shAdd 	= shAdd;
}

uint8_t MQTTSGwInfo::getCommandLength(){
	return 9;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSGwInfo::encode(byte buffer[], unsigned int &length){
	buffer[2] = _gwId;
	buffer[3] = _slAdd >> 24;
	buffer[4] = _slAdd >> 16;
	buffer[5] = _slAdd >> 8;
	buffer[6] = _slAdd & 0xFF;	
	buffer[7] = _shAdd >> 24;
	buffer[8] = _shAdd >> 16;
	buffer[9] = _shAdd >> 8;
	buffer[10] = _shAdd & 0xFF;

	// total buffer length
	length = 11;

}

void MQTTSGwInfo::decode(byte buffer[], const unsigned int length){

	_gwId = buffer[2];
	_slAdd = (uint32_t(buffer[3]) << 24) + (uint32_t(buffer[4]) << 16) + (uint16_t(buffer[5]) << 8) + (uint8_t(buffer[6]));
	_shAdd = (uint32_t(buffer[7]) << 24) + (uint32_t(buffer[8]) << 16) + (uint16_t(buffer[9]) << 8) + (uint8_t(buffer[10]));

}

void MQTTSGwInfo::setGwId(uint8_t gwId) {
	_gwId = gwId;
}

uint8_t MQTTSGwInfo::getGwId() {
	return _gwId;
}

void MQTTSGwInfo::setGwSlAdd(uint32_t slAdd) {
	_slAdd = slAdd;
}

uint32_t MQTTSGwInfo::getGwSlAdd() {
	return _slAdd;
}

void MQTTSGwInfo::setGwShAdd(uint32_t shAdd) {
	_shAdd = shAdd;
}

uint32_t MQTTSGwInfo::getGwShAdd() {
	return _shAdd;
}
////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSConnect
////////////////////////////////////////////////////////////////////////////////

MQTTSConnect::MQTTSConnect() {

}

MQTTSConnect::MQTTSConnect(uint8_t flags, uint8_t protocolId, uint16_t duration, char *clientId) {
	_flags.flags = flags;
	_protocolId = protocolId;
	_duration = duration;
	if(!_clientId) {
		// TODO : error 
	}
	_clientId = strdup(clientId);
}

uint8_t MQTTSConnect::getCommandLength(){
	return 4 + strlen(_clientId);
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSConnect::encode(byte buffer[], unsigned int &length){
	if(!_clientId) {
		// TODO : error 
	}

	buffer[2] = _flags.flags;
	buffer[3] = _protocolId;
	buffer[4] = _duration >> 8;
	buffer[5] = _duration & 0xFF;

	// copy clientId characters and add NUL-terminating character
	const unsigned int clientIdLen = strlen(_clientId);
	memcpy(buffer+6, _clientId, clientIdLen);
	// no need to copy NUL-terminating character

	// total buffer length
	length = 6 + clientIdLen;
}

void MQTTSConnect::decode(byte buffer[], const unsigned int length){
	if(length < 2) {
		// TODO : error 
	}

	_flags.flags = buffer[2];
	_protocolId = buffer[3];
	_duration = (buffer[4] << 8) | buffer[5];

	const unsigned int clientIdLen = length - 6;

	// free old buffer
	if(_clientId)
		free(_clientId);
	// allocate new buffer
	_clientId = (char *)malloc(clientIdLen+1);

	// copy topic characters and add NUL-terminating character
	memcpy(_clientId, buffer+6, clientIdLen);
	_clientId[clientIdLen] = '\0';
}

void MQTTSConnect::setFlags(uint8_t flags) {
	_flags.flags = flags;
}
	
FLAGS_BITS MQTTSConnect::getFlags() {
	return _flags;
}

void MQTTSConnect::setProtocolId(uint8_t protocolId) {
	_protocolId = protocolId;
}
	
uint8_t MQTTSConnect::getProtocolId() {
	return _protocolId;
}	

void MQTTSConnect::setDuration(uint16_t duration) {
	_duration = duration;
}

uint16_t MQTTSConnect::getDuration() {
	return _duration;
}	

void MQTTSConnect::setClientId(const char *clientId) {
	if(!_clientId) {
		// TODO : error 
	}
	_clientId = strdup(clientId);
}

char* MQTTSConnect::getClientId() {
	return _clientId;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSConAck
////////////////////////////////////////////////////////////////////////////////

MQTTSConAck::MQTTSConAck() {

}

MQTTSConAck::MQTTSConAck(uint8_t returnCode) {
	_returnCode = returnCode;
}

uint8_t MQTTSConAck::getCommandLength(){
	return 1;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSConAck::encode(byte buffer[], unsigned int &length){
	buffer[2] = _returnCode;

	// total buffer length
	length = 3;
}

void MQTTSConAck::decode(byte buffer[], const unsigned int length){
	_returnCode = buffer[2];
}

void MQTTSConAck::setReturnCode(uint8_t returnCode) {
	_returnCode = returnCode;
}

uint8_t MQTTSConAck::getReturnCode() {
	return _returnCode;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSWillTopicReq
////////////////////////////////////////////////////////////////////////////////

/*
MQTTSWillTopicReq::MQTTSWillTopicReq() {

}
*/

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSWillTopic
////////////////////////////////////////////////////////////////////////////////

MQTTSWillTopic::MQTTSWillTopic() {
	if(_willTopic)
		free(_willTopic);
}

MQTTSWillTopic::MQTTSWillTopic(uint8_t flags, const char *willTopic) {
	_flags.flags = flags;
	if(!_willTopic) {
		// TODO : error 
	}
	_willTopic = strdup(willTopic);
}

uint8_t MQTTSWillTopic::getCommandLength(){
	if(!_willTopic) {
		// TODO : error 
	}
	// first byte for flags + WillTopic bytes
	return 1 + strlen(_willTopic);
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSWillTopic::encode(byte buffer[], unsigned int &length){
	if(!_willTopic) {
		// TODO : error 
	}

	buffer[2] = _flags.flags;
	// copy topic characters and add NUL-terminating character
	const unsigned int willLen = strlen(_willTopic);
	memcpy(buffer+3, _willTopic, willLen);
	// no need to copy NUL-terminating character

	// total buffer length
	length = 3 + willLen;
}

void MQTTSWillTopic::decode(byte buffer[], const unsigned int length){
	if(length < 2) {
		// TODO : error 
	}

	_flags.flags = buffer[2];
	const unsigned int willLen = length - 3;

	// free old buffer
	if(_willTopic)
		free(_willTopic);
	// allocate new buffer
	_willTopic = (char *)malloc(willLen+1);

	// copy topic characters and add NUL-terminating character
	memcpy(_willTopic, buffer+3, willLen);
	_willTopic[willLen] = '\0';
}

void MQTTSWillTopic::setWillTopic(const char *willTopic) {
	if(!_willTopic) {
		// TODO : error 
	}
	_willTopic = strdup(willTopic);
}

char* MQTTSWillTopic::getWillTopic() {
	return _willTopic;
}

void MQTTSWillTopic::setFlags(uint8_t flags) {
	_flags.flags = flags;
}
	
FLAGS_BITS MQTTSWillTopic::getFlags() {
	return _flags;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSWillMsgReq
////////////////////////////////////////////////////////////////////////////////

/*
MQTTSWillMsgReq::MQTTSWillMsgReq() {

}
*/

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSWillMsg
////////////////////////////////////////////////////////////////////////////////

MQTTSWillMsg::MQTTSWillMsg() {

}

MQTTSWillMsg::MQTTSWillMsg(const char *willMsg) {
	if(!_willMsg) {
		// TODO : error 
	}
	_willMsg = strdup(willMsg);
}

uint8_t MQTTSWillMsg::getCommandLength(){
	if(!_willMsg) {
		// TODO : error 
	}
	// first byte for flags + WillTopic bytes
	return strlen(_willMsg);
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSWillMsg::encode(byte buffer[], unsigned int &length){
	if(!_willMsg) {
		// TODO : error 
	}
	// copy topic characters and add NUL-terminating character
	const unsigned int willLen = strlen(_willMsg);
	memcpy(buffer+2, _willMsg, willLen);
	// no need to copy NUL-terminating character

	// total buffer length
	length = willLen+2;
}

void MQTTSWillMsg::decode(byte buffer[], const unsigned int length){
	if(length < 2) {
		// TODO : error 
	}
	const unsigned int willLen = length-2;

	// free old buffer
	if(_willMsg)
		free(_willMsg);
	// allocate new buffer
	_willMsg = (char *)malloc(willLen+1);

	// copy msg characters and add NUL-terminating character
	memcpy(_willMsg, buffer+2, willLen);
	_willMsg[willLen] = '\0';
}

void MQTTSWillMsg::setWillMsg(const char *willMsg) {
	if(!_willMsg) {
		// TODO : error 
	}
	_willMsg = strdup(willMsg);
}

char* MQTTSWillMsg::getWillMsg() {
	return _willMsg;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSRegister
////////////////////////////////////////////////////////////////////////////////

MQTTSRegister::MQTTSRegister() {

}

MQTTSRegister::MQTTSRegister(uint16_t topicId, uint16_t msgId, const char *topicName) {
	_topicId = 	topicId;
	_msgId = 	msgId;
	if(!_topicName) {
		// TODO : error 
	}
	_topicName = strdup(topicName);
}

uint8_t MQTTSRegister::getCommandLength(){
	return 4 + strlen(_topicName);
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSRegister::encode(byte buffer[], unsigned int &length){
	if(!_topicName) {
		// TODO : error 
	}
	buffer[2] = _topicId >> 8;
	buffer[3] = _topicId & 0xFF;
	buffer[4] = _msgId >> 8;
	buffer[5] = _msgId & 0xFF;
	// copy topic characters and add NUL-terminating character
	const unsigned int topicLen = strlen(_topicName);
	memcpy(buffer+6, _topicName, topicLen);
	// no need to copy NUL-terminating character

	// total buffer length
	length = 6 + topicLen;
}

void MQTTSRegister::decode(byte buffer[], const unsigned int length){
	if(length < 2) {
		// TODO : error 
	}

	_topicId = (buffer[2] << 8) | buffer[3];
	_msgId = (buffer[4] << 8) | buffer[5];
	const unsigned int topicLen = length - 6;

	// free old buffer
	if(_topicName)
		free(_topicName);
	// allocate new buffer
	_topicName = (char *)malloc(topicLen+1);

	// copy topic characters and add NUL-terminating character
	memcpy(_topicName, buffer+6, topicLen);
	_topicName[topicLen] = '\0';
}

void MQTTSRegister::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSRegister::getMsgId() {
	return _msgId;
}

void MQTTSRegister::setTopicId(uint16_t topicId) {
	_topicId = topicId;
}

uint16_t MQTTSRegister::getTopicId() {
	return _topicId;
}

void MQTTSRegister::setTopicName(const char *topicName) {
	if(!_topicName) {
		// TODO : error 
	}
	_topicName = strdup(topicName);
}

char* MQTTSRegister::getTopicName() {
	return _topicName;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSRegAck
////////////////////////////////////////////////////////////////////////////////



MQTTSRegAck::MQTTSRegAck() {

}

MQTTSRegAck::MQTTSRegAck(uint16_t topicId, uint16_t msgId, uint8_t returnCode) {
	_topicId = topicId;
	_msgId = msgId;
	_returnCode = returnCode;
}

uint8_t MQTTSRegAck::getCommandLength(){
	return 5;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSRegAck::encode(byte buffer[], unsigned int &length){
	buffer[2] = _topicId >> 8;
	buffer[3] = _topicId & 0xFF;
	buffer[4] = _msgId >> 8;
	buffer[5] = _msgId & 0xFF;
	buffer[6] = _returnCode;

	// total buffer length
	length = 7;
}

void MQTTSRegAck::decode(byte buffer[], const unsigned int length){
	_topicId = (buffer[2] << 8) | buffer[3];
	_msgId = (buffer[4] << 8) | buffer[5];
	_returnCode = buffer[6];
}

void MQTTSRegAck::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSRegAck::getMsgId() {
	return _msgId;
}

void MQTTSRegAck::setTopicId(uint16_t topicId) {
	_topicId = topicId;
}

uint16_t MQTTSRegAck::getTopicId() {
	return _topicId;
}

void MQTTSRegAck::setReturnCode(uint8_t returnCode) {
	_returnCode = returnCode;
}

uint8_t MQTTSRegAck::getReturnCode() {
	return _returnCode;
}



////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSPublish
////////////////////////////////////////////////////////////////////////////////

MQTTSPublish::MQTTSPublish() {

}

MQTTSPublish::MQTTSPublish(uint8_t flags, uint16_t topicId, uint16_t msgId, char* data) {
	_flags.flags = flags;
	_topicId = topicId;
	_msgId = msgId;
	if(!_data) {
		// TODO : error 
	}
	_data = strdup(data);
}

uint8_t MQTTSPublish::getCommandLength(){
	return 5 + strlen(_data);
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSPublish::encode(byte buffer[], unsigned int &length){
	if(!_data) {
		// TODO : error 
	}
	buffer[2] = _flags.flags;
	buffer[3] = _topicId >> 8;
	buffer[4] = _topicId & 0xFF;
	buffer[5] = _msgId >> 8;
	buffer[6] = _msgId & 0xFF;
	// copy data characters and add NUL-terminating character
	const unsigned int dataLen = strlen(_data);
	memcpy(buffer+7, _data, dataLen);
	// no need to copy NUL-terminating character

	// total buffer length
	length = 7 + dataLen;
}

void MQTTSPublish::decode(byte buffer[], const unsigned int length){
	if(length < 2) {
		// TODO : error 
	}

	_flags.flags = buffer[2];
	_topicId = (buffer[3] << 8) | buffer[4];
	_msgId = (buffer[5] << 8) | buffer[6];
	const unsigned int dataLen = length - 7;

	// free old buffer
	if(_data)
		free(_data);
	// allocate new buffer
	_data = (char *)malloc(dataLen+1);

	// copy data characters and add NUL-terminating character
	memcpy(_data, buffer+7, dataLen);
	_data[dataLen] = '\0';
}

void MQTTSPublish::setFlags(uint8_t flags) {
	_flags.flags = flags;
}

FLAGS_BITS MQTTSPublish::getFlags() {
	return _flags;
}

void MQTTSPublish::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSPublish::getMsgId() {
	return _msgId;
}

void MQTTSPublish::setTopicId(uint16_t topicId) {
	_topicId = topicId;
}

uint16_t MQTTSPublish::getTopicId() {
	return _topicId;
}

void MQTTSPublish::setData(const char *data) {
	if(!_data) {
		// TODO : error 
	}
	_data = strdup(data);
}

char* MQTTSPublish::getData() {
	return _data;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSPubAck
////////////////////////////////////////////////////////////////////////////////


MQTTSPubAck::MQTTSPubAck() {

}

MQTTSPubAck::MQTTSPubAck(uint16_t msgId, uint16_t topicId, uint8_t returnCode) {
	_msgId = msgId;
	_topicId = topicId;
	_returnCode = returnCode;
}

uint8_t MQTTSPubAck::getCommandLength(){
	return 5;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSPubAck::encode(byte buffer[], unsigned int &length){
	buffer[2] = _topicId >> 8;
	buffer[3] = _topicId & 0xFF;
	buffer[4] = _msgId >> 8;
	buffer[5] = _msgId & 0xFF;
	buffer[6] = _returnCode;

	// total buffer length
	length = 7;
}

void MQTTSPubAck::decode(byte buffer[], const unsigned int length){
	_topicId = (buffer[2] << 8) | buffer[3];
	_msgId = (buffer[4] << 8) | buffer[5];
	_returnCode = buffer[6];

}

void MQTTSPubAck::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSPubAck::getMsgId() {
	return _msgId;
}

void MQTTSPubAck::setTopicId(uint16_t topicId) {
	_topicId = topicId;
}

uint16_t MQTTSPubAck::getTopicId() {
	return _topicId;
}

void MQTTSPubAck::setReturnCode(uint8_t returnCode) {
	_returnCode = returnCode;
}

uint8_t MQTTSPubAck::getReturnCode() {
	return _returnCode;
}

/*
////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSPubRec
////////////////////////////////////////////////////////////////////////////////

MQTTSPubRec::MQTTSPubRec() {

}

MQTTSPubRec::MQTTSPubRec(uint16_t msgId) {
	_msgId = msgId;
}

uint8_t MQTTSPubRec::getCommandLength(){
	return 2;
}

void MQTTSPubRec::encode(byte buffer[], unsigned int &length){
	buffer[2] = _msgId >> 8;
	buffer[3] = _msgId & 0xFF;

	// total buffer length
	length = 4;
}

void MQTTSPubRec::decode(byte buffer[], const unsigned int length){
	_msgId = (buffer[2] << 8) | buffer[3];
}

void MQTTSPubRec::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSPubRec::getMsgId() {
	return _msgId;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSPubRel
////////////////////////////////////////////////////////////////////////////////

MQTTSPubRel::MQTTSPubRel() {

}

MQTTSPubRel::MQTTSPubRel(uint16_t msgId) {
	_msgId = msgId;
}

uint8_t MQTTSPubRel::getCommandLength(){
	return 2;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSPubRel::encode(byte buffer[], unsigned int &length){
	buffer[2] = _msgId >> 8;
	buffer[3] = _msgId & 0xFF;

	// total buffer length
	length = 4;
}

void MQTTSPubRel::decode(byte buffer[], const unsigned int length){
	_msgId = (buffer[2] << 8) | buffer[3];
}

void MQTTSPubRel::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSPubRel::getMsgId() {
	return _msgId;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSPubComp
////////////////////////////////////////////////////////////////////////////////

MQTTSPubComp::MQTTSPubComp() {

}

MQTTSPubComp::MQTTSPubComp(uint16_t msgId) {
	_msgId = msgId;
}

uint8_t MQTTSPubComp::getCommandLength(){
	return 2;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSPubComp::encode(byte buffer[], unsigned int &length){
	buffer[2] = _msgId >> 8;
	buffer[3] = _msgId & 0xFF;

	// total buffer length
	length = 4;
}

void MQTTSPubComp::decode(byte buffer[], const unsigned int length){
	_msgId = (buffer[2] << 8) | buffer[3];
}

void MQTTSPubComp::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSPubComp::getMsgId() {
	return _msgId;
}
*/
////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSSubscribe
////////////////////////////////////////////////////////////////////////////////

MQTTSSubscribe::MQTTSSubscribe() {

}

MQTTSSubscribe::MQTTSSubscribe(uint8_t flags, uint16_t msgId, char* topicName) {
	_flags.flags = flags;
	_msgId = msgId;
	if(!_topicName) {
		// TODO : error 
	}
	_topicName = strdup(topicName);
}

uint8_t MQTTSSubscribe::getCommandLength(){
	return 3 + strlen(_topicName);
}

MQTTSSubscribe::~MQTTSSubscribe() { 
	if(_topicName) 
		free(_topicName); 
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSSubscribe::encode(byte buffer[], unsigned int &length){
	buffer[2] = _flags.flags;
	buffer[3] = _msgId >> 8;
	buffer[4] = _msgId & 0xFF;
	// copy topicName characters and add NUL-terminating character
	const unsigned int topicNameLen = strlen(_topicName);
	memcpy(buffer+5, _topicName, topicNameLen);
	// no need to copy NUL-terminating character

	// total buffer length
	length = 5 + topicNameLen;
}

void MQTTSSubscribe::decode(byte buffer[], const unsigned int length){
	_flags.flags = buffer[2];	
	_msgId = (buffer[3] << 8) | buffer[4];
	const unsigned int topicNameLen = length - 5;

	// free old buffer
	if(_topicName)
		free(_topicName);
	// allocate new buffer
	_topicName = (char *)malloc(topicNameLen+1);

	// copy data characters and add NUL-terminating character
	memcpy(_topicName, buffer+5, topicNameLen);
	_topicName[topicNameLen] = '\0';
}

void MQTTSSubscribe::setFlags(uint8_t flags) {
	_flags.flags = flags;
}

FLAGS_BITS MQTTSSubscribe::getFlags() {
	return _flags;
}

void MQTTSSubscribe::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSSubscribe::getMsgId() {
	return _msgId;
}

void MQTTSSubscribe::setTopicName(const char* topicName) {
	_topicName = strdup(topicName);
}

char* MQTTSSubscribe::getTopicName() {
	return _topicName;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSSubAck
////////////////////////////////////////////////////////////////////////////////

MQTTSSubAck::MQTTSSubAck() {

}

MQTTSSubAck::MQTTSSubAck(uint8_t flags, uint16_t topicId, uint16_t msgId, uint8_t returnCode) {
	_flags.flags = flags;
	_topicId = topicId;
	_msgId = msgId;
	_returnCode = returnCode;
}

uint8_t MQTTSSubAck::getCommandLength(){
	return 6;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSSubAck::encode(byte buffer[], unsigned int &length){
	buffer[2] = _flags.flags;
	buffer[3] = _topicId >> 8;
	buffer[4] = _topicId & 0xFF;
	buffer[5] = _msgId >> 8;
	buffer[6] = _msgId & 0xFF;
	buffer[7] = _returnCode;

	// total buffer length
	length = 8;
}

void MQTTSSubAck::decode(byte buffer[], const unsigned int length){
	_flags.flags = buffer[2];	
	_topicId = (buffer[3] << 8) | buffer[4];
	_msgId = (buffer[5] << 8) | buffer[6];
	_returnCode = buffer[7];	
}

void MQTTSSubAck::setFlags(uint8_t flags) {
	_flags.flags = flags;
}

FLAGS_BITS MQTTSSubAck::getFlags() {
	return _flags;
}

void MQTTSSubAck::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSSubAck::getMsgId() {
	return _msgId;
}

void MQTTSSubAck::setTopicId(uint16_t topicId) {
	_topicId = topicId;
}

uint16_t MQTTSSubAck::getTopicId() {
	return _topicId;
}

void MQTTSSubAck::setReturnCode(uint8_t returnCode) {
	_returnCode = returnCode;
}

uint8_t MQTTSSubAck::getReturnCode() {
	return _returnCode;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSUnsubscribe
////////////////////////////////////////////////////////////////////////////////

MQTTSUnsubscribe::MQTTSUnsubscribe() {

}

MQTTSUnsubscribe::MQTTSUnsubscribe(uint8_t flags, uint16_t msgId, uint16_t topicId) {
	_flags.flags = flags;
	_msgId = msgId;
	_topicId = topicId;
}

uint8_t MQTTSUnsubscribe::getCommandLength(){
	return 5;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSUnsubscribe::encode(byte buffer[], unsigned int &length){
	buffer[2] = _flags.flags;
	buffer[3] = _msgId >> 8;
	buffer[4] = _msgId & 0xFF;
	buffer[5] = _topicId >> 8;
	buffer[6] = _topicId & 0xFF;

	// total buffer length
	length = 7;
}

void MQTTSUnsubscribe::decode(byte buffer[], const unsigned int length){
	_flags.flags = buffer[2];	
	_msgId = (buffer[3] << 8) | buffer[4];
	_topicId = (buffer[5] << 8) | buffer[6];
}

void MQTTSUnsubscribe::setFlags(uint8_t flags) {
	_flags.flags = flags;
}

FLAGS_BITS MQTTSUnsubscribe::getFlags() {
	return _flags;
}

void MQTTSUnsubscribe::setTopicId(uint16_t topicId) {
	_topicId = topicId;
}

uint16_t MQTTSUnsubscribe::getTopicId() {
	return _topicId;
}

void MQTTSUnsubscribe::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSUnsubscribe::getMsgId() {
	return _msgId;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSUnsubAck
////////////////////////////////////////////////////////////////////////////////

MQTTSUnsubAck::MQTTSUnsubAck() {

}

MQTTSUnsubAck::MQTTSUnsubAck(uint16_t msgId) {
	_msgId = msgId;
}

uint8_t MQTTSUnsubAck::getCommandLength(){
	return 2;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSUnsubAck::encode(byte buffer[], unsigned int &length){
	buffer[2] = _msgId >> 8;
	buffer[3] = _msgId & 0xFF;

	// total buffer length
	length = 4;
}

void MQTTSUnsubAck::decode(byte buffer[], const unsigned int length){
	_msgId = (buffer[2] << 8) | buffer[3];

}

void MQTTSUnsubAck::setMsgId(uint16_t msgId) {
	_msgId = msgId;
}

uint16_t MQTTSUnsubAck::getMsgId() {
	return _msgId;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSPingReq
////////////////////////////////////////////////////////////////////////////////

MQTTSPingReq::MQTTSPingReq() {
	_clientId = NULL;
}

MQTTSPingReq::MQTTSPingReq(char *clientId) {
	if(!_clientId) {
		// TODO : error 
	}
	_clientId = strdup(clientId);
}

MQTTSPingReq::~MQTTSPingReq() { 
	if(_clientId) 
		free(_clientId); 
}


uint8_t MQTTSPingReq::getCommandLength(){
	return (_clientId ? strlen(_clientId) : 0);
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSPingReq::encode(byte buffer[], unsigned int &length){
	if(!_clientId) {
		// TODO : error 
	}
	// copy client id characters and add NUL-terminating character
	const unsigned int clientIdLen = getCommandLength();
	memcpy(buffer+2, _clientId, clientIdLen);
	// no need to copy NUL-terminating character

	// total buffer length
	length = clientIdLen+2;
}

void MQTTSPingReq::decode(byte buffer[], const unsigned int length){
	if(length < 2) {
		// TODO : error 
	}

	const unsigned int clientIdLen = length-2;

	// free old buffer
	if(_clientId)
		free(_clientId);
	// allocate new buffer
	_clientId = (char *)malloc(clientIdLen+1);

	// copy clientId characters and add NUL-terminating character
	memcpy(_clientId, buffer+2, clientIdLen);
	_clientId[clientIdLen] = '\0';
}

void MQTTSPingReq::setClientId(const char *clientId) {
	if(!_clientId) {
		// TODO : error 
	}
	_clientId = strdup(clientId);
}

char* MQTTSPingReq::getClientId() {
	return _clientId;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSPingResp
////////////////////////////////////////////////////////////////////////////////

MQTTSPingResp::MQTTSPingResp() {

}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSDisconnect
////////////////////////////////////////////////////////////////////////////////

MQTTSDisconnect::MQTTSDisconnect() : _duration(0) {

}

MQTTSDisconnect::MQTTSDisconnect(uint16_t duration) : _duration(duration) {
	
}

uint8_t MQTTSDisconnect::getCommandLength(){
	return 2;
}

// assumes that buffer already allocated by the caller - 
///should be big enough for the largest possible MQTTS message
void MQTTSDisconnect::encode(byte buffer[], unsigned int &length){
	if (_duration != 0) {
		buffer[2] = _duration >> 8;
		buffer[3] = _duration & 0xFF;

		// total buffer length
		length = 4;
	} else { 
		length = 2;
	}

}

void MQTTSDisconnect::decode(byte buffer[], const unsigned int length){
	if (length == 4) {
		_duration = (buffer[2] << 8) | buffer[3];
	} else {
		_duration = 0;
	}
}

void MQTTSDisconnect::setDuration(uint16_t duration) {
	_duration = duration;
}

uint16_t MQTTSDisconnect::getDuration() {
	return _duration;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSWillTopicUpd
////////////////////////////////////////////////////////////////////////////////

MQTTSWillTopicUpd::MQTTSWillTopicUpd() {

}

MQTTSWillTopicUpd::MQTTSWillTopicUpd(uint8_t flags, char *willTopic) {
	_flags.flags = flags;
	if(!_willTopic) {
		// TODO : error 
	}
	_willTopic = strdup(willTopic);
}

uint8_t MQTTSWillTopicUpd::getCommandLength(){
	return 1 + strlen(_willTopic);
}

void MQTTSWillTopicUpd::encode(byte buffer[], unsigned int &length){
	if(!_willTopic) {
		// TODO : error 
	}
	buffer[2] = _flags.flags;
	// copy will topic characters and add NUL-terminating character
	const unsigned int willLen = strlen(_willTopic);
	memcpy(buffer+3, _willTopic, willLen);
	// no need to copy NUL-terminating character

	// total buffer length
	length = 3 + willLen;
}

void MQTTSWillTopicUpd::decode(byte buffer[], const unsigned int length){
	if(length < 2) {
		// TODO : error 
	}

	_flags.flags = buffer[2];
	const unsigned int willLen = length - 3;

	// free old buffer
	if(_willTopic)
		free(_willTopic);
	// allocate new buffer
	_willTopic = (char *)malloc(willLen+1);

	// copy will topic characters and add NUL-terminating character
	memcpy(_willTopic, buffer+3, willLen);
	_willTopic[willLen] = '\0';
}

void MQTTSWillTopicUpd::setFlags(uint8_t flags) {
	_flags.flags = flags;
}
	
FLAGS_BITS MQTTSWillTopicUpd::getFlags() {
	return _flags;
}

void MQTTSWillTopicUpd::setWillTopic(const char *willTopic) {
	if(!_willTopic) {
		// TODO : error 
	}
	_willTopic = strdup(willTopic);
}

char* MQTTSWillTopicUpd::getWillTopic() {
	return _willTopic;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSWillTopicResp
////////////////////////////////////////////////////////////////////////////////

MQTTSWillTopicResp::MQTTSWillTopicResp() {

}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSWillMsgUpd
////////////////////////////////////////////////////////////////////////////////

MQTTSWillMsgUpd::MQTTSWillMsgUpd() {

}

MQTTSWillMsgUpd::MQTTSWillMsgUpd(char *willMsg) {
	if(!_willMsg) {
		// TODO : error 
	}
	_willMsg = strdup(willMsg);
}

uint8_t MQTTSWillMsgUpd::getCommandLength(){
	return strlen(_willMsg);
}

void MQTTSWillMsgUpd::encode(byte buffer[], unsigned int &length){
	if(!_willMsg) {
		// TODO : error 
	}
	// copy will msg characters and add NUL-terminating character
	const unsigned int willLen = strlen(_willMsg);
	memcpy(buffer+2, _willMsg, willLen);
	// no need to copy NUL-terminating character

	// total buffer length
	length = willLen+2;
}

void MQTTSWillMsgUpd::decode(byte buffer[], const unsigned int length){
	if(length < 2) {
		// TODO : error 
	}

	const unsigned int willLen = length-2;

	// free old buffer
	if(_willMsg)
		free(_willMsg);
	// allocate new buffer
	_willMsg = (char *)malloc(willLen+1);

	// copy will msg characters and add NUL-terminating character
	memcpy(_willMsg, buffer+2, willLen);
	_willMsg[willLen] = '\0';
}

void MQTTSWillMsgUpd::setWillMsg(const char *willMsg) {
	if(!_willMsg) {
		// TODO : error 
	}
	_willMsg = strdup(willMsg);
}

char* MQTTSWillMsgUpd::getWillMsg() {
	return _willMsg;
}

////////////////////////////////////////////////////////////////////////////////
// TODO MQTTSWillMsgResp
////////////////////////////////////////////////////////////////////////////////

MQTTSWillMsgResp::MQTTSWillMsgResp() {

}



