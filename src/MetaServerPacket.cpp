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
		m_readPtr  = m_packetPayload.data();
		m_headPtr  = m_packetPayload.data();
		m_writePtr = m_packetPayload.c_array();

		if ( bytes > 0 )
		{
			// if we have data ... parse out type
			parsePacketType();
		}
		else
		{
			// otherwise assume a construction and zero it out
			m_packetPayload.assign(0);
		}
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
	m_writePtr = pack_uint32(nmt, m_writePtr);
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

unsigned int
MetaServerPacket::addPacketData(uint32_t i)
{
	unsigned int ret_off = m_writePtr - m_headPtr;
	m_writePtr = pack_uint32(i,m_writePtr);
	return ret_off;
}

unsigned int
MetaServerPacket::addPacketData(std::string s)
{
	unsigned int ret_off = m_writePtr - m_headPtr;
	m_writePtr = pack_string( s , m_writePtr );
	return ret_off;
}

unsigned int
MetaServerPacket::getSize()
{
	return m_Bytes;
}

std::string
MetaServerPacket::getPacketMessage(unsigned int offset)
{
	std::string foo = "";

	m_readPtr = m_headPtr + offset;
	m_readPtr = unpack_string(&foo, m_readPtr, (m_Bytes - offset) );

	return foo;
}

uint32_t
MetaServerPacket::getIntData(unsigned int offset)
{
	uint32_t foo = 99;

	m_readPtr = m_headPtr + offset;
	m_readPtr = unpack_uint32(&foo, m_readPtr );

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
		m_packetType = getIntData(0);
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
MetaServerPacket::pack_uint32(uint32_t data, char *buffer)
{
    uint32_t netorder;

    netorder = htonl(data);

    memcpy(buffer, &netorder, sizeof(uint32_t));
    m_Bytes += sizeof(uint32_t);

    return buffer+sizeof(uint32_t);

}


char*
MetaServerPacket::unpack_uint32(uint32_t *dest, char *buffer)
{
    uint32_t netorder;

    memcpy(&netorder, buffer, sizeof(uint32_t));
    *dest = ntohl(netorder);
    return buffer+sizeof(uint32_t);

}


char*
MetaServerPacket::pack_string( std::string str, char *buffer )
{
	unsigned int ss = str.size();
	memcpy(buffer, str.data() , ss );
	m_Bytes += ss;
	return buffer+ss;
}

char *
MetaServerPacket::unpack_string(std::string *dest, char* buffer, unsigned int length )
{
	std::string s(buffer,length);
	*dest = s;
	return buffer+length;
}


