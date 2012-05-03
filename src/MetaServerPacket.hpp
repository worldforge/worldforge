/**
 Worldforge Next Generation MetaServer

 Copyright (C) 2011 Sean Ryan <sryan@evercrack.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 */

/*
 * Local Includes
 */
#include "MetaServerProtocol.hpp"

/*
 * System Includes
 */
#include <iosfwd>
#include <boost/asio/ip/address.hpp>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>



#ifndef METASERVERPACKET_HPP_
#define METASERVERPACKET_HPP_

#define MAX_PACKET_BYTES 1024
#define MAX_UDP_OUT_BYTES 570

class MetaServerPacket
{

public:
	MetaServerPacket();
	MetaServerPacket(boost::array<char,MAX_PACKET_BYTES>& pl, std::size_t bytes = 0 );
	~MetaServerPacket();

	const NetMsgType getPacketType() const { return m_packetType; }
	void setPacketType(const NetMsgType& nmt);

	const std::string getAddressStr() const { return m_AddressStr; }
	const boost::uint32_t getAddressInt() const { return m_AddressInt; }
	const boost::asio::ip::address getAddress() const { return m_Address; }
	void setAddress(const boost::asio::ip::address& address);

	void setSequence(const unsigned long long seq = 0) { m_Sequence = seq; }
	unsigned long long getSequence() const { return m_Sequence; }

	void setTimeOffset(const unsigned long long to = 0 ) { m_TimeOffset = to; }
	unsigned long long getTimeOffset() const { return m_TimeOffset; }

	unsigned int getPort() const { return m_Port; }
	void setPort(unsigned int p) { m_Port = p; }

	bool getOutBound() { return m_outBound; }
	void setOutBound(bool s) { m_outBound = s; }

	std::size_t getSize() const { return m_Bytes; }

	unsigned int addPacketData(boost::uint32_t i);
	unsigned int addPacketData(const std::string& s);

	const std::string getPacketMessage(unsigned int offset) const;
	uint32_t getIntData(unsigned int offset) const
	{
		uint32_t foo = 99;
//		printf("getIntData-1: %d\n", foo );
		unpack_uint32(&foo, m_readPtr + offset );
//		printf("getIntData-2: %d\n", foo );
		return foo;
	}

	boost::uint32_t	IpAsciiToNet(const char *buffer);
	std::string IpNetToAscii(boost::uint32_t address);

	boost::array<char,MAX_PACKET_BYTES>& getBuffer() const { return m_packetPayload; }
	void setBuffer( boost::array<char,MAX_PACKET_BYTES>& pl, std::size_t bytes = 0 )
	{
		m_packetPayload = pl;
		m_Bytes = bytes;
	}

	/*
	 * Stream Overloads
	 */
	friend std::ostream & operator<<( std::ostream& os, const MetaServerPacket &mp);
	friend std::istream & operator>>( std::istream& is, MetaServerPacket &mp );

	void parsePacketType();


private:


	char *pack_uint32(uint32_t data, char* buffer );
	char *unpack_uint32(uint32_t *dest, char* buffer ) const;
	char *pack_string(std::string str, char *buffer );
	char *unpack_string(std::string *dest, char* buffer, unsigned int length ) const;

	NetMsgType m_packetType;
	boost::asio::ip::address m_Address;
	boost::uint32_t m_AddressInt;
	std::string m_AddressStr;
	unsigned int m_Port;
	std::size_t m_Bytes;
	char * m_headPtr;
	char * m_writePtr;
	char * m_readPtr;
	boost::array<char,MAX_PACKET_BYTES>& m_packetPayload;
	bool m_needFree;
	bool m_outBound;
	unsigned long long m_Sequence;
	unsigned long long m_TimeOffset;

};

/*
 *
 */
std::ostream & operator<<(std::ostream &os, const MetaServerPacket &mp);

#endif /* METASERVERPACKET_HPP_ */
