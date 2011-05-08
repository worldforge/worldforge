#include "MetaServer.hpp"
#include "MetaServerHandlerTCP.hpp"
#include "MetaServerHandlerUDP.hpp"

MetaServer::MetaServer()
{
	server_list_.clear();
}

MetaServer::~MetaServer()
{
	std::cout << "dtor" << std::endl;
}


/*
	Entry point
*/
int main(int argc, char** argv)
{
	std::cout << "Start" << std::endl;
	boost::asio::io_service io_service;
	MetaServerHandlerTCP tcp(io_service, "192.168.1.200", 8453);
	MetaServerHandlerUDP udp(io_service, "192.168.1.200", 8453);

	MetaServer ms();

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
