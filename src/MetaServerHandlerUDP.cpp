#include "MetaServerHandlerUDP.hpp"

MetaServerHandlerUDP::MetaServerHandlerUDP(MetaServer& ms,
					  boost::asio::io_service& ios,
		              const std::string& address,
		              const unsigned int port )
   : m_msRef(ms),
	 m_Address(address),
     m_Port(port),
     m_Socket(ios, udp::endpoint(udp::v6(),port)),
     m_outboundTick(0),
     m_outboundMaxInterval(100)
{
	m_outboundTimer = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	m_outboundTimer->async_wait(boost::bind(&MetaServerHandlerUDP::process_outbound, this, boost::asio::placeholders::error));

	start_receive();
}

MetaServerHandlerUDP::~MetaServerHandlerUDP()
{

}

void
MetaServerHandlerUDP::start_receive()
{

    m_Socket.async_receive_from(
    		boost::asio::buffer(m_recvBuffer), m_remoteEndpoint,
    		boost::bind(&MetaServerHandlerUDP::handle_receive, this,
    		boost::asio::placeholders::error,
    		boost::asio::placeholders::bytes_transferred)
    );

}

void
MetaServerHandlerUDP::handle_receive(const boost::system::error_code& error,
									 std::size_t bytes_recvd)
{
	if(!error || error == boost::asio::error::message_size )
	{

		std::cout << "UDP : Incoming packet  [" << bytes_recvd << "]" << std::endl;
		std::cout << "      : from  [ " << m_remoteEndpoint.address().to_string() << " ]" << std::endl;
		std::cout << "      : port  [ " << m_remoteEndpoint.port() << " ]" << std::endl;


		/**
		 *  Standard network pump, receive data, evaluate, send response
		 */
		MetaServerPacket msp( m_recvBuffer , bytes_recvd );
		msp.setAddress(m_remoteEndpoint.address().to_string());
		msp.setPort(m_remoteEndpoint.port());

		std::cout << "      : type  [ " << msp.getPacketType() << " ]" << std::endl;
		std::cout << "      : size  [ " << msp.getSize() << " ]" << std::endl;
		std::cout << "      : dump: " << msp.getIntData() << std::endl;

		std::cout << "-----------------------------------------------" << std::endl;

		boost::array<char, MAX_PACKET_BYTES> send_buffer;

		MetaServerPacket rsp( send_buffer );

		/**
		 * The logic for what happens is inside the metaserver class
		 */
		m_msRef.processMetaserverPacket(msp,rsp);

		std::cout << "UDP : Outgoing packet  [" << rsp.getSize() << "]" << std::endl;
		std::cout << "      : to  [ " << m_remoteEndpoint.address().to_string() << " ]" << std::endl;
		std::cout << "      : port  [ " << m_remoteEndpoint.port() << " ]" << std::endl;
		std::cout << "      : type  [ " << rsp.getPacketType() << " ]" << std::endl;
		std::cout << "      : size  [ " << rsp.getSize() << " ]" << std::endl;
		std::cout << "      : dump: " << rsp.getIntData() << std::endl;

		std::cout << "===============================================" << std::endl;


		/**
		 * Send back response
		 */
	      m_Socket.async_send_to(boost::asio::buffer(send_buffer.data(),rsp.getSize()), m_remoteEndpoint,
	          boost::bind(&MetaServerHandlerUDP::handle_send, this, send_buffer,
	            boost::asio::placeholders::error,
	            boost::asio::placeholders::bytes_transferred));

		/**
		 *	Back to async read
		 */
		start_receive();

	} else {
		std::cerr << "ERROR:" << error.message() << std::endl;
	}

}

void
MetaServerHandlerUDP::handle_send(NetMsgType nmt, const boost::system::error_code& error, std::size_t bytes_sent)
{
	std::cout << "UDP: sent bytes : " << bytes_sent << std::endl;
}

/**
 * This is is the handler for when we process packets and queue up responses for
 * times of higher load, or in the case of a list where we want to queue up multiple
 * response packets for a given incoming packet.
 *
 * note: this may not be required, as we're using async transmission
 * @param error
 */
void
MetaServerHandlerUDP::process_outbound(const boost::system::error_code& error)
{
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration duration( now.time_of_day() );
	unsigned long tick = duration.total_milliseconds();
	unsigned int delay = m_outboundMaxInterval;

	/*
	 *  Process the outbound response packets
	 */

	/*
	 *  Adjust timer for next loop
	 */

	if( (tick - m_outboundTick) < m_outboundMaxInterval )
	{
		delay = tick - m_outboundTick;
	}

	m_outboundTimer->expires_from_now(boost::posix_time::milliseconds(delay));
    m_outboundTimer->async_wait(boost::bind(&MetaServerHandlerUDP::process_outbound, this, boost::asio::placeholders::error));
}



