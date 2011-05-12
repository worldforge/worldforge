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
		  m_packetType(NMT_NULL)
{
		/**
		 * @note exception risk in ctor ... re-think
		 */
		m_currentPtr = m_packetPayload.data();
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
	m_currentPtr = m_packetPayload.data();
	m_Bytes = 0;

	/*
	 * Grab read/write pointer for update
	 */
	char * ptr = m_packetPayload.c_array();

	m_currentPtr = pack_uint32(m_packetType,ptr);

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

void
MetaServerPacket::addPacketData(unsigned int i)
{
	char * ptr = m_packetPayload.c_array();
	m_currentPtr = pack_uint32(i,ptr);
}

unsigned int
MetaServerPacket::getSize()
{
	return( m_Bytes );
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
		m_currentPtr = unpack_uint32(&m_packetType,m_currentPtr);
}

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
