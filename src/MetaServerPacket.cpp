/*
 * MetaServerPacket.cpp
 *
 *  Created on: 2011-05-17
 *      Author: sryan
 */

#include "MetaServer.hpp"
#include "MetaServerPacket.hpp"

MetaServerPacket::MetaServerPacket(boost::array<char,MAX_PACKET_BYTES>& pl, std::size_t bytes )
		: m_packetPayload(pl),
		  m_Bytes(bytes),
		  m_packetType(NMT_NULL),
		  m_Port(0),
		  m_Address("")

{
		/**
		 * @note exception risk in ctor ... re-think
		 */
		m_readPtr  = m_packetPayload.data();
		m_headPtr  = m_readPtr;
		m_writePtr = m_packetPayload.c_array();
		parsePacketType();
}

MetaServerPacket::~MetaServerPacket()
{

}

NetMsgType
MetaServerPacket::getPacketType()
{
	return m_packetType;
}

void
MetaServerPacket::setPacketType(NetMsgType nmt)
{

	/**
	 *
	 */
	m_packetPayload.assign(0);
	m_readPtr = m_packetPayload.data();
	m_writePtr = m_packetPayload.c_array();
	m_Bytes = 0;

	// write must occur prior to read
	m_writePtr = pack_uint32(nmt);
	parsePacketType();

}


std::string
MetaServerPacket::getAddress()
{
	return m_Address;
}

void
MetaServerPacket::setAddress(std::string address)
{
	m_Address = address;
}

unsigned int
MetaServerPacket::getPort()
{
	return m_Port;
}

void
MetaServerPacket::setPort(unsigned int p)
{
	m_Port = p;
}

void
MetaServerPacket::addPacketData(uint32_t i)
{
	m_writePtr = pack_uint32(i);
}

void
MetaServerPacket::addPacketData(std::string s)
{
	/**
	 * TODO: add bounds checking
	 */
	m_writePtr = pack_string( s.data(), s.length() );
}

unsigned int
MetaServerPacket::getSize()
{
	return m_Bytes;
}

std::string
MetaServerPacket::getPacketMessage()
{
	std::string foo = "";
	unsigned int msgSize = (m_readPtr-m_headPtr);

	if ( msgSize > 0 ) {
		m_readPtr = unpack_string(&foo, msgSize );
	}
	return foo;
}

uint32_t
MetaServerPacket::getIntData()
{
	uint32_t foo = 99;
	m_readPtr = unpack_uint32(&foo);
	return foo;
}

void
MetaServerPacket::dumpBuffer()
{

}

/**
 * Pulls out the first byte of a packet, which universally indicates the packet type.
 * For empty packets ( ie newly created for outbound )
 * @return NetMsgType
 */
void
MetaServerPacket::parsePacketType()
{
	if ( m_Bytes > 0 )
	{
		m_readPtr = m_packetPayload.data();
		m_packetType = getIntData();
	}
}

/**
 *
 * Not sure if read only accessor is same address space or not
 *
 * @param data - src int
 * @param buffer - dest pointer
 * @return
 */
char*
MetaServerPacket::pack_uint32(uint32_t data)
{
    uint32_t netorder;

    netorder = htonl(data);
    memcpy(m_writePtr, &netorder, sizeof(uint32_t));
    m_Bytes += sizeof(uint32_t);

    return m_writePtr+sizeof(uint32_t);
}

char*
MetaServerPacket::unpack_uint32(uint32_t *dest)
{
    uint32_t netorder;

    memcpy(&netorder, m_readPtr, sizeof(uint32_t));
    *dest = ntohl(netorder);
    return m_readPtr+sizeof(uint32_t);

}

char*
MetaServerPacket::pack_string( const char * str, unsigned int length )
{
	memcpy(m_writePtr, str, length );
	m_Bytes += length;
	return m_writePtr+length;
}

char *
MetaServerPacket::unpack_string(std::string *dest, unsigned int length )
{
	std::string s(m_readPtr,length);
	*dest = s;
	return m_readPtr+length;
}


