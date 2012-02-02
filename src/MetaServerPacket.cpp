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


#include "MetaServerPacket.hpp"

MetaServerPacket::MetaServerPacket()
		: m_packetPayload( *new boost::array<char,MAX_PACKET_BYTES>() ),
		  m_Bytes(0),
		  m_packetType(NMT_NULL),
		  m_Port(0),
		  m_AddressInt(0),
		  m_needFree(true),
		  m_Sequence(0),
		  m_TimeOffset(0)
{
	m_readPtr  = m_packetPayload.data();
	m_headPtr  = m_packetPayload.data();
	m_writePtr = m_packetPayload.c_array();
	m_packetPayload.assign(0);
}

MetaServerPacket::MetaServerPacket(boost::array<char,MAX_PACKET_BYTES>& pl, std::size_t bytes )
		: m_packetPayload(pl),
		  m_Bytes(bytes),
		  m_packetType(NMT_NULL),
		  m_Port(0),
		  m_AddressInt(0),
		  m_needFree(false),
		  m_Sequence(0),
		  m_TimeOffset(0)
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
	// delete [] m_packetPayload causes a core
}

void
MetaServerPacket::setPacketType(const NetMsgType& nmt)
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

void
MetaServerPacket::setAddress(const boost::asio::ip::address& address)
{

	/*
	 *   NOTE: this is a compat problem, as it is basically a hack
	 *   to make everything logic wise ( session setting, etc ) all
	 *   look like it's from ipv4.  This should work fine for aliased ipv4 addresses
	 *   but for actually ipv6<->ipv6 communication, this could potentially fall down.
	 *
	 *   For this reason ... the internal address here should be considered
	 *   cosmetic.
	 */
	if ( address.is_v4() )
	{
		m_Address = address;
		m_AddressStr = m_Address.to_string();
		m_AddressInt = IpAsciiToNet( m_AddressStr.data() );

	}
	else
	{
		/*
		 *  NOTE: this will ONLY work on a mapped address, a real
		 *  ipv6 address will return a hex quad rather than a
		 *  dotted quad.
		 */

		std::string s = address.to_string();

		m_Address = address;
		m_AddressStr = s.substr(s.find_last_of(':') + 1) ;
		m_AddressInt = IpAsciiToNet( m_AddressStr.data() );
	}

}

unsigned int
MetaServerPacket::addPacketData(boost::uint32_t i)
{
	unsigned int ret_off = m_writePtr - m_headPtr;
	m_writePtr = pack_uint32(i,m_writePtr);
	return ret_off;
}

unsigned int
MetaServerPacket::addPacketData(const std::string& s)
{
	unsigned int ret_off = m_writePtr - m_headPtr;
	m_writePtr = pack_string( s , m_writePtr );
	return ret_off;
}

const std::string
MetaServerPacket::getPacketMessage(unsigned int offset) const
{
	std::string foo = "";
	unpack_string(&foo, m_headPtr + offset , (m_Bytes - offset) );
	return foo;
}

/*
 * This is the original metaserver way
 * This ... is stupid IMO, metaserver expects from
 * 127.0.2.1
 *
 * String value	1.2.0.127
   Binary	00000001 . 00000010 . 00000000 . 01111111
   Integer	16908415
 */
boost::uint32_t
MetaServerPacket::IpAsciiToNet(const char *buffer) {

  boost::uint32_t ret = 0;
  int shift = 0;  //  fill out the MSB first
  bool startQuad = true;
  while ((shift <= 24) && (*buffer)) {
    if (startQuad) {
      unsigned char quad = (unsigned char) atoi(buffer);
      ret |= (((boost::uint32_t)quad) << shift);
      shift += 8;
    }
    startQuad = (*buffer == '.');
    ++buffer;
  }
  return ret;
}

/*  This is the correct way to do things.
 *  ex:
 *  String value	127.0.2.1
	Binary	01111111 . 00000000 . 00000010 . 00000001
	Integer	2130706945
boost::uint32_t
MetaServerPacket::IpAsciiToNet(const char *buffer) {

  boost::uint32_t ret = 0;
  int shift = 24;  //  fill out the MSB first
  bool startQuad = true;
  while ((shift >= 0) && (*buffer)) {
    if (startQuad) {
      unsigned char quad = (unsigned char) atoi(buffer);
      ret |= (((boost::uint32_t)quad) << shift);
      shift -= 8;
    }
    startQuad = (*buffer == '.');
    ++buffer;
  }
  return ret;
}
*/


std::string
MetaServerPacket::IpNetToAscii(boost::uint32_t address) {
  const int sizer = 15;
  char ip_buffer[20];

   /**
    *  This is the "correct way" of doing things

   snprintf(ip_buffer, sizer, "%u.%u.%u.%u", (address>>24)&0xFF,
        (address>>16)&0xFF, (address>>8)&0xFF, (address>>0)&0xFF);
	*/

   /**
    *  This is the old ms way ...
    */
   snprintf(ip_buffer, sizer, "%u.%u.%u.%u", (address>>0)&0xFF,
       (address>>8)&0xFF, (address>>16)&0xFF, (address>>24)&0xFF);

   return ( std::string(ip_buffer));
}


/**
 *  Stream Insertion Operator
 *  Purpose: mostly for binary packet logging ... but in theory should work for textual
 *           output of the packet
 */
std::ostream& operator<<( std::ostream &os, const MetaServerPacket &mp)
{
	/*
	 * Output Format:
	 *
	 * Sequence:TimeOffset:Bytes of Data:Full RawBuffer
	 *
	 * NOTE: It is tempting to write more information, but this is not required.
	 * 	Packet Type: not required as it is parsed from the RawBuffer
	 * 	IP Address(s): not required, because they can't be used anyway.  Any replay
	 * 	               will contain the current IP address pairs.
	 *
	 * When Reading back the packet, sequence of read is:
	 *      - read packet sequence
	 *      - read time offset
	 *      - read buffer size
	 *      - read buffer
	 *      - create msp(buffer,size) [ this triggers a parsePacket, which is critical ]
	 *      - set dest address
	 *      - set dest port
	 *      - packet can now be sent.
	 *
	 *		     sequence             offset              size             raw_buffer
	 *       (unsigned long long) (unsigned long long) (std::size_t)  (boost::array<char,MAX_PACKET_BYTES>)
	 *
	 *		NOTE: size refers to the number of bytes received ( m_Bytes )
	 */

	 /*
	  * Sequence
	  */
     unsigned long long s = mp.getSequence();
	 os.write( (char*)&s, sizeof(unsigned long long) );

	 /*
	  * Time Offset
	  */
	 unsigned long long o = mp.getTimeOffset();
	 os.write( (char *)&o , sizeof(unsigned long long) );

	 /*
	  * Size of RECV buffer
	  */
	 std::size_t t = mp.getSize();
	 os.write( (char *)&t, sizeof(std::size_t) );


	/*
	 * Write the full MAX_PACKET_BYTES buffer size
	 */
	os.write( mp.getBuffer().c_array(), sizeof(char)*(MAX_PACKET_BYTES) );
	return os;
}

std::istream & operator>>( std::istream& is, MetaServerPacket &mp )
{

	unsigned long long seq;
	unsigned long long off;
	std::size_t size;
	boost::array<char,MAX_PACKET_BYTES> buf;

	/*
	 * Sequence
	 * Note: Read pointers are advanced automatically by is.read()
	 */
	is.read( (char *)&seq, sizeof(unsigned long long) );

	/*
	 * Time Offset
	 */
	is.read( (char *)&off, sizeof(unsigned long long) );

	/*
	 * Size
	 */
	is.read( (char *)&size, sizeof(std::size_t) );

	/*
	 * Raw Buffer
	 */
	is.read( buf.c_array(), sizeof(char)*(MAX_PACKET_BYTES) );

	mp.setBuffer(buf,size);
	mp.setSequence(seq);
	mp.setTimeOffset(off);
	mp.parsePacketType();

	return is;

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
MetaServerPacket::unpack_uint32(uint32_t *dest, char *buffer) const
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
MetaServerPacket::unpack_string(std::string *dest, char* buffer, unsigned int length ) const
{
	std::string s(buffer,length);
	*dest = s;
	return buffer+length;
}
