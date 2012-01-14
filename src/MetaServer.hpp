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

#ifndef METASERVER_HPP__
#define METASERVER_HPP__


/*
 * Local Includes
 */
#include "MetaServerPacket.hpp"
#include "DataObject.hpp"

/*
 * System Includes
 */
#include <boost/program_options.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/SimpleLayout.hh>

/*
 * Forward Declarations
 */
class PacketLogger;

#define foreach         		BOOST_FOREACH

//using boost::asio::ip::tcp;
//using boost::asio::ip::udp;

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
	void processCLIENTFILTER(MetaServerPacket& in, MetaServerPacket& out);

	void registerConfig( boost::program_options::variables_map & vm );
	void initLogger();

	log4cpp::Category& getLogger();

	bool isDaemon();

   private:

	DataObject msdo;
	unsigned int m_handshakeExpirySeconds;
	boost::asio::deadline_timer* m_expiryTimer;
	boost::asio::deadline_timer* m_updateTimer;
	unsigned int m_expiryDelayMilliseconds;
	unsigned int m_updateDelayMilliseconds;
	unsigned int m_sessionExpirySeconds;
	unsigned int m_clientExpirySeconds;
	unsigned int m_packetLoggingFlushSeconds;
	unsigned int m_maxServerSessions;
	unsigned int m_maxClientSessions;
	boost::posix_time::ptime m_startTime;
	bool m_keepServerStats;
	bool m_keepClientStats;
	bool m_logServerSessions;
	bool m_logClientSessions;
	bool m_logPackets;
	bool m_isDaemon;
	unsigned long long m_PacketSequence;
	PacketLogger* m_PacketLogger;
	std::string m_Logfile;
	std::string m_PacketLogfile;
	log4cpp::Category& m_Logger;
	log4cpp::Appender* m_LogAppender;
	log4cpp::Layout* m_LoggerLayout;

};

#endif
