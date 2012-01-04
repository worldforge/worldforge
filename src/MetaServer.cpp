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

#include "MetaServer.hpp"
#include "MetaServerHandlerTCP.hpp"
#include "MetaServerHandlerUDP.hpp"

MetaServer::MetaServer(boost::asio::io_service& ios)
	: m_expiryDelayMilliseconds(1500),
	  m_updateDelayMilliseconds(4000),
	  m_handshakeExpirySeconds(30),
	  m_sessionExpirySeconds(3600),
	  m_clientExpirySeconds(300),
	  m_keepServerStats(false),
	  m_keepClientStats(false),
	  m_maxServerSessions(1024),
	  m_maxClientSessions(4096),
	  m_isDaemon(false),
	  m_Logfile(""),
	  m_Logger( log4cpp::Category::getInstance("metaserver-ng") ),
	  m_logServerSessions(false),
	  m_logClientSessions(false)
{
	m_expiryTimer = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	m_expiryTimer->async_wait(boost::bind(&MetaServer::expiry_timer, this, boost::asio::placeholders::error));
	m_updateTimer = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	m_updateTimer->async_wait(boost::bind(&MetaServer::update_timer, this, boost::asio::placeholders::error));

	srand( (unsigned)time(0));

}

MetaServer::~MetaServer()
{
	m_Logger.info("Shutting down logging");
	log4cpp::Category::shutdown();
}

void
MetaServer::expiry_timer(const boost::system::error_code& error)
{
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
		logger.
	}

    /*
     * Sweep for server sessions ... expire any that are older than m_sessionExpirySeconds
     */

//    std::map<std::string,std::map<std::string,std::string> >::iterator itr2;
//    for ( itr2 = m_serverData.begin(); itr2 != m_serverData.end(); itr2++ )
//    {
//    	std::string s = itr2->first;
//
//    	etime =  boost::posix_time::from_iso_string(itr2->second["expiry"]) +
//    			 boost::posix_time::seconds(m_sessionExpirySeconds);
//
//    	if ( now > etime )
//    	{
//    		m_Logger.debugStream() << "EXPIRY TIMER: expire server session - " << s;
//    		removeServerSession(s);
//    	}
//    }

    /**
     * Sweep the ordered list we dish out in a listresp
     */
//    std::list<std::string>::iterator list_itr;
//    for ( list_itr = m_serverDataList.begin(); list_itr != m_serverDataList.end(); ++list_itr )
//    {
//    	m_Logger.debugStream() << "EXPIRY TIMER: check m_serverDataList : " << *list_itr;
//    	/*
//    	 * If find that one of the listresp items does NOT exist in the
//    	 * main data structure, we want to purge it
//    	 */
//    	if ( m_serverData.find( *list_itr ) == m_serverData.end() )
//    	{
//    		m_Logger.debugStream() << "EXPIRY TIMER: removing stale listresp ref: " << *list_itr;
//    		m_serverDataList.remove( *list_itr );
//    	}
//    }


    /**
     *  Remove client sessions that are expired
     */
//    for ( itr2 = m_clientData.begin(); itr2 != m_clientData.end(); itr2++ )
//    {
//    	std::string s = itr2->first;
//
//    	etime =  boost::posix_time::from_iso_string(itr2->second["expiry"]) +
//    			 boost::posix_time::seconds(m_clientExpirySeconds);
//
//    	if ( now > etime )
//    	{
//    		m_Logger.debugStream() << "EXPIRY TIMER: expire client session - " << s;
//    		removeClientSession(s);
//    	}
//    }


    /**
     * Remove client filters if there is no longer a client session.
     */
//    for ( itr2 = m_clientFilterData.begin(); itr2 != m_clientFilterData.end(); ++itr2 )
//    {
//    	std::string s = itr2->first;
//
//    	/*
//    	 *  If we have a filter session that does not have a client session
//    	 *  we need to remove it
//    	 */
//    	if( m_clientData.find( s ) == m_clientData.end() )
//    	{
//    		m_Logger.debugStream() << "EXPIRY TIMER: removing stale filter - " << s;
//    		clearClientFilter(s);
//    	}
//    }
//
//    for ( list_itr = m_serverDataList.begin(); list_itr != m_serverDataList.end(); ++list_itr )
//    {
//    	m_Logger.debugStream() << "EXPIRY TIMER: check m_serverDataList : " << *list_itr;
//    	/*
//    	 * If find that one of the listresp items does NOT exist in the
//    	 * main data structure, we want to purge it
//    	 */
//    	if ( m_serverData.find( *list_itr ) == m_serverData.end() )
//    	{
//    		m_Logger.debugStream() << "EXPIRY TIMER: removing stale listresp ref: " << *list_itr;
//    		m_serverDataList.remove( *list_itr );
//    	}
//    }
//

    /**
     * Display Server Sessions and Attributes
     */
//    if ( m_logServerSessions )
//    for ( itr2 = m_serverData.begin(); itr2 != m_serverData.end(); itr2++ )
//    {
//    	m_Logger.debug(" Server Session [%s]", itr2->first.c_str() );
//    	for ( attr_iter = itr2->second.begin(); attr_iter != itr2->second.end() ; attr_iter++ )
//    	{
//    		m_Logger.debug("    [%s][%s]", attr_iter->first.c_str(), attr_iter->second.c_str());
//    	}
//    }

    /**
     * Display Server Sessions and Attributes
     */
//    if ( m_logClientSessions )
//    for ( itr2 = m_clientData.begin(); itr2 != m_clientData.end(); itr2++ )
//    {
//    	m_Logger.debug(" Client Session [%s]", itr2->first.c_str() );
//    	for ( attr_iter = itr2->second.begin(); attr_iter != itr2->second.end() ; attr_iter++ )
//    	{
//    		m_Logger.debug("    [%s][%s]", attr_iter->first.c_str(), attr_iter->second.c_str());
//    	}
//    }

    /**
     * Set the next timer trigger
     */
    m_expiryTimer->expires_from_now(boost::posix_time::milliseconds(m_expiryDelayMilliseconds));
    m_expiryTimer->async_wait(boost::bind(&MetaServer::expiry_timer, this, boost::asio::placeholders::error));
}

void
MetaServer::update_timer(const boost::system::error_code& error)
{
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

	/**
	 * do update tasks
	 * possibly add a time of last update and do a delta and sleep less if we're running behind
	 */
    m_updateTimer->expires_from_now(boost::posix_time::milliseconds(m_updateDelayMilliseconds));
    m_updateTimer->async_wait(boost::bind(&MetaServer::update_timer, this, boost::asio::placeholders::error));
}


/**
 * Convenience method that evaluates what type of packet and call appropriate handle method
 * @param msp incoming metaserver packet
 * @param rsp outgoing metaserver packet to be filled
 */
void
MetaServer::processMetaserverPacket(MetaServerPacket& msp, MetaServerPacket& rsp)
{
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
	default:
		m_Logger.debug("Packet Type [%u] not supported.", msp.getPacketType());
		break;
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
MetaServer::processSERVERKEEPALIVE(MetaServerPacket& in, MetaServerPacket& out)
{

	uint32_t i = msdo.addHandshake();

	if ( i > 0 )
	{
		m_Logger.debug("processSERVERKEEPALIVE(%u)", i);
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
MetaServer::processSERVERSHAKE(MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int shake = in.getIntData(4);
	std::string ip = in.getAddressStr();
	m_Logger.debug("processSERVERSHAKE(%u)", shake);

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
		std::stringstream ss;
		ss << in.getPort();

		msdo.addServerSession(ip);
		msdo.addServerAttribute(ip,"port", ss.str() );

		// clear stream first
		ss.str("");
		ss << in.getAddressInt();

		msdo.addServerAttribute(ip,"ip_int", ss.str() );
	}

}

void
MetaServer::processTERMINATE(MetaServerPacket& in, MetaServerPacket& out)
{

	/**
	 *  For backwards compat, we make a regular "TERM" packet end a server session
	 *  and a TERM packet with any additional data sent signifies a client session.
	 */
	if ( in.getSize() > (sizeof(uint32_t)) )
	{
		m_Logger.debug("processTERMINATE-client(%s)", in.getAddressStr().c_str() );
		msdo.removeClientSession(in.getAddressStr());
	}
	else
	{
		m_Logger.debug("processTERMINATE-server(%s)", in.getAddressStr().c_str() );
		msdo.removeServerSession(in.getAddressStr());
	}

}

void
MetaServer::processCLIENTKEEPALIVE(MetaServerPacket& in, MetaServerPacket& out)
{

	uint32_t i = msdo.addHandshake();

	if ( i > 0 )
	{
		m_Logger.debug("processCLIENTKEEPALIVE(%u)", i);
		out.setPacketType(NMT_HANDSHAKE);
		out.addPacketData(i);
		out.setAddress( in.getAddress() );
	}

}

void
MetaServer::processCLIENTSHAKE(MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int shake = in.getIntData(4);
	std::string ip = in.getAddressStr();
	m_Logger.debug("processCLIENTSHAKE(%u)", shake );

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
MetaServer::processLISTREQ(MetaServerPacket& in, MetaServerPacket& out)
{
	uint32_t server_index = in.getIntData(4);
	uint32_t total = msdo.getServerSessionCount();
	uint32_t packed_max = total;
	uint32_t packed = 0;
	std::list<uint32_t> resp_list;

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
	std::list<std::string> sess_list = msdo.getServerSessionList(server_index,packed_max);
    std::list<std::string>::iterator list_itr;

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
    		 * Note: see if there is a way to do this without atoi
    		 */
    		resp_list.push_back( atoi( msdo.getServerSession(*list_itr)["ip_int"].c_str() ) );
    		++packed;
    	}

    }

    if ( packed != resp_list.size() )
    {
    	m_Logger.warn("Packed (%u) vs Response(%u) MISMATCH!", packed, resp_list.size() );
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
			m_Logger.debug("processLISTRESP(%d) - Adding", resp_list.front() );
			out.addPacketData(resp_list.front());
			resp_list.pop_front();
		}
	}
	else
	{
		/*
		 *  For the record, I think this is a stupid protocol construct
		 */
		m_Logger.debug("processLISTRESP(0,0) - Empty");
		out.addPacketData( 0 );
		out.addPacketData( 0 );
	}

}

void
MetaServer::processSERVERATTR(MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int name_length = in.getIntData(4);
	unsigned int value_length = in.getIntData(8);
	std::string msg = in.getPacketMessage(12);
	std::string name = msg.substr(0,name_length);
	std::string value = msg.substr(name_length);
	std::string ip = in.getAddressStr();
	m_Logger.debug("processSERVERATTR(%s,%s)", name.c_str(), value.c_str() );
	msdo.addServerAttribute(ip,name,value);

	out.setPacketType(NMT_NULL);
}

void
MetaServer::processCLIENTATTR(MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int name_length = in.getIntData(4);
	unsigned int value_length = in.getIntData(8);
	std::string msg = in.getPacketMessage(12);
	std::string name = msg.substr(0,name_length);
	std::string value = msg.substr(name_length);
	std::string ip = in.getAddressStr();
	m_Logger.debug("processCLIENTATTR(%s,%s)", name.c_str(), value.c_str() );
	msdo.addClientAttribute(ip,name,value);

	out.setPacketType(NMT_NULL);
}

void
MetaServer::processCLIENTFILTER(MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int name_length = in.getIntData(4);
	unsigned int value_length = in.getIntData(8);
	std::string msg = in.getPacketMessage(12);
	std::string name = msg.substr(0,name_length);
	std::string value = msg.substr(name_length);
	std::string ip = in.getAddressStr();
	m_Logger.debug("processCLIENTFILTER(%s,%s)", name.c_str(), value.c_str() );
	msdo.addClientFilter(ip,name,value);

	out.setPacketType(NMT_NULL);
}


//uint32_t
//MetaServer::addHandshake()
//{
//	// generate random #, will not be 0 ( i hope )
//	unsigned int handshake = rand();
//	unsigned int ret = 0;
//
//	// set expiry in data structure
//	m_handshakeQueue[handshake]["expiry"] = boost::posix_time::to_iso_string( getNow() );
//
//	// if we find said element again, return handshake, otherwise 0
//	if ( m_handshakeQueue[handshake].find("expiry") != m_handshakeQueue[handshake].end() )
//	{
//		m_Logger.debug("addHandshake(%u)", handshake );
//		ret = handshake;
//	}
//
//	return ret;
//}

//void
//MetaServer::removeHandshake(unsigned int hs)
//{
//	m_Logger.debug("removeHandshake(%u), %d", hs, m_handshakeQueue.size() );
//	unsigned int res = m_handshakeQueue.erase(hs);
//	if ( res == 1 )
//	{
//		m_Logger.debug("removeHandshake(%u), %d", hs, m_handshakeQueue.size() );
//	}
//}

//void
//MetaServer::addServerAttribute(std::string sessionid, std::string name, std::string value )
//{
//	/**
//	 *  check if session exists
//	 *  check if attr exists
//	 *  	if yes, update
//	 *  	if no, insert
//	 */
//	m_serverData[sessionid][name] = value;
//}

//void
//MetaServer::removeServerAttribute(std::string sessionid, std::string name )
//{
//	/**
//	 * 	Some attributes are protected
//	 * 	ip
//	 * 	port
//	 * 	expiry
//	 */
//	if ( m_serverData.find(sessionid) != m_serverData.end() )
//	{
//		if ( name != "ip" && name != "expiry" && name != "port" )
//		{
//			m_serverData[sessionid].erase(name);
//		}
//
//	}
//}

//void
//MetaServer::addClientAttribute(std::string sessionid, std::string name, std::string value )
//{
//	/**
//	 *  check if session exists
//	 *  check if attr exists
//	 *  	if yes, update
//	 *  	if no, insert
//	 */
//	m_clientData[sessionid][name] = value;
//}

//void
//MetaServer::removeClientAttribute(std::string sessionid, std::string name )
//{
//	/**
//	 * 	Some attributes are protected
//	 * 	ip
//	 * 	port
//	 * 	expiry
//	 */
//	if ( m_clientData.find(sessionid) != m_clientData.end() )
//	{
//		if ( name != "ip" && name != "expiry" && name != "port" )
//		{
//			m_clientData[sessionid].erase(name);
//		}
//
//	}
//}
//
//void
//MetaServer::addClientFilter(std::string sessionid, std::string name, std::string value )
//{
//	m_clientFilterData[sessionid][name] = value;
//}
//
//void
//MetaServer::removeClientFilter(std::string sessionid, std::string name )
//{
//	if ( m_clientFilterData.find(sessionid) != m_clientFilterData.end() )
//	{
//		m_clientFilterData[sessionid].erase(name);
//	}
//}
//
//void
//MetaServer::clearClientFilter(std::string sessionid)
//{
//	if ( m_clientFilterData.find(sessionid) != m_clientFilterData.end() )
//	{
//		m_clientFilterData.erase(sessionid);
//	}
//}
//
//
//void
//MetaServer::addServerSession(std::string ip)
//{
//	/*
//	 *  If the server session does not exist, create it, and add+uniq the listresp
//	 */
//	if ( m_serverData.find(ip) == m_serverData.end() )
//	{
//		addServerAttribute(ip,"ip",ip);
//		m_serverDataList.push_back(ip);
//
//		/*
//		 *  This is a precautionary action in case there is a slip in logic
//		 *  that allows any dups.
//		 */
//		m_serverDataList.unique();
//		m_Logger.info("Adding Server Session");
//	}
//
//	/*
//	 *  If a new structure, this will create the expiry, if existing it will just
//	 *  refresh the timeout
//	 */
//	addServerAttribute(ip,"expiry", boost::posix_time::to_iso_string( getNow() ) );
//
//}

//void
//MetaServer::removeServerSession(std::string sessionid)
//{
//	m_serverDataList.remove(sessionid);
//	if(  m_serverData.erase(sessionid) == 1 )
//	{
//		m_Logger.debug("removeServerSession(%s)", sessionid.c_str() );
//	}
//}
//
//void
//MetaServer::addClientSession(std::string ip)
//{
//	addClientAttribute(ip,"ip",ip);
//	addClientAttribute(ip,"expiry", boost::posix_time::to_iso_string( getNow() ) );
//
//}
//
//void
//MetaServer::removeClientSession(std::string sessionid)
//{
//	if( m_clientData.erase(sessionid) == 1 )
//	{
//		m_Logger.debug("removeClientSession(%s)", sessionid.c_str() );
//		clearClientFilter(sessionid);
//	}
//}

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


	if( vm.count("server.logfile") )
	{
		m_Logfile = vm["server.logfile"].as<std::string>();

		/**
		 * I tried to use boost::filesystem here, but it is so very stupid
		 * that I have opted for the more brittle way, because at least it works.
		 *
		 * TODO: add ifdef WIN32 here if/when metserver needs to run on windows
		 */
		if ( m_Logfile.substr(0,2) == "~/")
		{
			m_Logfile.replace(0,1, std::getenv("HOME") );
		}

	}

	/**
	 * Initialize the logger to appropriately
	 */
	initLogger();

	/**
	 * Print out the startup values
	 */
	m_Logger.info("WorldForge MetaServer Runtime Configuration");
	for (boost::program_options::variables_map::iterator it=vm.begin(); it!=vm.end(); ++it )
	{
		if ( it->second.value().type() == typeid(int) )
		{
			m_Logger.info( "%s = %u", it->first.c_str(), it->second.as<int>());
		}
		else if (it->second.value().type() == typeid(std::string) )
		{
			m_Logger.info( "%s = %s", it->first.c_str(), it->second.as<std::string>().c_str() );
		}
	}
}

void
MetaServer::initLogger()
{
	/*
	if ( m_LogAppender )
	m_LogAppender->close
		delete m_LogAppender;

	if ( m_LoggerLayout )
		delete m_LoggerLayout;
		*/

	m_Logger.setAdditivity(false);
	m_LoggerLayout = new log4cpp::BasicLayout();

	/**
	 * If a logfile is specified, use it, otherwise stdout.
	 * TODO: change this ... it leaks
	 */
	if ( !m_Logfile.empty() )
	{
		m_LogAppender = new log4cpp::FileAppender("FileAppender", m_Logfile.c_str() );
	}
	else
	{
		m_LogAppender = new log4cpp::OstreamAppender("OstreamAppender", &std::cout);
	}

	m_LogAppender->setLayout(m_LoggerLayout);
	m_Logger.setAppender(m_LogAppender);
	m_Logger.setPriority(log4cpp::Priority::DEBUG);
	m_Logger.info("MetaServer logging initialised [%s][%s]", m_LogAppender->getName().c_str(), m_Logfile.c_str() );
	std::cout << "MetaServer logging initialised [" << m_LogAppender->getName() << "]["
              <<  m_Logfile << "]" << std::endl;

}

log4cpp::Category&
MetaServer::getLogger()
{
	return m_Logger;
}

//void
//MetaServer::dumpHandshake()
//{
//    std::map<unsigned int, std::map<std::string,std::string> >::iterator itr;
//    std::map<std::string,std::string>::iterator iitr;
//
//    std::cout << "m_handshakeQueue.size() : " << m_handshakeQueue.size() << std::endl;
//
//    for( itr = m_handshakeQueue.begin(); itr != m_handshakeQueue.end(); itr++)
//    {
//
//        std::cout << itr->first << std::endl;
//        for ( iitr = itr->second.begin() ; iitr != itr->second.end(); iitr++)
//        {
//           std::cout << "\t" << iitr->first << "\t\t\t" << iitr->second << std::endl;
//        }
//
//    }
//}

//boost::posix_time::ptime
//MetaServer::getNow()
//{
//	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
//	return now;
//}

bool
MetaServer::isDaemon()
{
	return m_isDaemon;
}

