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

/*
 * Local Includes
 */
#include "MetaServer.hpp"
#include "MetaServerPacket.hpp"
#include "MetaServerHandlerTCP.hpp"
#include "MetaServerHandlerUDP.hpp"

/*
 * System Includes
 */
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <istream>
#include <fstream>
#include <boost/asio/placeholders.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>


MetaServer::MetaServer()
	: m_handshakeExpirySeconds(30),
	  m_expiryTimer(0),
	  m_updateTimer(0),
	  m_scoreTimer(0),
	  m_expiryDelayMilliseconds(3000),
	  m_updateDelayMilliseconds(5000),
	  m_scoreDelayMilliseconds(60000),
	  m_sessionExpirySeconds(3600),
	  m_clientExpirySeconds(300),
	  m_packetLoggingFlushSeconds(10),
	  m_loggingFlushSeconds(10),
	  m_maxServerSessions(1024),
	  m_maxClientSessions(4096),
	  m_startTime(boost::posix_time::microsec_clock::local_time()),
	  m_keepServerStats(false),
	  m_keepClientStats(false),
	  m_logServerSessions(false),
	  m_logClientSessions(false),
	  m_logPackets(false),
	  m_isDaemon(false),
	  m_PacketSequence(0),
	  m_PacketLogger(0),
	  m_Logfile(""),
	  m_PacketLogfile(""),
	  m_isShutdown(false),
	  m_logPacketAllow(false),
	  m_logPacketDeny(false),
	  m_serverClientCacheExpirySeconds(60)
{

	/*
	 * Enumerate the ADMINISTRATIVE command set
	 * Note: this is intentionally specific to force agreement with the unittest
	 */
	m_adminCommandSet = { "NMT_ADMINREQ_ENUMERATE", "NMT_ADMINREQ_ADDSERVER" };

	m_loggingFlushTime = msdo.getNow();
	srand( (unsigned)time(0));

	/*
	 * Construct Stats map
	 */
	m_metaStats["packet.sequence"] = "0";
	m_metaStats["server.sessions"] = "0";
	m_metaStats["client.sessions"] = "0";
	m_metaStats["client.cache"] = "0";
	m_metaStats["current.handshakes"] = "0";
	m_metaStats["server.uptime"] = "0";

}

MetaServer::~MetaServer()
{
	LOG(INFO) << "Shutting down metaserver-ng";
	google::FlushLogFiles(google::INFO);
}

void
MetaServer::expiry_timer(const boost::system::error_code& error)
{

	VLOG(5) << "Tick expiry_timer";

	boost::posix_time::ptime now = msdo.getNow();
	boost::posix_time::ptime etime;
	std::map<std::string,std::string>::iterator attr_iter;

	/*
	 * Remove handshakes that are older than threshold
	 * TODO: returned vector shows removed
	 */
	std::vector<unsigned int> expiredHS = msdo.expireHandshakes(m_handshakeExpirySeconds);
	if ( expiredHS.size() > 0 )
	{
		VLOG(2) << "Expiry Handshakes: " << expiredHS.size();
	}

    /*
     * Sweep for server sessions ... expire any that are older than m_sessionExpirySeconds
     */
	std::vector<std::string> expiredSS = msdo.expireServerSessions(m_sessionExpirySeconds);
	if ( expiredSS.size() > 0 )
	{
		VLOG(2) << "Expiry ServerSessions: " << expiredSS.size();
	}

    /**
     *  Remove client sessions that are expired
     */
	std::vector<std::string> expiredCS = msdo.expireClientSessions(m_clientExpirySeconds);
	if ( expiredCS.size() > 0 )
	{
		VLOG(2) << "Expiry ClientSessions: " << expiredCS.size();
	}

	/*
	 * We want to purge any cache items that are missing
	 */
	std::vector<std::string> expiredCSC = msdo.expireClientSessionCache(m_serverClientCacheExpirySeconds);
	if ( expiredCS.size() > 0 )
	{
		VLOG(2) << "Expiry ClientSession Cache: " << expiredCS.size();
	}

	/**
     * Display Server Sessions and Attributes
     */
	std::map< std::string, std::map<std::string,std::string> > itr_outer;
	std::map<std::string, std::string>::iterator itr_inner;
	if ( m_logServerSessions )
	{
		/*
		 * NOTE: maybe make getServerSessionList return an iterator instead so we don't have copy overhead ?
		 */
		std::list<std::string> slist = msdo.getServerSessionList(0,msdo.getServerSessionCount());
		VLOG(2) << " Total Server Sessions: " << slist.size();
		while ( ! slist.empty() )
		{
			std::string key = slist.front(); slist.pop_front();

			std::map<std::string,std::string> item = msdo.getServerSession(key);
			VLOG(2) << " Server Session: " << key;
			for( itr_inner = item.begin(); itr_inner != item.end(); itr_inner++ )
			{
				VLOG(3) << "    " << itr_inner->first << " == " << itr_inner->second;
			}
		}
	}

	/**
	 * Display Client Sessions and Attributes
	 */
	if ( m_logClientSessions )
	{
		/*
		 * NOTE: maybe make getServerSessionList return an iterator instead so we don't have copy overhead ?
		 */
		std::list<std::string> slist = msdo.getClientSessionList();
		while ( ! slist.empty() )
		{
			std::string key = slist.front(); slist.pop_front();
			std::map<std::string,std::string> item = msdo.getClientSession(key);
			VLOG(2) << " Client Session: " << key;
			for( itr_inner = item.begin(); itr_inner != item.end(); itr_inner++ )
			{
				VLOG(2) << "    " << itr_inner->first << " == " << itr_inner->second;
			}
			item = msdo.getClientFilter(key);
			if ( item.size() > 0 )
			{
				VLOG(2) << "    Filters:";
				for( itr_inner = item.begin(); itr_inner != item.end(); itr_inner++ )
				{
					VLOG(2) << "        " << itr_inner->first << " == " << itr_inner->second;
				}
			}


		}
	}

	/*
	 * Flush the packet logger
	 */
	int i=1;
	if ( m_logPackets )
	{
		if ( i > 0 )
			VLOG(2) << "     PacketLogger Flushed : " << i;
	}

	/*
	 * Flush the main logfiles
	 */
	etime = m_loggingFlushTime + boost::posix_time::seconds(m_loggingFlushSeconds);
	VLOG(9) << "   logginfFlushTime: " << m_loggingFlushTime;
	VLOG(9) << "   etime           : " << etime;
	if ( etime > now )
	{
		m_loggingFlushTime = now;
		VLOG_EVERY_N(3,10) << "  Flushing Log Messages";
		google::FlushLogFiles(google::INFO);
	}


    /**
     * Set the next timer trigger
     */
    m_expiryTimer->expires_from_now(boost::posix_time::milliseconds(m_expiryDelayMilliseconds));
    m_expiryTimer->async_wait(boost::bind(&MetaServer::expiry_timer, this, boost::asio::placeholders::error));
}

void
MetaServer::update_timer(const boost::system::error_code& error)
{

	VLOG(5) << "Tick update_timer";

	/**
	 *  Update Stats
	 *  This is intentionally stored as strings to avoid having to deal with
	 *  the different integer types.  Almost everything via stringstream can
	 *  convert to/from a number
	 */
	std::ostringstream ss;

	ss.str("");
	ss << m_PacketSequence;
	m_metaStats["packet.sequence"] = ss.str();

	ss.str("");
	ss << msdo.getServerSessionCount();
	m_metaStats["server.sessions"] = ss.str();

	ss.str("");
	ss << msdo.getClientSessionCount();
	m_metaStats["client.sessions"] = ss.str();

	ss.str("");
	ss << msdo.getHandshakeCount();
	m_metaStats["current.handshakes"] = ss.str();

	ss.str("");
	ss << msdo.getServerSessionCacheList().size();
	m_metaStats["client.cache"] = ss.str();

	/*
	 * Calculate uptime
	 */
	ss.str("");
	ss << getDeltaMillis();
	m_metaStats["server.uptime"] = ss.str();

	/*
	 * Reset Timer
	 */
    m_updateTimer->expires_from_now(boost::posix_time::milliseconds(m_updateDelayMilliseconds));
    m_updateTimer->async_wait(boost::bind(&MetaServer::update_timer, this, boost::asio::placeholders::error));
}

void
MetaServer::score_timer(const boost::system::error_code& error)
{
	VLOG(5) << "Tick score_timer(" << m_scoreDelayMilliseconds << ")";

	/*
	 * Check for the pidfile, shutdown if it's missing
	 */
	if ( !boost::filesystem::exists(m_pidFile) ||
		 !boost::filesystem::is_regular_file(m_pidFile) )
	{
		m_isShutdown = true;
		throw std::runtime_error("Pidfile was removed.  Inititating shutdown");
	}

	/*
	 * We want to write the m_serverData scoreboard
	 */
	std::ofstream clubber;
	std::list<std::string> slist;
	std::map<std::string,std::string> sess;

	if( ! boost::filesystem::is_directory(m_scoreServer.parent_path().string()) )
	{
		/*
		 * The parent directory of the specified score file does not exist
		 * and we want to just skip nicely over it, making a warning in the logs
		 * and continuing on anyway
		 */
		LOG(WARNING) << "Parent path of score file does not exist (" << m_scoreServer.string() << ")";
	}
	else
	{
		/*
		 * Directory is present, we're go for the clobber and fill
		 */
		boost::filesystem::remove(m_scoreServer);
		clubber.open(m_scoreServer.c_str());
		clubber.clear();

		/*
		 * Blast out servers
		 */
		slist = msdo.getServerSessionList(0,m_maxServerSessions);
		VLOG(5) << "Servers: " << slist.size();
		for(auto& m : slist )
		{
			VLOG(9) << "scoreboard(" << m_scoreServer.string() << "):" << m;
			sess = msdo.getServerSession(m);
			clubber << m << "={";
			for( auto& n : sess )
			{
				clubber << n.first << "=" << n.second << ",";
			}
			clubber << "}" << std::endl;
		}
		clubber.close();

	}

	/*
	 * Client Sessions
	 */
	if( ! boost::filesystem::is_directory(m_scoreClient.parent_path().string()) )
	{
		/*
		 * The parent directory of the specified score file does not exist
		 * and we want to just skip nicely over it, making a warning in the logs
		 * and continuing on anyway
		 */
		LOG(WARNING) << "Parent path of score file does not exist (" << m_scoreClient.string() << ")";
	}
	else
	{
		/*
		 * Directory is present, we're go for the clobber and fill
		 */
		boost::filesystem::remove(m_scoreClient);
		clubber.open(m_scoreClient.c_str());
		clubber.clear();

		/*
		 * Blast out clients
		 */
		slist = msdo.getClientSessionList();
		VLOG(5) << "Clients: " << slist.size();
		for(auto& m : slist )
		{
			VLOG(9) << "scoreboard(" << m_scoreClient.string() << "):" << m;
			sess = msdo.getClientSession(m);
			clubber << m << "={";
			for( auto& n : sess )
			{
				clubber << n.first << "=" << n.second << ",";
			}
			clubber << "}" << std::endl;
		}
		clubber.close();

	}

	/*
	 * Stats Scoreboard
	 */
	if( ! boost::filesystem::is_directory(m_scoreStats.parent_path().string()) )
	{
		LOG(WARNING) << "Parent path of score file does not exist (" << m_scoreStats.string() << ")";
	}
	else
	{
		boost::filesystem::remove(m_scoreStats);
		clubber.open(m_scoreStats.c_str());
		clubber.clear();

		/*
		 * Put in the stats
		 */
		for( auto& r: m_metaStats )
		{
			clubber << r.first << "=" << r.second << std::endl;
		}
		clubber.close();
	}

	/*
	 * Cache Scoreboard
	 */
	if( ! boost::filesystem::is_directory(m_scoreCCache.parent_path().string()) )
	{
		LOG(WARNING) << "Parent path of score file does not exist (" << m_scoreCCache.string() << ")";
	}
	else
	{
		std::list<std::string> v;
		boost::filesystem::remove(m_scoreCCache);
		clubber.open(m_scoreCCache.c_str());
		clubber.clear();
		v.clear();
		slist = msdo.getServerSessionCacheList();
		for( auto& m : slist )
		{
			clubber << m << "(" << msdo.getServerSessionCount(m) << ")";

			v = msdo.getServerSessionList(0,m_maxServerSessions,m);

			clubber << "(" << v.size() << ")";
			if ( v.size() > 0 )
			{
				clubber << "={ ";
				for(auto& n : v)
				{
					clubber << n << ",";
				}
				clubber << " };" << std::endl;
			}
			else
			{
				clubber << "={ };" << std::endl;
			}
		}
		clubber.close();
	}

	/*
	 * Reset Timer
	 */
    m_scoreTimer->expires_from_now(boost::posix_time::millisec(m_scoreDelayMilliseconds));
    m_scoreTimer->async_wait(boost::bind(&MetaServer::score_timer, this, boost::asio::placeholders::error));
}

/**
 * Convenience method that evaluates what type of packet and call appropriate handle method
 * @param msp incoming metaserver packet
 * @param rsp outgoing metaserver packet to be filled
 */
void
MetaServer::processMetaserverPacket(MetaServerPacket& msp, MetaServerPacket& rsp)
{

	/*
	 * Packet Sequence: store this so that we can replay the packets in the
	 *                  same order after the fact
	 * Time Offset: time in milliseconds relative to the "start time".  The start time
	 *              is defined as the first packet to be processed.  I chose this because
	 *              it will be possible to replay the packets in the correct order, at
	 *              exactly the same rate, relative to the start of the first packet
	 */
	if ( m_PacketSequence == 0 )
		m_startTime = boost::posix_time::microsec_clock::local_time();

	++m_PacketSequence;
	msp.setSequence(m_PacketSequence);
	msp.setTimeOffset( getDeltaMillis() );

	switch(msp.getPacketType())
	{
	case NMT_SERVERKEEPALIVE:
		processSERVERKEEPALIVE(msp,rsp);
		break;
	case NMT_SERVERSHAKE:
		processSERVERSHAKE(msp,rsp);
		break;
	case NMT_TERMINATE:
		processTERMINATE(msp,rsp);
		break;
	case NMT_CLIENTKEEPALIVE:
		processCLIENTKEEPALIVE(msp,rsp);
		break;
	case NMT_CLIENTSHAKE:
		processCLIENTSHAKE(msp,rsp);
		break;
	case NMT_LISTREQ:
		processLISTREQ(msp,rsp);
		break;
	case NMT_SERVERATTR:
		processSERVERATTR(msp,rsp);
		break;
	case NMT_CLIENTATTR:
		processCLIENTATTR(msp,rsp);
		break;
	case NMT_CLIENTFILTER:
		processCLIENTFILTER(msp,rsp);
		break;
	case NMT_DNSREQ:
		processDNSREQ(msp,rsp);
		break;
	case NMT_ADMINREQ:
		processADMINREQ(msp,rsp);
	default:
//		--m_PacketSequence;
		VLOG(1) << "Packet Type [" << msp.getPacketType() << "] not supported.";
		break;
	}

	/*
	 * Flag response packets sequence and offset tagging
	 */
	++m_PacketSequence;
	rsp.setSequence(m_PacketSequence);
	rsp.setTimeOffset( getDeltaMillis() );
	rsp.setOutBound(true);

	/*
	 * Packet Logging
	 */
	if ( m_logPackets )
	{
		// log some packets
	}

}

/**
 * @param in incoming metaserver packet
 *
 * 	NMT_SERVERKEEPALIVE		indicates a keep alive for a server, also serves as a "registration"
 * 	Response Packet Type: NMT_HANDSHAKE
 * 	- pack packet type
 * 	- pack random number
 */
void
MetaServer::processSERVERKEEPALIVE(const MetaServerPacket& in, MetaServerPacket& out)
{

	uint32_t i = msdo.addHandshake();

	if ( i > 0 )
	{
		VLOG(1) << "processSERVERKEEPALIVE(): " << i;
		out.setPacketType(NMT_HANDSHAKE);
		out.addPacketData(i);
		out.setAddress( in.getAddress() );
	}
}

/**
 * NMT_SERVERSHAKE - last part of the 3-way server handshake
 * Response: None
 * @param in
 * @param out
 */
void
MetaServer::processSERVERSHAKE(const MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int shake = in.getIntData(4);
	std::string ip = in.getAddressStr();
	uint32_t packed_ip = 0;
	VLOG(1) << "processSERVERSHAKE(): " << shake;

	/**
	 * If a handshake exists, we can know the following:
	 * 1) the client is valid
	 * 2) they have a non-expired handshake
	 *
	 * What we do then is to:
	 * 1) register a server session
	 * 2) de-register the handshake ( maybe we just let it expire ? )
	 */
	if( msdo.handshakeExists(shake) )
	{
		/*
		 * Grab the in packet default values
		 */
		std::stringstream ss,ss2,ss3;
		ss << in.getPort();
		ss2 << in.getAddressInt();

		/*
		 * If it's larger than 8 bytes, it's got an IP
		 */
		if ( in.getSize() > 8 )
		{
			packed_ip = in.getIntData(8);
			ip = MetaServerPacket::IpNetToAscii(packed_ip);

			ss2.str("");
			ss2 << packed_ip;
		}

		/*
		 * If it's larger than 12 bytes, it also includes port
		 */
		if ( in.getSize() > 12 )
		{
			ss.str("");
			ss << in.getIntData(12);
		}

		/*
		 * Wrangle the time between when the keepalive and servershake were sent
		 * to determine communication latency
		 */
		boost::posix_time::ptime t1 = msdo.getHandshakeExpiry(shake);
		VLOG(5) << ip << "-handShakeExpiry(" << shake << "):" << t1;

		boost::posix_time::ptime t2 = msdo.getNow();
		VLOG(5) << ip << "-handShakeExpiry(now): " << t2;

		unsigned int latency = msdo.getLatency(t1,t2);
		VLOG(5) << "  " << ip << "-latency : " << latency;

		ss3 << latency;

		/*
		 * Put it into state
		 */
		msdo.addServerSession(ip);
		msdo.addServerAttribute(ip,"port", ss.str() );
		msdo.addServerAttribute(ip,"ip_int", ss2.str() );
		msdo.addServerAttribute(ip,"latency",ss3.str() );

	}

}

void
MetaServer::processTERMINATE(const MetaServerPacket& in, MetaServerPacket& out)
{

	/**
	 *  For backwards compat, we make a regular "TERM" packet end a server session
	 *  and a TERM packet with any additional data sent signifies a client session.
	 */
	if ( in.getSize() > (sizeof(uint32_t)) )
	{
		/*
		 * Grab the "extra" bit.
		 * Use Case 1: 0 value padding indicates client terminate
		 * Use Case 2: non-zero value padding is an IP by proxy
		 */
		uint32_t key = in.getIntData(4);
		if ( key == 0 )
		{
			VLOG(1) << "processTERMINATE-client(): " << in.getAddressStr();
			/*
			 * Normally want to terminate sessions and may turn this back
			 * on, but generally speaking we want client sessions to expire out
			 */
//			msdo.removeClientSession(in.getAddressStr());
		}
		else
		{
			std::string skey = MetaServerPacket::IpNetToAscii(key);
			VLOG(1) << "processTERMINATE-server(" << key << ")(" << skey << ")";
			msdo.removeServerSession(skey);
		}
	}
	else
	{
		VLOG(1) << "processTERMINATE-server(): " << in.getAddressStr();
		msdo.removeServerSession(in.getAddressStr());
	}

}

void
MetaServer::processCLIENTKEEPALIVE(const MetaServerPacket& in, MetaServerPacket& out)
{

	uint32_t i = msdo.addHandshake();

	if ( i > 0 )
	{
		VLOG(1) << "processCLIENTKEEPALIVE()" << i;
		out.setPacketType(NMT_HANDSHAKE);
		out.addPacketData(i);
		out.setAddress( in.getAddress() );
	}

}

void
MetaServer::processCLIENTSHAKE(const MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int shake = in.getIntData(4);
	std::string ip = in.getAddressStr();
	VLOG(1) << "processCLIENTSHAKE()" << shake;

	if( msdo.handshakeExists(shake) )
	{
		std::stringstream ss;
		ss << in.getPort();

		msdo.addClientSession(ip);
		msdo.addClientAttribute(ip,"port", ss.str() );
	}

}

/**
 * NMT_LISTREQ:
 * 		4 bytes type
 * 		4 bytes start index
 *
 * Response:
 *
 * NMT_LISTRESP
 *
 * 		4 bytes type
 * 		4 bytes total servers in list
 * 		4 bytes servers in this packet ( triggering client to have another REQ with total-servers offset )
 * 		4 bytes per server in this packet
 *
 */
void
MetaServer::processLISTREQ( const MetaServerPacket& in, MetaServerPacket& out)
{
	uint32_t server_index = in.getIntData(4);
	uint32_t packed = 0;
	uint32_t temp_int = 0;
	std::list<uint32_t> resp_list;
	std::string ip_str = in.getAddressStr();
	uint32_t total = msdo.getServerSessionCount(ip_str);
	uint32_t packed_max = total;


	/*
	 * Trigger the creation of a listreq cache creation
	 */
	if ( server_index == 0 )
	{
		VLOG(4) << "Initial LISTREQ Request, creating lookup cache (" << ip_str << ")";
		total = msdo.createServerSessionListresp(ip_str);
		VLOG(5) << "  Total: " << total;
		packed_max = total;
	}

	/*
	 * If we are unable to pack the entire list into 1 packet
	 */
	if ( (total*sizeof(uint32_t) - (server_index*sizeof(uint32_t)) ) > (MAX_UDP_OUT_BYTES-4-4-4) )
	{
		/*
		 * We want it to round ... just like the price is right, the goal is not to go over
		 */
		packed_max = (MAX_UDP_OUT_BYTES-4-4-4) / sizeof(uint32_t);
	}


	/*
	 * We hide the craziness of what goes on here inside the single method.
	 * The goal, is to get the list of servers constrained by packed_max
	 */
	std::list<std::string> sess_list = msdo.getServerSessionList(server_index,packed_max,ip_str);
    std::list<std::string>::iterator list_itr;

	VLOG(5) << "server_index:" << server_index
			<< " ** total: " << total
			<< " ** packed_max: " << packed_max
			<< " ** sess_list: " << sess_list.size();

    for ( list_itr = sess_list.begin(); list_itr != sess_list.end() ; list_itr++ )
    {
    	/*
    	 * Defensive to make sure we're not going to exceed our max
    	 */
    	if ( packed >= packed_max )
    		break;

    	/*
    	 * Defensive to make sure that the item in the list is
    	 * actually a valid data item we can send ( orthogonal processes
    	 * such as expiry could invalidate at any time ).
    	 *
    	 * Thus we can iterate over as much of the server list as we need to
    	 * and dead items won't count, only those added to the response packet.
    	 */
    	if ( msdo.serverSessionExists( *list_itr ))
    	{
    		/*
    		 * Pack the int IP
    		 */
    		std::istringstream( msdo.getServerSession(*list_itr)["ip_int"] ) >> temp_int;

    		VLOG(5) << "Packing Session Itr[" << *list_itr << "] Session Int["
    				<< msdo.getServerSession(*list_itr)["ip_int"] << "] Session IP["
    				<< msdo.getServerSession(*list_itr)["ip"] << "] SS Int["
    				<< temp_int << "]";
    		resp_list.push_back( temp_int );

    		//resp_list.push_back( atoi( msdo.getServerSession(*list_itr)["ip_int"].c_str() ) );
    		++packed;
    	}

    }

    if ( packed != resp_list.size() )
    {
    	LOG(WARNING) << "Packed: " << packed << " vs Response: " << resp_list.size() << "MISMATCH!";
    }

	out.setAddress( in.getAddress() );
	out.setPacketType(NMT_LISTRESP);

	/**
	 * If the list is empty, just send a 0,0 to indicate completion.
	 * NOTE: I think this logic is a bug in the protocol, as the
	 * 		original MS code looks as if this was just not working correctly.
	 */
	if ( resp_list.size() > 0 )
	{
		out.addPacketData( msdo.getServerSessionCount() );
		out.addPacketData( (uint32_t)resp_list.size() );
		while ( ! resp_list.empty() )
		{
			VLOG(2) << "processLISTRESP() - Adding : " << resp_list.front();
			out.addPacketData(resp_list.front());
			resp_list.pop_front();
		}
	}
	else
	{
		/*
		 *  For the record, I think this is a stupid protocol construct
		 */
		VLOG(2) << "processLISTRESP(0,0) - Empty";
		out.addPacketData( 0 );
		out.addPacketData( 0 );
	}

}

void
MetaServer::processSERVERATTR(const MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int name_length = in.getIntData(4);
	unsigned int value_length = in.getIntData(8);
	std::string msg = in.getPacketMessage(12);
	std::string name = msg.substr(0,name_length);
	std::string value = msg.substr(name_length,value_length);
	std::string ip = in.getAddressStr();
	VLOG(2) << "processSERVERATTR() : " << name << "," << value;
	msdo.addServerAttribute(ip,name,value);

	out.setPacketType(NMT_NULL);
}

void
MetaServer::processCLIENTATTR(const MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int name_length = in.getIntData(4);
	unsigned int value_length = in.getIntData(8);
	std::string msg = in.getPacketMessage(12);
	std::string name = msg.substr(0,name_length);
	std::string value = msg.substr(name_length,value_length);
	std::string ip = in.getAddressStr();
	VLOG(2) << "processCLIENTATTR() : " << name << "," << value;
	msdo.addClientAttribute(ip,name,value);

	out.setPacketType(NMT_NULL);
}

void
MetaServer::processCLIENTFILTER(const MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int name_length = in.getIntData(4);
	unsigned int value_length = in.getIntData(8);
	std::string msg = in.getPacketMessage(12);
	std::string name = msg.substr(0,name_length);
	std::string value = msg.substr(name_length,value_length);
	std::string ip = in.getAddressStr();
	VLOG(2) << "processCLIENTFILTER() : " << name << "," << value;
	msdo.addClientFilter(ip,name,value);

	out.setPacketType(NMT_NULL);
}


/**
 * This is a search and find type of request.
 * The input is a string that is:
 * 		1) The IP desired, as in the case of a reverse lookup (1.2.3.4)
 * 			The results of that would yield the 'name' attribute of the server
 * 		2) The NAME desired, as corresponding to the 'name' server attribute
 * 			The results would then be the IP
 *
 * 	We can not distinguish between these cases ahead of time, simply because when someone
 * 	does a dig foo @server and dig 1.2.3.4 @server will functionally look the same and we
 * 	need to distinguish via searching.  We could make a different packet type for each of those,
 * 	but then we require the caller to know ahead of time if it's a forward vs reverse lookup.
 *
 * 	The response is almost identical to the listresp except we'll encode by string instead of
 * 	by uint32 ( making it a more heavy request ).
 *
 * 	This is an unauthenticated request.  If there needs to be abuse detection, we can
 * 	always put in a scoreboard for just this type of request and throttle it.
 *
 * 	This must NEVER perform any write/change operations
 * 	on any of the metaserver data.  Unauthenticated data modification is Bad(tm).
 */
void
MetaServer::processDNSREQ(const MetaServerPacket& in, MetaServerPacket& out)
{
	uint32_t dns_type = in.getIntData(4);
	uint32_t msg_length = in.getIntData(8);
	std::string msg = in.getPacketMessage(12);
	std::list<std::string> resp_list,temp_list;
	std::string outmsg;

	std::string name = msg.substr(0,msg_length);

	VLOG(2) << "processDNSREQ-type: " << dns_type << " : " << name;
	VLOG(2) << "processDNSREQ-msg_length : " << msg_length;
	VLOG(2) << "processDNSREQ-msg: " << msg.length() << " : " << msg;

	/**
	 * Determine packet search type and get results
	 */
	switch(dns_type) {
		case DNS_TYPE_A:
			/*
			 * Forward lookup
			 */
			resp_list = msdo.searchServerSessionByAttribute( "name", name );
			VLOG(2) << "processDNSREQ: DNS_TYPE_A request : " << resp_list.size();
			break;
		case DNS_TYPE_PTR:
			/*
			 * Reverse Lookup
			 */
			resp_list = msdo.searchServerSessionByAttribute( "ip", name );
			VLOG(2) << "processDNSREQ: DNS_TYPE_PTR request : " << resp_list.size();
			break;
		case DNS_TYPE_ALL:
			/*
			 * sigh
			 */
			temp_list = msdo.searchServerSessionByAttribute("name", name);
			resp_list = msdo.searchServerSessionByAttribute("ip", name);
			VLOG(2) << "processDNSREQ: DNS_TYPE_ALL request : " << temp_list.size() << ":" << resp_list.size();
			resp_list.merge(temp_list);
			break;

		default:
			// search name by default
			// resp_list = msdo.searchServerSessionByAttribute("name", name );
			VLOG(2) << "processDNSREQ: unknown DNS_TYPE sent : " << dns_type;
			break;
	}


	/**
	 * Keep it minimal
	 */
	resp_list.unique();

	/**
	 * Initial packet stuffing
	 */
	out.setPacketType(NMT_DNSRESP);

	/**
	 *	Initial expectations is to have only a couple of responses
	 *	if large/full listings are expected, we'll need to breakup
	 *	similar to a LISTRESP
	 */
	if ( resp_list.size() > 0 )
	{
		/*
		 * pseudo:
		 * 	    for each resp_list item
		 * 	    	-break loop if itr size ( 1 byte per string item ) + 4 bytes ( for size uint )
		 * 	    	is larger than max packet size.
		 * 	    	-count bytes, put on ship-to list/array
		 * 	    	- keep index
		 *
		 * 	    pack resp_list size
		 * 	    pack index
		 * 	  	loop over ship-to list
		 * 	  		pack size
		 * 	  		pack msg
		 */
		/*
		 * Max Servers
		 */
		out.addPacketData(resp_list.size());

		/*
		 * Packed Servers
		 */
		out.addPacketData(resp_list.size());

		/**
		 * Pack in lengths
		 */
		std::list<std::string>::iterator itr;
		unsigned int rlength = 0;
		for ( itr = resp_list.begin(); itr!=resp_list.end(); itr++ )
		{
			rlength = itr->length();
			VLOG(2) << "Stuffing DNSRESP Packet [" << rlength << "] " << *itr;
			out.addPacketData(rlength);
			outmsg+=*itr;
		}

		VLOG(2) << "Stuffing DNSRESP Message [" << outmsg << "]";

		out.addPacketData(outmsg);

	}
	else
	{

		VLOG(2) << "processDNSREQ: packing null packet";
		/*
		 * Max Servers
		 */
		out.addPacketData(0);

		/*
		 * Packed Servers
		 */
		out.addPacketData(0);
	}

}

void
MetaServer::processADMINREQ(const MetaServerPacket& in, MetaServerPacket& out)
{

	uint32_t sub_type  = in.getIntData(4);
	uint32_t in_addr;
	unsigned int in_port;
	std::stringstream ss;
	std::string out_msg = "";

	VLOG(2) << "processADMINREQ(" << sub_type << ")";

	/*
	 * TODO: add in acl check, deny immediately if appropriate
	 */
	// msdo.aclAdminCheck(ip) || return ERR

	out.setPacketType(NMT_ADMINRESP);

	switch(sub_type) {
		case NMT_ADMINREQ_ENUMERATE:
			VLOG(3) << "NMT_ADMINREQ_ENUMERATE : " << m_adminCommandSet.size();
			out.addPacketData(NMT_ADMINRESP_ENUMERATE);
			out.addPacketData(m_adminCommandSet.size());
			for(auto& p: m_adminCommandSet)
			{
				out.addPacketData(p.length());
				out_msg.append(p);
			}
			out.addPacketData(out_msg);
			break;
		case NMT_ADMINREQ_ADDSERVER:
			in_addr   = in.getIntData(8);
			in_port   = in.getIntData(12);

			/*
			 * Convert IP
			 */
			out.setAddress(MetaServerPacket::IpNetToAscii(in_addr));
			out.setPort(in_port);
			msdo.addServerSession(out.getAddress());

			ss << in_port;
			msdo.addServerAttribute(out.getAddress(),"port", ss.str() );
			ss.str("");
			ss << in_addr;
			msdo.addServerAttribute(out.getAddress(),"ip_int", ss.str() );
			out.addPacketData(NMT_ADMINRESP_ADDSERVER);
			out.addPacketData(in_addr);
			out.addPacketData(in_port);
			break;
		default:
			VLOG(2) << "NMT_ADMINRESP_ERR";
			out.addPacketData(NMT_ADMINRESP_ERR);
	}

}

void
MetaServer::registerConfig( boost::program_options::variables_map & vm )
{

	/*
	 * All configuration items passed in must be converted to local type
	 * member variables, or are ignored.  We do not want to reference a
	 * variable_map during operation as a bad_cast exception could cause
	 * server failure during the run loop.
	 */
	if( vm.count("performance.server_session_expiry_seconds") )
		m_sessionExpirySeconds = vm["performance.server_session_expiry_seconds"].as<int>();

	if( vm.count("performance.client_session_expiry_seconds") )
		m_clientExpirySeconds = vm["performance.client_session_expiry_seconds"].as<int>();

	if ( vm.count("performance.max_client_sessions") )
		m_maxClientSessions = vm["performance.max_client_sessions"].as<int>();

	if ( vm.count("performance.max_server_sessions") )
		m_maxServerSessions = vm["performance.max_server_sessions"].as<int>();

	if ( vm.count("performance.server_client_cache_expiry_seconds") )
		m_serverClientCacheExpirySeconds = vm["performance.server_client_cache_expiry_seconds"].as<int>();

	if ( vm.count("performance.tick_expiry_milliseconds") )
		m_expiryDelayMilliseconds = vm["performance.tick_expiry_milliseconds"].as<int>();

	if ( vm.count("performance.tick_update_milliseconds") )
		m_updateDelayMilliseconds = vm["performance.tick_update_milliseconds"].as<int>();

	if ( vm.count("performance.tick_score_milliseconds") )
		m_scoreDelayMilliseconds = vm["performance.tick_score_milliseconds"].as<int>();


	if( vm.count("server.client_stats") )
	{
		std::string s = vm["server.client_stats"].as<std::string>();
		if ( boost::iequals(s,"true") )
		{
			m_keepClientStats = true;
		}
		else if ( boost::iequals(s,"false") )
		{
			m_keepClientStats = false;
		}

	}

	if( vm.count("server.server_stats") )
	{
		std::string s = vm["server.server_stats"].as<std::string>();
		if ( boost::iequals(s,"true") )
		{
			m_keepServerStats = true;
		}
		else if ( boost::iequals(s,"false") )
		{
			m_keepServerStats = false;
		}

	}

	if( vm.count("server.daemon") )
	{
		std::string s = vm["server.daemon"].as<std::string>();
		if ( boost::iequals(s,"true") )
		{
			m_isDaemon = true;
		}
		else if ( boost::iequals(s,"false") )
		{
			m_isDaemon = false;
		}

	}

	if ( vm.count("server.domain"))
	{
		m_Domain = vm["server.domain"].as<std::string>();
		if ( m_Domain.length() == 0 )
			m_Domain = "ms.worldforge.org";
	}

	if( vm.count("logging.server_sessions") )
	{
		std::string s = vm["logging.server_sessions"].as<std::string>();
		if ( boost::iequals(s,"true") )
		{
			m_logServerSessions = true;
		}
		else if ( boost::iequals(s,"false") )
		{
			m_logServerSessions = false;
		}

	}

	if( vm.count("logging.client_sessions") )
	{
		std::string s = vm["logging.client_sessions"].as<std::string>();
		if ( boost::iequals(s,"true") )
		{
			m_logClientSessions = true;
		}
		else if ( boost::iequals(s,"false") )
		{
			m_logClientSessions = false;
		}

	}

	if ( vm.count("logging.packet_logging") )
	{
		std::string s = vm["logging.packet_logging"].as<std::string>();
		if ( boost::iequals(s,"true") )
		{
			m_logPackets = true;
		}
		else if ( boost::iequals(s,"false") )
		{
			m_logPackets = false;
		}

	}

	if ( vm.count("logging.packet_allow") )
	{
		std::string s = vm["logging.packet_allow"].as<std::string>();
		if ( boost::iequals(s,"true") )
		{
			m_logPacketAllow = true;
		}
		else if ( boost::iequals(s,"false") )
		{
			m_logPacketAllow = false;
		}

	}

	if ( vm.count("logging.packet_deny") )
	{
		std::string s = vm["logging.packet_deny"].as<std::string>();
		if ( boost::iequals(s,"true") )
		{
			m_logPacketDeny = true;
		}
		else if ( boost::iequals(s,"false") )
		{
			m_logPacketDeny = false;
		}

	}


	if( vm.count("logging.packet_logfile") )
	{
		m_PacketLogfile = vm["logging.packet_logfile"].as<std::string>();
		/*
		 *  Set a hard default if it's not specified
		 */
		if( m_PacketLogfile.length() == 0 )
		{
			m_PacketLogfile = "~/.metaserver-ng/packetdefault.bin";
		}

		if ( m_PacketLogfile.substr(0,1) == "~")
		{
			m_PacketLogfile.replace(0,1, std::getenv("HOME") );
		}

	}

	if ( vm.count("logging.packet_logging_flush_seconds"))
		m_packetLoggingFlushSeconds = vm["logging.packet_logging_flush_seconds"].as<unsigned int>();

	if ( vm.count("logging.logging_flush_seconds"))
		m_loggingFlushSeconds = vm["logging.logging_flush_seconds"].as<unsigned int>();

    //std::cout << "DEBUG: " << vm["server.port"].as<std::string>() << std::endl;
	for (boost::program_options::variables_map::iterator it=vm.begin(); it!=vm.end(); ++it )
	{
		if ( it->second.value().type() == typeid(int) )
		{
			std::cout << "  " << it->first.c_str() << " = " << it->second.as<int>() << std::endl;
		}
		else if (it->second.value().type() == typeid(std::string) )
		{
			std::cout << "  " << it->first.c_str() << " = " << it->second.as<std::string>().c_str() << std::endl;
		}
	}

	if( vm.count("server.logfile") )
	{
		m_Logfile = vm["server.logfile"].as<std::string>();
		std::cout << "Assigning m_Logfile : " << m_Logfile << std::endl;
		/**
		 * I tried to use boost::filesystem here, but it is so very stupid
		 * that I have opted for the more brittle way, because at least it works.
		 *
		 * TODO: add ifdef WIN32 here if/when metserver needs to run on windows
		 */
		if ( m_Logfile.substr(0,1) == "~")
		{
			m_Logfile.replace(0,1, std::getenv("HOME") );
		}
	}

	if( vm.count("server.pidfile") )
	{
		m_pidFile = vm["server.pidfile"].as<std::string>();
		std::cout << "Assigning m_pidFile : " << m_pidFile.string() << std::endl;
	}

	/*
	 * Scoreboard files
	 */
	if ( vm.count("scoreboard.server") )
	{
		m_scoreServer = vm["scoreboard.server"].as<std::string>();
		std::cout << "Assigning m_scoreServer : " << m_scoreServer.string() << std::endl;
	}

	if ( vm.count("scoreboard.client") )
	{
		m_scoreClient = vm["scoreboard.client"].as<std::string>();
		std::cout << "Assigning m_scoreClient : " << m_scoreClient.string() << std::endl;
	}


	if ( vm.count("scoreboard.stats") )
	{
		m_scoreStats = vm["scoreboard.stats"].as<std::string>();
		std::cout << "Assigning m_scoreStats : " << m_scoreStats.string() << std::endl;
	}

	if ( vm.count("scoreboard.ccache") )
	{
		m_scoreCCache = vm["scoreboard.ccache"].as<std::string>();
		std::cout << "Assigning m_scoreCCache : " << m_scoreCCache.string() << std::endl;
	}

	/**
	 * Initialise the logger to appropriately
	 */
	initLogger();

	/**
	 * Initialise the packet logger
	 */
	if ( m_logPackets )
	{
		// init packet
	}

	/**
	 * Print out the startup values
	 */
	LOG(INFO) << "WorldForge MetaServer Runtime Configuration";
	LOG(INFO) << "Server Version: " << SERVER_VERSION;
	LOG(INFO) << "API    Version: " << API_VERSION;
	for (boost::program_options::variables_map::iterator it=vm.begin(); it!=vm.end(); ++it )
	{
		if ( it->second.value().type() == typeid(int) )
		{
			LOG(INFO) << "  " << it->first.c_str() << " = " << it->second.as<int>();
		}
		else if (it->second.value().type() == typeid(std::string) )
		{
			LOG(INFO) << "  " << it->first.c_str() << " = " << it->second.as<std::string>().c_str();
		}
	}
}

void
MetaServer::initLogger()
{

	/**
	 * If a logfile is specified, use it
	 */
	std::cout << "m_Logfile: " << m_Logfile << std::endl;
	if ( !m_Logfile.empty() )
	{
		/*
		 * Create the log directory if it does not exist
		 */
		boost::filesystem::path p = m_Logfile;
		LOG(INFO) << "Log File: " << p.filename().string();
		std::cout << "Log File: " << p.filename().string() << std::endl;
		std::cout << "Log Directory Root Path: " << p.parent_path().string() << std::endl;

		if ( ! boost::filesystem::is_directory(p.parent_path().string()))
		{
			LOG(INFO) << "Creating Log Directory : " << p.parent_path().string();
			std::cout << "Creating Log Directory : " << p.parent_path().string() << std::endl;
			boost::filesystem::create_directory(p.parent_path());
		}

		/*
		 * I hope this actually logs to stdout in the event that no destination is set
		 */
		google::SetLogDestination(google::INFO,m_Logfile.c_str());
		LOG(INFO) << "Metaserver Logfile : " << m_Logfile;
	}
//	else
//	{
//		boost::filesystem::path p = boost::filesystem::temp_directory_path();
//		p /= "/metaserver.log";
//		google::SetLogDestination(google::INFO,p.c_str());
//		LOG(INFO) << "Temporary Logfile: " << p;
//		std::cout << "Temporary Logfile: " << p << std::endl;
//	}

	LOG(INFO) << "MetaServer logging initialized";

}

void
MetaServer::initTimers(boost::asio::io_service& ios)
{
	LOG(INFO) << "Timer initiation";

	if(m_expiryTimer)
	{
		LOG(WARNING) << "Purging m_expiryTimer";
		delete m_expiryTimer;
		m_expiryTimer = 0;
	}

	if(m_updateTimer)
	{
		LOG(WARNING) << "Purging m_updateTimer";
		delete m_updateTimer;
	}

	if(m_scoreTimer)
	{
		LOG(WARNING) << "Purging m_scoreTimer";
		delete m_scoreTimer;
	}

	m_expiryTimer = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	m_expiryTimer->async_wait(boost::bind(&MetaServer::expiry_timer, this, boost::asio::placeholders::error));
	m_updateTimer = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	m_updateTimer->async_wait(boost::bind(&MetaServer::update_timer, this, boost::asio::placeholders::error));
    m_scoreTimer  = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
    m_scoreTimer->async_wait(boost::bind(&MetaServer::score_timer, this, boost::asio::placeholders::error));

	LOG(INFO) << "Timer initiation completed";

}

unsigned long long
MetaServer::getDeltaMillis()
{
    boost::posix_time::ptime ntime = msdo.getNow();
    boost::posix_time::time_duration dur = ntime - m_startTime;
    return dur.total_milliseconds();
}

bool
MetaServer::isDaemon()
{
	return m_isDaemon;
}

void
MetaServer::getMSStats( std::map<std::string,std::string>& req_stats )
{
	if( req_stats.size() == 0 )
	{
		/*
		 * If you've specified nothing, you get everything
		 */
		VLOG(3) << "Full Stats Request";
		for(auto& x: m_metaStats)
		{
			req_stats[x.first] = x.second;
			VLOG(4) << "   Set " + x.first + " to " + x.second;
		}
	}
	else
	{
		/*
		 * Check all elements of the requesting map
		 */
		VLOG(3) << "Partial Stats Request";
		for(auto& x: req_stats)
		{

			/*
			 * If it exists, fill it with the value
			 */
			if ( m_metaStats.count(x.first) > 0 )
			{
				req_stats[x.first] = m_metaStats[x.first];
				VLOG(4) << "   Set " + x.first + " to " + m_metaStats[x.first];
			}
			else
			{
				/*
				 * If not present, remove the entry
				 * NOTE: can I remove WITH the iterator i'm using?
				 */
				req_stats.erase(x.first);
				VLOG(4) << "   Removing " + x.first;
			}
		}
	}
}
