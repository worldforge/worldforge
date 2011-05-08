#include "MetaServerHandlerUDP.hpp"

MetaServerHandlerUDP::MetaServerHandlerUDP(boost::asio::io_service& ios,
		              const std::string& address,
		              const unsigned int port )
   : address_(address),
     port_(port),
     socket_(ios, udp::endpoint(udp::v6(),port))
{
	std::cout << "udp handler ctor: " << std::endl;
	start_receive();
}

MetaServerHandlerUDP::~MetaServerHandlerUDP()
{
	std::cout << "meta udp handler dtor" << std::endl;
}

void
MetaServerHandlerUDP::start_receive()
{
	std::cout << "start_receive" << std::endl;

    socket_.async_receive_from(
    		boost::asio::buffer(recv_buffer_), remote_endpoint_,
    		boost::bind(&MetaServerHandlerUDP::handle_receive, this,
    		boost::asio::placeholders::error,
    		boost::asio::placeholders::bytes_transferred)
    );

}

void
MetaServerHandlerUDP::handle_receive(const boost::system::error_code& error,
									 std::size_t bytes_transferred)
{
	std::cout << "handle_receive" << std::endl;
	if(!error || error == boost::asio::error::message_size )
	{
		std::cout << "UDP-1 : read off packet [" << bytes_transferred << "]" << std::endl;
		std::cout << "UDP-2 : analyse packet" << std::endl;
		std::cout << "      : bytes [ " << bytes_transferred << " ]" << std::endl;
		std::cout << "      : from  [ " << remote_endpoint_.address().to_string() << " ]" << std::endl;
		std::cout << "      : buffer[ " << recv_buffer_ << " ]" << std::endl;

		std::cout << "UDP-3 : make call to ms object" << std::endl;
		std::cout << "UDP-4 : get response from ms" << std::endl;
		std::cout << "UDP-5 : construct client response packet" << std::endl;
		std::cout << "UDP-6 : send async response" << std::endl;
		std::cout << "UDP-7 : loop back to start recv" << std::endl;

		start_receive();
	} else {
		std::cerr << "ERROR:" << error.message() << std::endl;
	}

}
