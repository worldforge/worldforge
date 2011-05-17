#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>

#include "MetaServerProtocol.hpp"

#ifndef __METASERVER_HPP__

#define __METASERVER_HPP__

#define foreach         		BOOST_FOREACH
#define MAX_PACKET_BYTES 1024

/**
 * Convenience typedefs
 */
typedef std::pair<std::string,std::string> TStringStringPair;

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class MetaServerPacket
{

public:
	MetaServerPacket(boost::array<unsigned char,MAX_PACKET_BYTES>& pl, std::size_t bytes = 0 )
		: packetPayload_(pl),
		  bytes_(bytes)
	{
		/**
		 * @note exception risk in ctor ... re-think
		 */
		packetType_ = parsePacketType();
	}
	NetMsgType getPacketType()
	{
		return packetType_;
	}
	void setPacketType(NetMsgType nmt)
	{
		packetType_ = nmt;
		setPacketData<NetMsgType>(0,nmt);
	}
	std::string getAddress()
	{
		return(address_);
	}
	void setAddress(std::string address)
	{
		address_ = address;
	}
	unsigned int getSize()
	{
		return( bytes_ );
	}
	/**
	 * Pulls out the first byte of a packet, which universally indicates the packet type
	 * @return
	 */
	NetMsgType parsePacketType()
	{
		/* The boost::array.at provides bounds checking, this is used so that an empty
		 * packet can be created to be sent.
		 */

		if ( packetPayload_.size() < bytes_ )
			return NMT_NULL;

		/**
		 * TODO: change, as an "int" may not always be 4 bytes
		 */
		unsigned int nmt_width = sizeof(NetMsgType);
		char rawTypeBytes[nmt_width];

		for (unsigned int i = 0; i<nmt_width; ++i)
			rawTypeBytes[i] = packetPayload_.at(i);

		NetMsgType rawType = NMT_NULL;

		// Snarf data from buffer
		memcpy( (void *)&rawType, rawTypeBytes , sizeof(rawType) );

		// convert back from network byte order
		return ( (NetMsgType)htonl(rawType) );
	}

	/**
	 * This will require some kind of careful protocol knowledge
	 * @param index
	 * @param bytes
	 * @return
	 */
	template<class T> T getPacketData(unsigned int offset, unsigned int bytes)
	{
		T resp;

		// grab c style array via ptr
		unsigned char* ptr = packetPayload_.data();

		/**
		 * @note dodgy
		 */
		ptr+=offset;

		memcpy( (void *)&resp, ptr , bytes );

		return( (T)htonl(resp));

	}

	template <class T> void setPacketData(unsigned int offset, const T& datum)
	{
		unsigned char* ptr = packetPayload_.c_array();

		/**
		 * @note dodgy
		 * @note think about placing a lock on payload, although msp is short lived
		 *
		 * move the pointer offset appropriately, and memcpy the value in
		 */
		ptr+=offset;

		T d = (T)htonl(datum);

		memcpy( (void *)ptr, &d, sizeof(T) );

		bytes_+=sizeof(T);

	}

private:
	NetMsgType packetType_;
	std::string address_;
	std::size_t bytes_;
	boost::array<unsigned char,MAX_PACKET_BYTES> packetPayload_;

};

class MetaServer
{
   public:
	MetaServer(boost::asio::io_service& ios);
	~MetaServer();
	void expiry_timer(const boost::system::error_code& error);
	void update_timer(const boost::system::error_code& error);
	void processMetaserverPacket(MetaServerPacket& msp, MetaServerPacket& rsp);
	void processSERVERKEEPALIVE(MetaServerPacket& in, MetaServerPacket& out);
	int addHandshake(unsigned int hs, std::map<std::string,std::string> attr);
	void dumpHandshake();


   private:
	/**
	 *  Example Data Structure ( ms_data_ )
	 *  "192.168.1.200" => {
	 *  	"serverVersion" => "0.5.20",
	 *  	"serverType" => "cyphesis",
	 *  	"serverUsers" => "100",
	 *  	"attribute1" => "value1",
	 *  	"attribute2" => "value2"
	 *  }
	 */
	std::map<std::string, std::map<std::string,std::string> > ms_data_;
	std::map<unsigned int,std::map<std::string,std::string> > handshake_queue_;
	std::map<unsigned int,std::map<std::string,std::string> > *hs_ptr_;
	boost::asio::deadline_timer* expiry_timer_;
	boost::asio::deadline_timer* update_timer_;
	unsigned int expiry_timer_delay_milliseconds_;
	unsigned int update_timer_delay_milliseconds_;


};

#endif
