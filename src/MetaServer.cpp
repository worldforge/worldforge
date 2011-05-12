#include "MetaServer.hpp"
#include "MetaServerHandlerTCP.hpp"
#include "MetaServerHandlerUDP.hpp"

MetaServer::MetaServer(boost::asio::io_service& ios)
	: tick_delay_milliseconds_(200)
{
	ms_data_.clear();
	ticker_ = new boost::asio::deadline_timer(ios, boost::posix_time::seconds(1));
	ticker_->async_wait(boost::bind(&MetaServer::tick, this, boost::asio::placeholders::error));

}

MetaServer::~MetaServer()
{
	std::cout << "dtor" << std::endl;
}

void
MetaServer::tick(const boost::system::error_code& error)
{
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	std::cout << "tick(" << now << ")" << std::endl;
    ticker_->expires_from_now(boost::posix_time::milliseconds(tick_delay_milliseconds_));
    ticker_->async_wait(boost::bind(&MetaServer::tick, this, boost::asio::placeholders::error));
}

/*
	Entry point
*/
int main(int argc, char** argv)
{
	std::cout << "Start" << std::endl;
	boost::asio::io_service io_service;

	MetaServer ms(io_service);

	MetaServerHandlerTCP tcp(ms, io_service, "192.168.1.200", 8453);
	MetaServerHandlerUDP udp(ms, io_service, "192.168.1.200", 8453);


	try
	{
		std::cout << "do stuff" << std::endl;
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	std::cout << "End" << std::endl;
	return 0;
}
