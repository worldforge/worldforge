/**
 * MetaServerHandler.cpp
 *
 * @author Sean Ryan <sryan@evercrack.com>
 */

#include "MetaServerHandler.hpp"

/**
 *
 * @param byte_array
 *
 * @TODO Add error checking
 */
unsigned int
MetaServerHandler::parsePacketType(boost::array<unsigned char, MAX_PACKET_BYTES> & byte_array)
{
	/**
	 * First 4 bytes are the Packet Type, discard if there is not enough data
	 */
	if ( byte_array.size() < 4 )
		return 0;

	char rawTypeBytes[4];
	     rawTypeBytes[0] = byte_array.at(0);
	     rawTypeBytes[1] = byte_array.at(1);
	     rawTypeBytes[2] = byte_array.at(2);
	     rawTypeBytes[3] = byte_array.at(3);

	int rawType = 0;

	// Snarf data from buffer
	memcpy( (void *)&rawType, rawTypeBytes , sizeof(rawType) );

	// convert back from network byte order
	return ( htonl(rawType) );
}

