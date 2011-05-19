/*
 * MetaServerPacket.hpp
 *
 */

#include "MetaServerProtocol.hpp"

#ifndef METASERVERPACKET_HPP_
#define METASERVERPACKET_HPP_

#define MAX_PACKET_BYTES 1024

class MetaServerPacket
{

public:
	MetaServerPacket(boost::array<char,MAX_PACKET_BYTES>& pl, std::size_t bytes = 0 );
	~MetaServerPacket();

	NetMsgType getPacketType();
	void setPacketType(NetMsgType nmt);

	std::string getAddress();
	void setAddress(std::string address);

	unsigned int getPort();
	void setPort(unsigned int p);

	void addPacketData(unsigned int i);

	unsigned int getSize();

private:

	void parsePacketType();
	char *pack_uint32(uint32_t data, char *buffer);
	char *unpack_uint32(uint32_t *dest, char *buffer);



	NetMsgType m_packetType;
	std::string m_Address;
	unsigned int m_Port;
	std::size_t m_Bytes;
	char * m_currentPtr;
	boost::array<char,MAX_PACKET_BYTES> m_packetPayload;

};


#endif /* METASERVERPACKET_HPP_ */
