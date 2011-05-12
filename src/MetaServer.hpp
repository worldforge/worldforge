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

#include "MetaServerProtocol.hpp"

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
	void tick(const boost::system::error_code& error);


   private:
	/**
	 *  Example Data Structure
	 *  "192.168.1.200" => {
	 *  	"serverVersion" => "0.5.20",
	 *  	"serverType" => "cyphesis",
	 *  	"serverUsers" => "100",
	 *  	"attribute1" => "value1",
	 *  	"attribute2" => "value2"
	 *  }
	 */
	std::map<std::string, std::map<std::string,std::string> > ms_data_;
	std::map<int,std::string> handshake_queue_;
	boost::asio::deadline_timer* ticker_;
	int tick_delay_milliseconds_;


};

#endif
