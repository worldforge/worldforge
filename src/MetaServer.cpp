#include "MetaServer.hpp"
#include "MetaServerHandlerTCP.hpp"
#include "MetaServerHandlerUDP.hpp"

MetaServer::MetaServer(boost::asio::io_service& ios)
	: expiry_timer_delay_milliseconds_(1500),
	  update_timer_delay_milliseconds_(4000),
	  m_handshakeExpirySeconds(30)
{
	ms_data_.clear();
	expiry_timer_ = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	expiry_timer_->async_wait(boost::bind(&MetaServer::expiry_timer, this, boost::asio::placeholders::error));
	update_timer_ = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	update_timer_->async_wait(boost::bind(&MetaServer::update_timer, this, boost::asio::placeholders::error));

	srand( (unsigned)time(0));

}

MetaServer::~MetaServer()
{
	std::cout << "dtor" << std::endl;
}

void
MetaServer::expiry_timer(const boost::system::error_code& error)
{
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

	/**
	 * Go over handshake queue ... expire any that are older than m_handshakeExpirySeconds
	 */
    std::map<unsigned int,std::map<std::string,std::string> >::iterator itr;
    std::map<std::string,std::string>::iterator iitr;

    //std::cout << "HSQ: " << m_handshakeQueue.size() << std::endl;

    for( itr = m_handshakeQueue.begin(); itr != m_handshakeQueue.end(); itr++)
    {

    	unsigned int key = itr->first;
    	boost::posix_time::ptime etime( boost::posix_time::from_iso_string(itr->second["expiry"]) +
    									 boost::posix_time::seconds(m_handshakeExpirySeconds));

    	if ( now > etime )
    	{
    		std::cout << "     EXPIRE handshake - " << key << std::endl;
    		removeHandshake(key);
    	}

    }

    expiry_timer_->expires_from_now(boost::posix_time::milliseconds(expiry_timer_delay_milliseconds_));
    expiry_timer_->async_wait(boost::bind(&MetaServer::expiry_timer, this, boost::asio::placeholders::error));
}

void
MetaServer::update_timer(const boost::system::error_code& error)
{
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	//std::cout << "update_timer_(" << now << ")" << std::endl;
	/**
	 * do update tasks
	 * possibly add a time of last update and do a delta and sleep less if we're running behind
	 */
    update_timer_->expires_from_now(boost::posix_time::milliseconds(update_timer_delay_milliseconds_));
    update_timer_->async_wait(boost::bind(&MetaServer::update_timer, this, boost::asio::placeholders::error));
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

	//unsigned int tick = duration.total_milliseconds();

	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

	std::map<std::string,std::string> tmp_;

	tmp_["ip"] = in.getAddress();
	tmp_["expiry"] = boost::posix_time::to_iso_string(now);

	uint32_t i = addHandshake(handshake,tmp_);

	std::cout << "handshake(" << i << ") : " << handshake << std::endl;

	out.setPacketType(NMT_HANDSHAKE);
	out.addPacketData(handshake);
	//out.dumpBuffer();

}

void
MetaServer::processSERVERSHAKE(MetaServerPacket& in, MetaServerPacket& out)
{
	unsigned int shake = in.getIntData(4);
	std::string ip = in.getAddress();
	std::cout << "SERVERSHAKE : " << shake << std::endl;
	removeHandshake(shake);
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
	std::cout << "removeHandshake : " << hs << std::endl;
	m_handshakeQueue.erase(hs);
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

