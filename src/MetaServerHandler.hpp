/**
 * MetaServerHandler.hpp
 *
 * @author Sean Ryan <sryan@evercrack.com>
 */

#include "MetaServer.hpp"

#ifndef METASERVERHANDLER_HPP_
#define METASERVERHANDLER_HPP_

#define MAX_PACKET_BYTES 1024

class MetaServerHandler
{
public:
	virtual ~MetaServerHandler() {}

	static unsigned int parsePacketType( boost::array<unsigned char, MAX_PACKET_BYTES> & byte_array );

private:
	int i;

};

#endif /* METASERVERHANDLER_HPP_ */
