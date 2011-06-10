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
	  m_maxClientSessions(4096)
{
	m_serverData.clear();
	m_clientData.clear();
	m_serverDataList.clear();
	m_expiryTimer = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	m_expiryTimer->async_wait(boost::bind(&MetaServer::expiry_timer, this, boost::asio::placeholders::error));
	m_updateTimer = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	m_updateTimer->async_wait(boost::bind(&MetaServer::update_timer, this, boost::asio::placeholders::error));

	srand( (unsigned)time(0));

}

MetaServer::~MetaServer()
{
	std::cout << "dtor" << std::endl;
}

void
MetaServer::expiry_timer(const boost::system::error_code& error)
{
	boost::posix_time::ptime now = getNow();
	boost::posix_time::ptime etime;
	std::map<std::string,std::string>::iterator attr_iter;

	/**
	 * Go over handshake queue ... expire any that are older than m_handshakeExpirySeconds
	 */
    std::map<unsigned int,std::map<std::string,std::string> >::iterator itr;
    for( itr = m_handshakeQueue.begin(); itr != m_handshakeQueue.end(); itr++)
    {

    	unsigned int key = itr->first;
    	etime =  boost::posix_time::from_iso_string(itr->second["expiry"]) +
    			 boost::posix_time::seconds(m_handshakeExpirySeconds);

    	if ( now > etime )
    	{
    		std::cout << "     EXPIRE handshake - " << key << std::endl;
    		removeHandshake(key);
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
    		std::cout << "     EXPIRE server session - " << s << std::endl;
    		removeServerSession(s);
    	}

    }

    /**
     * Sweep the ordered list we dish out in a listresp
     */
    std::list<std::string>::iterator list_itr;
    for ( list_itr = m_serverDataList.begin(); list_itr != m_serverDataList.end(); list_itr++ )
    {
    	std::cout << "          m_serverDataList : " << *list_itr << std::endl;
    	/*
    	 * If find that one of the listresp items does NOT exist in the
    	 * main data structure, we want to purge it
    	 */
    	if ( m_serverData.find( *list_itr ) == m_serverData.end() )
    	{
    		std::cout << "removing stale listresp ref: " << *list_itr << std::endl;
    		m_serverDataList.remove( *list_itr );
    	}
    }


    //std::cout << "=============== Sessions [" << m_serverData.size() << "]======================" << std::endl;

    for ( itr2 = m_serverData.begin(); itr2 != m_serverData.end(); itr2++ )
    {
    	std::cout << "=====> Server Session [ " << itr2->first << "]" << std::endl;
    	for ( attr_iter = itr2->second.begin(); attr_iter != itr2->second.end() ; attr_iter++ )
    	{
    		std::cout << "==========> [" << attr_iter->first << "][" << attr_iter->second << "]" << std::endl;
    	}
    }

    //std::cout << "=============================================" << std::endl;


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
	//std::cout << "m_updateTimer(" << now << ")" << std::endl;
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
		std::cout << "Packet type [" << msp.getPacketType() << "] not supported" << std::endl;
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
		std::cout << "processSERVERKEEPALIVE(" << i << ") : " << std::endl;
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
	std::cout << "SERVERSHAKE : " << shake << std::endl;

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
		std::cout << "terminate session " << in.getAddressStr() << std::endl;
		removeServerSession(in.getAddressStr());
	}

}

void
MetaServer::processCLIENTKEEPALIVE(MetaServerPacket& in, MetaServerPacket& out)
{

	uint32_t i = addHandshake();

	if ( i > 0 )
	{
		std::cout << "processCLIENTKEEPALIVE(" << i << ") : " << std::endl;
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
	std::cout << "CLIENTSHAKE : " << shake << std::endl;

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

    std::cout << "Size1 : " << resp_list.size() << std::endl;
    if ( packed != resp_list.size() )
    {
    	std::cout << "Packed [" << packed << "] Response [" << resp_list.size() << "] MISMATCH" << std::endl;
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
			//std::cout << "ADDING RESP :" << resp_list.front() << std::endl;
			out.addPacketData(resp_list.front());
			resp_list.pop_front();
		}
	}
	else
	{
		/*
		 *  For the record, I think this is a stupid protocol construct
		 */
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
		std::cout << "addHandshake(" << handshake << ") : " << std::endl;
		ret = handshake;
	}

	return ret;
}

void
MetaServer::removeHandshake(unsigned int hs)
{
	unsigned int res = m_handshakeQueue.erase(hs);
	if ( res == 1 )
	{
		std::cout << "removeHandshake : " << hs << std::endl;
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
		std::cout << "server session erased " << sessionid << std::endl;
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
		std::cout << "client session erased " << sessionid << std::endl;
	}
}

void
MetaServer::registerConfig( boost::program_options::variables_map & vm )
{

	/*
	 * Print out the startup values
	 */
	for (boost::program_options::variables_map::iterator it=vm.begin(); it!=vm.end(); ++it )
	{
		if ( it->second.value().type() == typeid(int) )
		{
			std::cout << "  " << it->first <<  "=" << it->second.as<int>() << std::endl;
		}
		else if (it->second.value().type() == typeid(std::string) )
		{
			std::cout << "  " << it->first <<  "=" << it->second.as<std::string>() << std::endl;
		}
	}

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


	if( vm.count("server.keep_client_stats") )
	{
		std::string s = vm["server.keep_client_stats"].as<std::string>();
		if ( boost::iequals(s,"true") )
		{
			m_keepClientStats = true;
		}
		else if ( boost::iequals(s,"false") )
		{
			m_keepClientStats = false;
		}

	}

	if( vm.count("server.keep_server_stats") )
	{
		std::string s = vm["server.keep_server_stats"].as<std::string>();
		if ( boost::iequals(s,"true") )
		{
			m_keepServerStats = true;
		}
		else if ( boost::iequals(s,"false") )
		{
			m_keepServerStats = false;
		}

	}

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


