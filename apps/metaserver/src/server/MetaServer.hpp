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

#ifndef METASERVER_HPP
#define METASERVER_HPP


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
#include <boost/asio/io_context.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <filesystem>

#include <set>
#include <random>


/*
 * Magic Definitions
 */
const char DEFAULT_CONF[] = SYSCONFDIR;

class MetaServer {
public:
	MetaServer();

	~MetaServer();

	void expiry_timer(const boost::system::error_code& error);

	void update_timer(const boost::system::error_code& error);

	void score_timer(const boost::system::error_code& error);

	void processMetaserverPacket(MetaServerPacket& msp, MetaServerPacket& rsp);

	void processSERVERKEEPALIVE(const MetaServerPacket& in, MetaServerPacket& out);

	void processSERVERSHAKE(const MetaServerPacket& in, MetaServerPacket& out);

	void processTERMINATE(const MetaServerPacket& in, MetaServerPacket& out);

	void processCLIENTKEEPALIVE(const MetaServerPacket& in, MetaServerPacket& out);

	void processCLIENTSHAKE(const MetaServerPacket& in, MetaServerPacket& out);

	void processLISTREQ(const MetaServerPacket& in, MetaServerPacket& out);

	void processSERVERATTR(const MetaServerPacket& in, MetaServerPacket& out);

	void processCLIENTATTR(const MetaServerPacket& in, MetaServerPacket& out);

	void processCLIENTFILTER(const MetaServerPacket& in, MetaServerPacket& out);

	void processDNSREQ(const MetaServerPacket& in, MetaServerPacket& out);

	void processADMINREQ(const MetaServerPacket& in, MetaServerPacket& out);

	void registerConfig(boost::program_options::variables_map& vm);

	void initLogger();

	void initTimers(boost::asio::io_context& ios);

	std::string getLogFile() { return m_Logfile; }

	bool isShutdown() const { return m_isShutdown; }

	unsigned long long getDeltaMillis();

	bool isDaemon() const;

	void shutDown() { m_isShutdown = true; }

	void getMSStats(std::map<std::string, std::string>& req_stats);


private:

	DataObject msdo;
	unsigned int m_handshakeExpirySeconds;
	std::unique_ptr<boost::asio::deadline_timer> m_expiryTimer;
	std::unique_ptr<boost::asio::deadline_timer> m_updateTimer;
	std::unique_ptr<boost::asio::deadline_timer> m_scoreTimer;
	unsigned int m_expiryDelayMilliseconds;
	unsigned int m_updateDelayMilliseconds;
	unsigned int m_scoreDelayMilliseconds;
	unsigned int m_sessionExpirySeconds;
	unsigned int m_clientExpirySeconds;
	unsigned int m_packetLoggingFlushSeconds;
	unsigned int m_loggingFlushSeconds;
	boost::posix_time::ptime m_loggingFlushTime;
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
	std::string m_Logfile;
	std::string m_PacketLogfile;
	bool m_isShutdown;
	std::filesystem::path m_pidFile;
	std::filesystem::path m_scoreServer;
	std::filesystem::path m_scoreClient;
	std::filesystem::path m_scoreStats;
	std::filesystem::path m_scoreCCache;
	bool m_logPacketAllow;
	bool m_logPacketDeny;
	std::string m_Domain;
	std::set<std::string> m_adminCommandSet;
	std::map<std::string, std::string> m_metaStats;
	unsigned int m_serverClientCacheExpirySeconds;
	std::default_random_engine mRandomEngine;

};

#endif
