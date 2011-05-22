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
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>


#include "MetaServerPacket.hpp"

#ifndef __METASERVER_HPP__

#define __METASERVER_HPP__

#define foreach         		BOOST_FOREACH

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

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
	boost::asio::deadline_timer* expiry_timer_;
	boost::asio::deadline_timer* update_timer_;
	unsigned int expiry_timer_delay_milliseconds_;
	unsigned int update_timer_delay_milliseconds_;


};

#endif
