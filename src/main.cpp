/*
 * main.cpp
 *
 *  Created on: 2011-05-19
 *      Author: sryan
 */

#include "MetaServer.hpp"
#include "MetaServerHandlerTCP.hpp"
#include "MetaServerHandlerUDP.hpp"

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
