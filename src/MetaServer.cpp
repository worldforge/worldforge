#include "MetaServer.hpp"
#include "MetaServerHandlerTCP.hpp"

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
	MetaServerHandlerTCP tcp(io_service, "localhost", 6666 );

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
