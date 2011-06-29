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
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 */

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <typeinfo>
#include <map>
#include <queue>
#include <algorithm>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/SimpleLayout.hh>


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
	void processTERMINATE(MetaServerPacket& in, MetaServerPacket& out);
	void processCLIENTKEEPALIVE(MetaServerPacket& in, MetaServerPacket& out);
	void processCLIENTSHAKE(MetaServerPacket& in, MetaServerPacket& out);
	void processLISTREQ(MetaServerPacket& in, MetaServerPacket& out);
	void processSERVERATTR(MetaServerPacket& in, MetaServerPacket& out);
	void processCLIENTATTR(MetaServerPacket& in, MetaServerPacket& out);

	uint32_t addHandshake();
	void removeHandshake(unsigned int hs);

	void addServerAttribute(std::string sessionid, std::string name, std::string value );
	void removeServerAttribute(std::string sessionid, std::string name );
	void addClientAttribute(std::string sessionid, std::string name, std::string value );
	void removeClientAttribute(std::string sessionid, std::string name );


	void addServerSession(std::string sessionid);
	void removeServerSession(std::string sessionid);
	void addClientSession(std::string sessionid);
	void removeClientSession(std::string sessionid);

	void registerConfig( boost::program_options::variables_map & vm );
	void initLogger();

	log4cpp::Category& getLogger();

	void dumpHandshake();
	boost::posix_time::ptime getNow();

	bool isDaemon();

   private:
	/**
	 *  Example Data Structure ( m_serverData )
	 *  "192.168.1.200" => {
	 *  	"serverVersion" => "0.5.20",
	 *  	"serverType" => "cyphesis",
	 *  	"serverUsers" => "100",
	 *  	"attribute1" => "value1",
	 *  	"attribute2" => "value2"
	 *  }
	 *
	 *  m_serverDataList contains an ordered representation of
	 *  m_serverData keys so that multiple LISTREQ requests can be
	 *  done and avoid duplicate servers packet responses.
	 */
	std::map<std::string, std::map<std::string,std::string> > m_serverData;
	std::list<std::string> m_serverDataList;

	std::map<std::string, std::map<std::string,std::string> > m_clientData;
	std::map<unsigned int,std::map<std::string,std::string> > m_handshakeQueue;
	unsigned int m_handshakeExpirySeconds;
	boost::asio::deadline_timer* m_expiryTimer;
	boost::asio::deadline_timer* m_updateTimer;
	unsigned int m_expiryDelayMilliseconds;
	unsigned int m_updateDelayMilliseconds;
	unsigned int m_sessionExpirySeconds;
	unsigned int m_clientExpirySeconds;
	unsigned int m_maxServerSessions;
	unsigned int m_maxClientSessions;
	bool m_keepServerStats;
	bool m_keepClientStats;
	bool m_logServerSessions;
	bool m_logClientSessions;
	bool m_isDaemon;
	std::string m_Logfile;
	log4cpp::Category& m_Logger;
	log4cpp::Appender* m_LogAppender;
	log4cpp::Layout* m_LoggerLayout;

};

#endif
