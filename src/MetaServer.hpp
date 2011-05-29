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
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
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

	void dumpHandshake();
	boost::posix_time::ptime getNow();


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


};

#endif
