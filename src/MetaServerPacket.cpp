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
		  m_Port(0)
{
		/**
		 * @note exception risk in ctor ... re-think
		 */
		m_readPtr = m_packetPayload.data();
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

	/*
	 * Convenience to just set member variable for getter
	 */
	m_packetType = nmt;


	/**
	 * Essentially resets read-only pointer and size
	 * @note Should never be called on a packet you want to get data from, and
	 * is essentially a "reset everything"
	 */
	m_packetPayload.assign(0);
	m_readPtr = m_packetPayload.data();
	m_writePtr = m_packetPayload.c_array();
	m_Bytes = 0;

	pack_uint32(m_packetType);

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
	/**
	 * Need to get the read/write accessor and increment by current size
	 */
	m_writePtr = pack_uint32(i);

}

unsigned int
MetaServerPacket::getSize()
{
	return m_Bytes;
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
	for ( int i = 0; i< m_Bytes; ++i )
	{
		std::cout << "      : Payload-" << i << " [" << m_packetPayload.at(i) << "]" << std::endl;
	}
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
