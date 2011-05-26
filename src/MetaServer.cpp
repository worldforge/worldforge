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
	  m_sessionExpirySeconds(300)
{
	m_serverData.clear();
	m_clientData.clear();
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


    std::cout << "=============== Sessions [" << m_serverData.size() << "]======================" << std::endl;
    /**
    for ( itr2 = m_serverData.begin(); itr2 != m_serverData.end(); itr2++ )
    {
    	std::cout << "=====> Server Session [ " << itr2->first << "]" << std::endl;
    	for ( attr_iter = itr2->second.begin(); attr_iter != itr2->second.end() ; attr_iter++ )
    	{
    		std::cout << "==========> [" << attr_iter->first << "][" << attr_iter->second << "]" << std::endl;
    	}
    }
    std::cout << "=============================================" << std::endl;

	*/
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

	unsigned int handshake = rand();

	std::string a = in.getAddress();

	//boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	//boost::posix_time::time_duration duration( now.time_of_day() );
	//unsigned int tick = duration.total_milliseconds()

	std::map<std::string,std::string> tmp_;

	tmp_["ip"] = in.getAddress();
	tmp_["expiry"] = boost::posix_time::to_iso_string( getNow() );

	uint32_t i = addHandshake(handshake,tmp_);

	std::cout << "handshake(" << i << ") : " << handshake << std::endl;

	out.setPacketType(NMT_HANDSHAKE);
	out.addPacketData(handshake);

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
	std::string ip = in.getAddress();
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
	}

}

void
MetaServer::processTERMINATE(MetaServerPacket& in, MetaServerPacket& out)
{
	if( m_serverData.find( in.getAddress() ) != m_serverData.end() )
	{
		std::cout << "terminate session " << in.getAddress() << std::endl;
		removeServerSession(in.getAddress());
	}

}


int
MetaServer::addHandshake(unsigned int hs, std::map<std::string,std::string> attr )
{
	m_handshakeQueue[hs] = attr;
	return m_handshakeQueue.size();
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
MetaServer::addServerSession(std::string ip)
{
	addServerAttribute(ip,"ip",ip);
	addServerAttribute(ip,"expiry", boost::posix_time::to_iso_string( getNow() ) );

}

void
MetaServer::removeServerSession(std::string sessionid)
{
	unsigned int res = m_serverData.erase(sessionid);
	if( res == 1 )
	{
		std::cout << "server session erased " << sessionid << std::endl;
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


