#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <map>
#include <queue>
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
	void processSERVERSHAKE(MetaServerPacket& in, MetaServerPacket& out);
	int addHandshake(unsigned int hs, std::map<std::string,std::string> attr);
	void removeHandshake(unsigned int hs);
	void addServerAttribute(std::string sessionid, std::string name, std::string value );
	void removeServerAttribute(std::string sessionid, std::string name );
	void addServerSession(std::string sessionid);
	void removeServerSession(std::string sessionid);
	void dumpHandshake();
	boost::posix_time::ptime getNow();


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
	std::map<std::string, std::map<std::string,std::string> > m_serverData;
	std::map<std::string, std::map<std::string,std::string> > m_clientData;
	std::map<unsigned int,std::map<std::string,std::string> > m_handshakeQueue;
	unsigned int m_handshakeExpirySeconds;
	boost::asio::deadline_timer* m_expiryTimer;
	boost::asio::deadline_timer* m_updateTimer;
	unsigned int m_expiryDelayMilliseconds;
	unsigned int m_updateDelayMilliseconds;
	unsigned int m_sessionExpirySeconds;


};

#endif
