#include "MetaServerHandlerUDP.hpp"

MetaServerHandlerUDP::MetaServerHandlerUDP(MetaServer& ms,
					  boost::asio::io_service& ios,
		              const std::string& address,
		              const unsigned int port )
   : ms_ref_(ms),
	 address_(address),
     port_(port),
     socket_(ios, udp::endpoint(udp::v6(),port))
{
	start_receive();
}

MetaServerHandlerUDP::~MetaServerHandlerUDP()
{

}

void
MetaServerHandlerUDP::start_receive()
{

    socket_.async_receive_from(
    		boost::asio::buffer(recv_buffer_), remote_endpoint_,
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

		std::cout << "UDP-1 : read off packet [" << bytes_recvd << "]" << std::endl;
		std::cout << "UDP-2 : analyse packet" << std::endl;
		std::cout << "      : bytes [ " << bytes_recvd << " ]" << std::endl;
		std::cout << "      : bytes [ " << recv_buffer_.size() << " ]" << std::endl;
		std::cout << "      : from  [ " << remote_endpoint_.address().to_string() << " ]" << std::endl;
		std::cout << "      : port  [ " << remote_endpoint_.port() << " ]" << std::endl;
		std::cout << "      : sizeof int " << sizeof(int) << std::endl;

		/**
		 *  Standard network pump, receive data, evaluate, send response
		 */
		MetaServerPacket msp( recv_buffer_, bytes_recvd );
		msp.setAddress(remote_endpoint_.address().to_string());

		/**
		 * Create empty packet and buffer
		 */
		boost::array<char, MAX_PACKET_BYTES> send_buffer;
		MetaServerPacket rsp( send_buffer );

		ms_ref_.processMetaserverPacket(msp,rsp);

		/**
		 * Send back response
		 */
	      /*socket_.async_send_to(boost::asio::buffer(send_buffer), remote_endpoint_,
	          boost::bind(&MetaServerHandlerUDP::handle_send, this, send_buffer,
	            boost::asio::placeholders::error,
	            boost::asio::placeholders::bytes_transferred)); */

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
