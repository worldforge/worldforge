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

#include "MetaServer.hpp"
#include "MetaServerHandlerTCP.hpp"
#include "MetaServerHandlerUDP.hpp"

MetaServer::MetaServer(boost::asio::io_service& ios)
	: m_expiryDelayMilliseconds(1500),
	  m_updateDelayMilliseconds(4000),
	  m_handshakeExpirySeconds(30),
	  m_sessionExpirySeconds(300),
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
	m_serverData.clear();
	m_clientData.clear();
	m_serverDataList.clear();
	m_expiryTimer = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	m_expiryTimer->async_wait(boost::bind(&MetaServer::expiry_timer, this, boost::asio::placeholders::error));
	m_updateTimer = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	m_updateTimer->async_wait(boost::bind(&MetaServer::update_timer, this, boost::asio::placeholders::error));

	srand( (unsigned)time(0));

	/**
	 * NOTE: Since the configuration file parsing comes after the instance is created,
	 * we need to create the logging ( ala console ) by default, and then if
	 * the configuration is loaded, and it contains a log file, we re-initialise the
	 * logger to log to a file.  This is double initialisation, however I don't
	 * see a way around it without having a mandatory log file.
	 */
	//initLogger();

}

MetaServer::~MetaServer()
{
	m_Logger.info("Shutting down logging");
	log4cpp::Category::shutdown();
}

void
MetaServer::expiry_timer(const boost::system::error_code& error)
{
	boost::posix_time::ptime now = getNow();
	boost::posix_time::ptime etime;
	std::map<std::string,std::string>::iterator attr_iter;

	/**
	 * Output tick
	 */
	//m_Logger.debugStream() << "EXPIRY TIMER: " << now ;

	/**
	 * Go over handshake queue ... expire any that are older than m_handshakeExpirySeconds
	 */
    std::map<unsigned int,std::map<std::string,std::string> >::iterator itr;
    for( itr = m_handshakeQueue.begin(); itr != m_handshakeQueue.end(); )
    {
    	unsigned int key = itr->first;
    	etime =  boost::posix_time::from_iso_string(itr->second["expiry"]) +
    			 boost::posix_time::seconds(m_handshakeExpirySeconds);

    	/**
    	 * We need to make a copy of the iterator if we modify the
    	 * underlying container because the iterator becomes invalid
    	 */
    	if ( now > etime )
    	{
    		std::map<unsigned int,std::map<std::string,std::string> >::iterator itr_copy = itr;
    		++itr_copy;
    		m_Logger.infoStream() << "EXPIRY TIMER: expire handshake - " << key;
    		removeHandshake(key);
    		itr = itr_copy;
    	}
    	else
    	{
    		/**
    		 * We are not modifying, just increment normally.
    		 */
    		++itr;
    	}

    }

    /**
     * Sweep for server sessions ... expire any that are older than m_sessionExpirySeconds
     */

    std::map<std::string,std::map<std::string,std::string> >::iterator itr2;
    for ( itr2 = m_serverData.begin(); itr2 != m_serverData.end(); itr2++ )
    {
    	std::string s = itr2->first;

    	etime =  boost::posix_time::from_iso_string(itr2->second["expiry"]) +
    			 boost::posix_time::seconds(m_sessionExpirySeconds);

    	if ( now > etime )
    	{
    		m_Logger.debugStream() << "EXPIRY TIMER: expire server session - " << s;
    		removeServerSession(s);
    	}
    }

    /**
     * Sweep the ordered list we dish out in a listresp
     */
    std::list<std::string>::iterator list_itr;
    for ( list_itr = m_serverDataList.begin(); list_itr != m_serverDataList.end(); ++list_itr )
    {
    	m_Logger.debugStream() << "EXPIRY TIMER: check m_serverDataList : " << *list_itr;
    	/*
    	 * If find that one of the listresp items does NOT exist in the
    	 * main data structure, we want to purge it
    	 */
    	if ( m_serverData.find( *list_itr ) == m_serverData.end() )
    	{
    		m_Logger.debugStream() << "EXPIRY TIMER: removing stale listresp ref: " << *list_itr;
    		m_serverDataList.remove( *list_itr );
    	}
    }

    /**
     * Display Server Sessions and Attributes
     */
    if ( m_logServerSessions )
    for ( itr2 = m_serverData.begin(); itr2 != m_serverData.end(); itr2++ )
    {
    	m_Logger.debug(" Server Session [%s]", itr2->first.c_str() );
    	for ( attr_iter = itr2->second.begin(); attr_iter != itr2->second.end() ; attr_iter++ )
    	{
    		m_Logger.debug("    [%s][%s]", attr_iter->first.c_str(), attr_iter->second.c_str());
    	}
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
	default:
		m_Logger.debug("Packet Type [%u] not supported.", msp.getPacketType());
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

	uint32_t i = addHandshake();

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
	if( m_handshakeQueue.find(shake) != m_handshakeQueue.end() )
	{
		std::stringstream ss;
		ss << in.getPort();

		addServerSession(ip);
		addServerAttribute(ip,"port", ss.str() );

		// clear stream first
		ss.str("");
		ss << in.getAddressInt();

		addServerAttribute(ip,"ip_int", ss.str() );
	}

}

void
MetaServer::processTERMINATE(MetaServerPacket& in, MetaServerPacket& out)
{
	if( m_serverData.find( in.getAddressStr() ) != m_serverData.end() )
	{
		m_Logger.debug("processTERMINATE(%s)", in.getAddressStr().c_str() );
		removeServerSession(in.getAddressStr());
	}

}

void
MetaServer::processCLIENTKEEPALIVE(MetaServerPacket& in, MetaServerPacket& out)
{

	uint32_t i = addHandshake();

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

	if( m_handshakeQueue.find(shake) != m_handshakeQueue.end() )
	{
		std::stringstream ss;
		ss << in.getPort();

		addClientSession(ip);
		addClientAttribute(ip,"port", ss.str() );
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
	uint32_t total = m_serverDataList.size();
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


    std::list<std::string>::iterator list_itr = m_serverDataList.begin();

    /**
     * HACK ALERT : I have to re-think this ... i need the random iterator of a vector
     * but also the unique() of a list ... what I really want to do is:
     * list_itr = m_serverDataList.begin() + server_index in order to offset the starting
     * location by the index without losing the list functionality.
     */
    for ( int i = 0; i < server_index; i++ )
    {
    	++list_itr;
    }

    for ( ; list_itr != m_serverDataList.end(); list_itr++ )
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
    	if ( m_serverData.find( *list_itr ) != m_serverData.end() )
    	{
    		/*
    		 * Note: see if there is a way to do this without atoi
    		 */
    		resp_list.push_back( atoi(m_serverData[*list_itr]["ip_int"].c_str() ) );
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
		out.addPacketData( m_serverDataList.size() );
		out.addPacketData( (uint32_t)resp_list.size() );
		while ( ! resp_list.empty() )
		{
			m_Logger.debug("processLISTRESP(%s) - Adding", resp_list.front() );
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


uint32_t
MetaServer::addHandshake()
{
	// generate random #, will not be 0 ( i hope )
	unsigned int handshake = rand();
	unsigned int ret = 0;

	// set expiry in data structure
	m_handshakeQueue[handshake]["expiry"] = boost::posix_time::to_iso_string( getNow() );

	// if we find said element again, return handshake, otherwise 0
	if ( m_handshakeQueue[handshake].find("expiry") != m_handshakeQueue[handshake].end() )
	{
		m_Logger.debug("addHandshake(%u)", handshake );
		ret = handshake;
	}

	return ret;
}

void
MetaServer::removeHandshake(unsigned int hs)
{
	m_Logger.debug("removeHandshake(%u), %d", hs, m_handshakeQueue.size() );
	unsigned int res = m_handshakeQueue.erase(hs);
	if ( res == 1 )
	{
		m_Logger.debug("removeHandshake(%u), %d", hs, m_handshakeQueue.size() );
	}
}

void
MetaServer::addServerAttribute(std::string sessionid, std::string name, std::string value )
{
	/**
	 *  check if session exists
	 *  check if attr exists
	 *  	if yes, update
	 *  	if no, insert
	 */
	m_serverData[sessionid][name] = value;
}

void
MetaServer::removeServerAttribute(std::string sessionid, std::string name )
{
	/**
	 * 	Some attributes are protected
	 * 	ip
	 * 	port
	 * 	expiry
	 */
	if ( m_serverData.find(sessionid) != m_serverData.end() )
	{
		if ( name != "ip" && name != "expiry" && name != "port" )
		{
			m_serverData[sessionid].erase(name);
		}

	}
}

void
MetaServer::addClientAttribute(std::string sessionid, std::string name, std::string value )
{
	/**
	 *  check if session exists
	 *  check if attr exists
	 *  	if yes, update
	 *  	if no, insert
	 */
	m_clientData[sessionid][name] = value;
}

void
MetaServer::removeClientAttribute(std::string sessionid, std::string name )
{
	/**
	 * 	Some attributes are protected
	 * 	ip
	 * 	port
	 * 	expiry
	 */
	if ( m_clientData.find(sessionid) != m_clientData.end() )
	{
		if ( name != "ip" && name != "expiry" && name != "port" )
		{
			m_clientData[sessionid].erase(name);
		}

	}
}


void
MetaServer::addServerSession(std::string ip)
{
	/*
	 *  If the server session does not exist, create it, and add+uniq the listresp
	 */
	if ( m_serverData.find(ip) == m_serverData.end() )
	{
		addServerAttribute(ip,"ip",ip);
		m_serverDataList.push_back(ip);

		/*
		 *  This is a precautionary action in case there is a slip in logic
		 *  that allows any dups.
		 */
		m_serverDataList.unique();
		m_Logger.info("Adding Server Session");
	}

	/*
	 *  If a new structure, this will create the expiry, if existing it will just
	 *  refresh the timeout
	 */
	addServerAttribute(ip,"expiry", boost::posix_time::to_iso_string( getNow() ) );

}

void
MetaServer::removeServerSession(std::string sessionid)
{
	m_serverDataList.remove(sessionid);
	if(  m_serverData.erase(sessionid) == 1 )
	{
		m_Logger.debug("removeServerSession(%s)", sessionid.c_str() );
	}
}

void
MetaServer::addClientSession(std::string ip)
{
	addClientAttribute(ip,"ip",ip);
	addClientAttribute(ip,"expiry", boost::posix_time::to_iso_string( getNow() ) );

}

void
MetaServer::removeClientSession(std::string sessionid)
{
	if( m_clientData.erase(sessionid) == 1 )
	{
		m_Logger.debug("removeClientSession(%s)", sessionid.c_str() );
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

void
MetaServer::dumpHandshake()
{
    std::map<unsigned int, std::map<std::string,std::string> >::iterator itr;
    std::map<std::string,std::string>::iterator iitr;

    std::cout << "m_handshakeQueue.size() : " << m_handshakeQueue.size() << std::endl;

    for( itr = m_handshakeQueue.begin(); itr != m_handshakeQueue.end(); itr++)
    {

        std::cout << itr->first << std::endl;
        for ( iitr = itr->second.begin() ; iitr != itr->second.end(); iitr++)
        {
           std::cout << "\t" << iitr->first << "\t\t\t" << iitr->second << std::endl;
        }

    }
}

boost::posix_time::ptime
MetaServer::getNow()
{
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	return now;
}

bool
MetaServer::isDaemon()
{
	return m_isDaemon;
}

