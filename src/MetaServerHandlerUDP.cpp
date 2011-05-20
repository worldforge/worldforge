#include "MetaServerHandlerUDP.hpp"

MetaServerHandlerUDP::MetaServerHandlerUDP(MetaServer& ms,
					  boost::asio::io_service& ios,
		              const std::string& address,
		              const unsigned int port )
   : m_msRef(ms),
	 m_Address(address),
     m_Port(port),
     m_Socket(ios, udp::endpoint(udp::v6(),port))
{
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


		boost::array<char, MAX_PACKET_BYTES> send_buffer;
		boost::array<char, MAX_PACKET_BYTES> bs;

		/**
		 *   1) stupid thing ... this is what the damn memcpy should be doing
		 *   2) instead of directly writing back the packet push onto an outbound queue
		 *   3) change / create one of the timers to process the outbound queue
		 *      - have an interval to set for the next timer time, if busy you want to
		 *      have a faster timer
		 */
		bs[0] = 0;
		bs[1] = 0;
		bs[2] = 0;
		bs[3] = 3;
		bs[4] = 0;
		bs[5] = 0;
		bs[6] = 0;
		bs[7] = 7;

		MetaServerPacket rsp( send_buffer );

		/**
		 * The original packet object is recycled into the response
		 */
		m_msRef.processMetaserverPacket(msp,rsp);

		for ( int i = 0; i<rsp.getSize(); ++i )
		{
			std::cout << "      : buffer val-" << i << " [" << send_buffer.at(i) << "]" << std::endl;
		}

		std::cout << "      : rsp size  [ " << rsp.getSize() << " ]" << std::endl;
		std::cout << "      : rsp type  [ " << rsp.getPacketType() << " ]" << std::endl;
		std::cout << "      : rsp parse random  [ " << rsp.getIntData() << " ]" << std::endl;

		/**
		 * Send back response
		 */
	      m_Socket.async_send_to(boost::asio::buffer(bs,rsp.getSize()), m_remoteEndpoint,
	          boost::bind(&MetaServerHandlerUDP::handle_send, this, bs,
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
MetaServerHandlerUDP::handle_send(boost::array<char, MAX_PACKET_BYTES> buf, const boost::system::error_code& error, std::size_t bytes_sent)
{
	std::cout << "UDP: sent bytes : " << bytes_sent << std::endl;
}
