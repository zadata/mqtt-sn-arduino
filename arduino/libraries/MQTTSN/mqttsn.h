////////////////////////////////////////////////////////////////////////////////
/// File: mqttsn.h
/// Date: 05/09/2012
///
/// constants, classes related to MQTT-SN protocol handling
///
/// \author    Zvi Avraham
/// \copyright 2012-2014 ZADATA Ltd. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef MQTTSN_H
#define MQTTSN_H


#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <inttypes.h>

/// \brief MQTT-SN message types
/// The MsgType field is 1-octet long and specifies the message type

#define MQTTS_ADVERTISE     0x00 //!<  ADVERTISE message is broadcasted periodically by a gateway to advertise its presence.
#define MQTTS_SEARCHGW      0x01 //!<  SEARCHGW message is broadcasted by a client when it searches for a GW.
#define MQTTS_GWINFO        0x02 //!<  GWINFO message is sent as response to a SEARCHGW message using the broadcast service
                                 //       of the underlying layer, with the radius as indicated in the SEARCHGW message.
//  RESERVED -              0x03

#define MQTTS_CONNECT       0x04 //!<  CONNECT message is sent by a client to setup a connection.
#define MQTTS_CONNACK       0x05 //!<  CONNACK message is sent by the broker in response to a connection request from a client.

#define MQTTS_WILLTOPICREQ  0x06 //!<  WILLTOPICREQ message is sent by the GW to request a client for sending the Will topic name.
#define MQTTS_WILLTOPIC     0x07 //!<  WILLTOPIC message is sent by a client as response to the WILLTOPICREQ message for
                                 //       transferring its Will topic name to the GW.
#define MQTTS_WILLMSGREQ    0x08 //!<  WILLMSGREQ message is sent by the GW to request a client for sending the Will message.
#define MQTTS_WILLMSG       0x09 //!<  WILLMSG message is sent by a client as response to a WILLMSGREQ for transferring its Will message to the GW.

#define MQTTS_REGISTER      0x0A //!<  REGISTER message is sent by a client to a GW for requesting a topic id value for the included topic name.
#define MQTTS_REGACK        0x0B //!<  REGACK message is sent by a client or by a GW as an acknowledgment to the receipt and processing of a REGISTER message.

#define MQTTS_PUBLISH       0x0C //!<  PUBLISH message is used by both clients and gateways to publish data for a certain topic. 
#define MQTTS_PUBACK        0x0D //!<  The PUBACK message is sent by a gateway or a client as an acknowledgment to the receipt and processing 
                                 //       of a PUBLISH message in case of QoS levels 1 or 2.
#define MQTTS_PUBCOMP       0x0E //!<  As with MQTT, the PUBREC, PUBREL, and PUBCOMP messages are used in conjunction with a PUBLISH
                                 //       message with QoS level 2.
#define MQTTS_PUBREC        0x0F  
#define MQTTS_PUBREL        0x10 

// RESERVED -               0x11

#define MQTTS_SUBSCRIBE     0x12 //!<  SUBSCRIBE message is used by a client to subscribe to a certain topic name.
#define MQTTS_SUBACK        0x13 //!<  SUBACK message is sent by a gateway to a client as an acknowledgment to the receipt and processing of a SUBSCRIBE message.
#define MQTTS_UNSUBSCRIBE   0x14 //!<  UNSUBSCRIBE message is sent by the client to the GW to unsubscribe from named topics.
#define MQTTS_UNSUBACK      0x15 //!<  UNSUBACK message is sent by a GW to acknowledge the receipt and processing of an UNSUBSCRIBE message.

#define MQTTS_PINGREQ       0x16 //!<  As with MQTT, the PINGREQ message is an ”are you alive” message that is sent from or received by a connected client.
#define MQTTS_PINGRESP      0x17 //!<  As with MQTT, a PINGRESP message is the response to a PINGREQ message and means ”yes I am alive”.

#define MQTTS_DISCONNECT    0x18 //!<  As with MQTT, the DISCONNECT message is sent by a client to indicate that it wants to close the connection.

// RESERVED -               0x19

#define MQTTS_WILLTOPICUPD  0x1A //!<  WILLTOPICUPD message is sent by a client to update its Will topic name stored in the GW/broker.     
#define MQTTS_WILLTOPICRESP 0x1B //!<  WILLTOPICRESP message is sent by a GW to acknowledge the receipt and processing of an WILL- TOPICUPD message.
#define MQTTS_WILLMSGUPD    0x1C //!<  WILLMSGUPD message is sent by a client to update its Will message stored in the GW/broker.
#define MQTTS_WILLMSGRESP   0x1D //!<  WILLMSGRESP message is sent by a GW to acknowledge the receipt and processing of an WILLMSGUPD message.

// RESERVED -               0x1E - 0xFD

#define MQTTS_ENCAPSULATED_MSG     0xFE	

//  RESERVED -              0xFF

#define MQTTS_PROTOCOLID    0x01     			//!<  TODO - ???

#define MQTTS_NULL_TOPICID  0x0000
#define MQTTS_NULL_MSGID    0x0000

/*************************************************************************************
** MQTT-SN Spec: 5.3.4 Flags
** TopicIdType: indicates whether the field TopicId or TopicName included in this message contains a normal
** topic id (set to “0b00”), a pre-defined topic id (set to “0b01”), or a short topic name (set to “0b10”). The
** value “0b11” is reserved. Refer to sections 3 and 6.7 for the definition of the various types of topic ids.
*************************************************************************************/

#define MQTTS_DUP_FIRST             0x00 //!< message is sent for the first time
#define MQTTS_DUP_RETRANSMITT       0x01 //!< message is retransmitted

#define MQTTS_TID_TYPE_NORMAL       0x00 //!<  message contains a normal topic id (set to “0b00”)
#define MQTTS_TID_TYPE_PREDEFINED   0x01 //!<  a pre-defined topic id (set to “0b01”)
#define MQTTS_TID_TYPE_SHORT        0x02 //!<  short topic name (set to “0b10”)     
#define MQTTS_TID_TYPE_RESERVED     0x03 //!<  The value “0b11” is reserved

#define MQTTS_RADIUS_BROADCAST      0x00


/// \brief Return codes. The value and meaning of the 1-octet long ReturnCode field. MQTT-SN Spec: 5.3.10 ReturnCode
#define MQTTS_RC_ACCEPTED 0x00 					//!<  Accepted
#define MQTTS_RC_REJECTED_CONGESTION 0x01 		//!<  Rejected: congestion
#define MQTTS_RC_REJECTED_INVALID_TOPIC_ID 0x02 //!<  Rejected: invalid topic ID
#define MQTTS_RC_REJECTED_NOT_SUPPORTED 0x03    //!<  Rejected: not supported
//  RESERVED: 0x04 - 0xFF

//“Best practice” values for timers and counters
#define NUM_OF_PINGS_TO_RECONNECT 3 //!< number of ping message failure before reconection yo gateway
#define N_ADVERTISE 3
#define T_SEARCHGW 5000
#define T_WAIT 50000

#define DATA_FRAME_OFFSET 11 		//!< offset of data in reponse
#define SEND_BROADCAST 0 
#define SEND_NORMAL 1 


/// \brief WHAT IS THIS?
typedef union tagFLAGS_BITS
{
  struct
  {
    byte topicId:	2;	//!< indicates whether the field TopicId or TopicName included in this message
    byte cleanSession:	1;	//!< same meaning as with MQTT, however extended for Will topic and Will message
    byte will:		1;	//!< if set, indicates that client is asking for Will topic and Will message prompting
    byte retain:	1;	//!< same meaning as with MQTT
    byte qos:		2;	//!< set to “0b00” for QoS level 0, “0b01” for QoS level 1, “0b10” for QoS level 2, and “0b11” for new QoS level -1
    byte dup:		1;	//!< set to “0” if message is sent for the first time; set to “1” if retransmitted
  } bits;

  byte flags;   //!< TODO
} FLAGS_BITS;

//typedef byte FLAGS_BITS;

 /// The super class of all MQTTS commands 
 /// Users should never attempt to create an instance of this class; instead
 /// create an instance of a subclass
 /// It is recommend to reuse subclasses to conserve memory
class MQTTSCommand {
public:
    /// \brief Default constructor
    MQTTSCommand();

    /// \brief Default destructor
    virtual ~MQTTSCommand() {}

    /// \return command length
    virtual uint8_t getCommandLength();

    /// \return command length
    virtual uint8_t getCommandType() = 0;

	/// \brief encode command into byte buffer
	/// derived class should overide this and encode its own members
	virtual void encode(byte buffer[], unsigned int &length);

	/// \brief decode command from byte buffer
	/// derived class should overide this and decode its own members
	virtual void decode(byte buffer[], const unsigned int length);

	void encodeHeader(uint8_t type, byte buffer[], unsigned int length);

private:

};



/// \brief Represents ADVERTISE mqtts message
class MQTTSAdvertise : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSAdvertise();
	MQTTSAdvertise(uint8_t gwId, uint16_t duration);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_ADVERTISE; }

	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setGwId(uint8_t gwId);
	uint8_t getGwId();

	void setDuration(uint16_t duration);
	uint16_t getDuration();

private:
    uint8_t _gwId;		//!< the id of the gateway which sends this message.
    uint16_t _duration;		//!< time interval until the next ADVERTISE is broadcasted by this gateway.
};



/// \brief Represents SEARCHGW mqtts message
class MQTTSSearchGw : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSSearchGw();
	MQTTSSearchGw(uint8_t radius);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_SEARCHGW; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setRadius(uint8_t radius);
	uint8_t getRadius();

private:
    uint8_t _radius;		//!< the broadcast radius of this message.
};


/// \brief Represents GWINFO mqtts message
class MQTTSGwInfo : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSGwInfo();
	MQTTSGwInfo(uint8_t gwId, uint32_t slAdd, uint32_t shAdd);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_GWINFO; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);
	
	void setGwId(uint8_t gwId);
	uint8_t getGwId();

	void setGwSlAdd(uint32_t slAdd);
	uint32_t getGwSlAdd();

	void setGwShAdd(uint32_t shAdd);
	uint32_t getGwShAdd();

private:
    uint8_t 	_gwId;			//!< the id of a GW.
    uint32_t 	_slAdd;			//!< low 32 bits of xbee 64bit address
    uint32_t 	_shAdd;			//!< high 32 bits of xbee 64bit address
};

/// \brief Represents CONNECT mqtts message
class MQTTSConnect : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSConnect();
	MQTTSConnect(uint8_t flags, uint8_t protocolId, uint16_t duration, char *clientId);

	~MQTTSConnect() { if(_clientId) free(_clientId); }

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_CONNECT; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);
	
	void setFlags(uint8_t flags);
	FLAGS_BITS getFlags();

	void setProtocolId(uint8_t protocolId);
	uint8_t getProtocolId();	

	void setDuration(uint16_t duration);
	uint16_t getDuration();	

	void setClientId(const char* clientId);
	char* getClientId();

private:
	FLAGS_BITS _flags;
	uint8_t _protocolId;		//!< corresponds to the“ProtocolName” and “ProtocolVersion” of the MQTTCONNECT message.
	uint16_t _duration;			//!< same as with MQTT, contains the value of the Keep Alive timer.
	char *_clientId;	 		//!< same as with MQTT, contains the client id which is a 1-23 character long string which uniquely identifies the client to the broker. 
};

/// \brief Represents CONACK mqtts message
class MQTTSConAck : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSConAck();
	MQTTSConAck(uint8_t ReturnCode);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_CONNACK;}
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setReturnCode(uint8_t ReturnCode);
	uint8_t getReturnCode();

private:
    uint8_t _returnCode;
};

/*

/// \brief Represents WILLTOPICREQ mqtts message
class MQTTSWillTopicReq : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSWillTopicReq();
	uint8_t getCommandType() { return MQTTS_WILLTOPICREQ; }
};

*/

/// \brief Represents WILLTOPIC mqtts message
class MQTTSWillTopic : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSWillTopic();
	MQTTSWillTopic(uint8_t flags, const char *willTopic);

	~MQTTSWillTopic() { if(_willTopic) free(_willTopic); }

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_WILLTOPIC; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setWillTopic(const char *willTopic);
	char* getWillTopic();
	void setFlags(uint8_t flags);
	FLAGS_BITS getFlags();

private:
	FLAGS_BITS _flags;
	char *_willTopic;			//!< contains the Will topic name.								
};

/*

/// \brief Represents WILLMSGREQ mqtts message
class MQTTSWillMsgReq : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSWillMsgReq();
	uint8_t getCommandType() { return MQTTS_WILLMSGREQ; }
};

*/

/// \brief  Represents WILLMSG mqtts message
class MQTTSWillMsg : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSWillMsg();
	MQTTSWillMsg(const char *willMsg);

	~MQTTSWillMsg() { if(_willMsg) free(_willMsg); }

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_WILLMSG; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setWillMsg(const char *willMsg);
	char* getWillMsg();

private:
	char *_willMsg;			//!< contains the Will message.  									
};



/// \brief  Represents REGISTER mqtts message
class MQTTSRegister : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSRegister();
	MQTTSRegister(uint16_t topicId, uint16_t msgId, const char *topicName);

	~MQTTSRegister() { if(_topicName) free(_topicName); }

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_REGISTER; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setTopicId(uint16_t topicId);
	uint16_t getTopicId();

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

	void setTopicName(const char *topicName);
	char* getTopicName();

private:
	uint16_t _topicId;		//!< if sent by a client, it is coded 0x0000 and is not relevant; if sent by a GW, it contains the topic id
							//!< 	value assigned to the topic name included in the TopicName field;
	uint16_t _msgId;		//!< should be coded such that it can be used to identify the corresponding REGACK message.
	char *_topicName;		//!< contains the topic name. 										
};



/// \brief  Represents REGACK mqtts message
class MQTTSRegAck : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSRegAck();
	MQTTSRegAck(uint16_t topicId, uint16_t msgId, uint8_t returnCode);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_REGACK; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setTopicId(uint16_t topicId);
	uint16_t getTopicId();

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

	void setReturnCode(uint8_t returnCode);
	uint8_t getReturnCode();

private:
    uint16_t 	_topicId;         //!< the value that shall be used as topic id in the PUBLISH messages;
    uint16_t 	_msgId;           //!< same value as the one contained in the corresponding REGISTER message.
    uint8_t     _returnCode;      //!< “accepted’, or rejection reason.
};



/// \brief  Represents PUBLISH mqtts message
class MQTTSPublish : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSPublish();
	MQTTSPublish(uint8_t flags, uint16_t topicId, uint16_t msgId, char* data);

	~MQTTSPublish() { if(_data) free(_data); }

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_PUBLISH; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setFlags(uint8_t flags);
	FLAGS_BITS getFlags();

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

	void setTopicId(uint16_t topicId);
	uint16_t getTopicId();

	void setData(const char* data);
	char* getData();

private:
	FLAGS_BITS _flags;
	uint16_t _topicId;			//!< contains the topic id value or the short topic name for which the data is published.
	uint16_t _msgId;			//!< same meaning as the MQTT “Message ID”; only relevant in case of QoS levels 1 and 2, otherwise coded 0x0000.
	char *_data;				//!< the published data.				
};



/// \brief  Represents PUBACK mqtts message
class MQTTSPubAck : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSPubAck();
	MQTTSPubAck(uint16_t msgId, uint16_t topicId, uint8_t returnCode);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_PUBACK; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

	void setTopicId(uint16_t topicId);
	uint16_t getTopicId();

	void setReturnCode(uint8_t returnCode);
	uint8_t getReturnCode();

private:
    uint16_t _topicId;			//!< same value the one contained in the corresponding PUBLISH message.
    uint16_t _msgId;			//!< same value as the one contained in the corresponding PUBLISH message.
    uint8_t _returnCode;		//!< “accepted”, or rejection reason.
};

/*
/// \brief  Represents PUBREC mqtts message
class MQTTSPubRec : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSPubRec();
	MQTTSPubRec(uint16_t msgId);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_PUBREC; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

private:
    uint16_t _msgId;		//!< same value as the one contained in the corresponding PUBLISH message.
};


/// \brief  Represents PUBREL mqtts message
class MQTTSPubRel : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSPubRel();
	MQTTSPubRel(uint16_t msgId);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_PUBREL; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

private:
    uint16_t _msgId;		//!< same value as the one contained in the corresponding PUBLISH message.
};


/// \brief  Represents PUBCOMP mqtts message
class MQTTSPubComp : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSPubComp();
	MQTTSPubComp(uint16_t msgId);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_PUBCOMP; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

private:
    uint16_t _msgId;		//!< same value as the one contained in the corresponding PUBLISH message.
};
*/

/// \brief  Represents SUBSCRIBE mqtts message
class MQTTSSubscribe : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSSubscribe();
	MQTTSSubscribe(uint8_t flags, uint16_t msgId, char* topicName);

	~MQTTSSubscribe();

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_SUBSCRIBE; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setFlags(uint8_t flags);
	FLAGS_BITS getFlags();

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

	void setTopicName(const char* topicName);
	char* getTopicName();

private:
	FLAGS_BITS _flags;
	uint16_t _msgId;		//!< should be coded such that it can be used to identify the corresponding SUBACK message.
	//uint16_t _topicId;		//!< contains topic name, topic id, or short topic name as indicated in the TopicIdType field.
	char* _topicName;		//!< contains topic name, topic id, or short topic name as indicated in the TopicIdType field.
};


/// \brief  Represents SUBACK mqtts message
class MQTTSSubAck : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSSubAck();
	MQTTSSubAck(uint8_t flags, uint16_t msgId, uint16_t topicId, uint8_t returnCode);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_SUBACK; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setFlags(uint8_t flags);
	FLAGS_BITS getFlags();

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

	void setTopicId(uint16_t topicId);
	uint16_t getTopicId();

	void setReturnCode(uint8_t returnCode);
	uint8_t getReturnCode();

private:
	FLAGS_BITS _flags;
	uint16_t _topicId;		//!< in case of “accepted” the value that will be used as topicid by the gateway when sending PUBLISH messages to the client
	uint16_t _msgId;		//!< same value as the one contained in the corresponding SUBSCRIBE message.
	uint8_t _returnCode;	//!< “accepted”, or rejection reason.
};


/// \brief  Represents UNSUBSCRIBE mqtts message
class MQTTSUnsubscribe : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSUnsubscribe();
	MQTTSUnsubscribe(uint8_t flags, uint16_t msgId, uint16_t topicId);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_UNSUBSCRIBE; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setFlags(uint8_t flags);
	FLAGS_BITS getFlags();

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

	void setTopicId(uint16_t topicId);
	uint16_t getTopicId();

private:
	FLAGS_BITS _flags;
	uint16_t _msgId;		//!< should be coded such that it can be used to identify the corresponding SUBACK message.
	uint16_t _topicId;		//!< contains topic name, topic id, or short topic name as indicated in the TopicIdType field.
};


/// \brief  Represents UNSUBACK mqtts message
class MQTTSUnsubAck : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSUnsubAck();
	MQTTSUnsubAck(uint16_t msgId);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_UNSUBACK; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setMsgId(uint16_t msgId);
	uint16_t getMsgId();

private:
    uint16_t _msgId;		//!< same value as the one contained in the corresponding UNSUBSCRIBE message.
};


/// \brief  Represents PINGREQ mqtts message
class MQTTSPingReq : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSPingReq();
	MQTTSPingReq(char *clientId);

	~MQTTSPingReq(); // { if(_clientId) free(_clientId); }

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_PINGREQ; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setClientId(const char* clientId);
	char* getClientId();

private:
	char *_clientId;		//!< contains the client id; this field is optional and is included by a “sleeping” client when it goes to the 
							//!<	“awake” state and is waiting for messages sent by the broker/gateway, see Section 6.14 for further details. 												??? 
};


/// \brief  Represents PINGRESP mqtts message
class MQTTSPingResp : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSPingResp();
	uint8_t getCommandType() { return MQTTS_PINGRESP; }
};


/// \brief  Represents DISCONNECT mqtts message
class MQTTSDisconnect : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSDisconnect();
	MQTTSDisconnect(uint16_t duration);

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_DISCONNECT; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setDuration(uint16_t duration);
	uint16_t getDuration();

private:
    uint16_t _duration;		//!< contains the value of the sleep timer; this field is optional and is included by a “sleeping” client that wants to go the “asleep” state
};


/// \brief  Represents WILLTOPICUPD mqtts message
class MQTTSWillTopicUpd : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSWillTopicUpd();
	MQTTSWillTopicUpd(uint8_t flags, char* willTopic);

	~MQTTSWillTopicUpd() { if(_willTopic) free(_willTopic); }

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_WILLTOPICUPD; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setFlags(uint8_t flags);
	FLAGS_BITS getFlags();

	void setWillTopic(const char* willTopic);
	char* getWillTopic();

private:
	FLAGS_BITS _flags;
	char *_willTopic; 		//!< contains the Will topic name.										
};


/// \brief  Represents WILLTOPICRESP mqtts message
class MQTTSWillTopicResp : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSWillTopicResp();
	uint8_t getCommandType() { return MQTTS_WILLTOPICRESP; }
};


/// \brief  Represents WILLMSGUPD mqtts message
class MQTTSWillMsgUpd : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSWillMsgUpd();
	MQTTSWillMsgUpd(char* willMsg);

	~MQTTSWillMsgUpd() { if(_willMsg) free(_willMsg); }

	uint8_t getCommandLength();
	uint8_t getCommandType() { return MQTTS_WILLMSGUPD; }
	void encode(byte buffer[], unsigned int &length);
	void decode(byte buffer[], const unsigned int length);

	void setWillMsg(const char* willMsg);
	char* getWillMsg();

private:
    char *_willMsg; 		//!< contains the Will message.  										
};
		

/// \brief  Represents WILLMSGRESP mqtts message
class MQTTSWillMsgResp : public MQTTSCommand {
public:
	/// \brief Default constructor
	MQTTSWillMsgResp();
	uint8_t getCommandType() { return MQTTS_WILLMSGRESP; }
};



#endif //MQTTSN_H
