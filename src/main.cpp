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
#include <unistd.h> /* daemon() */

/*
	Entry point
*/
int main(int argc, char** argv)
{

	std::ifstream config_file("metaserver-ng.conf");
	boost::asio::io_service io_service;
	boost::program_options::variables_map vm;

	int port = 8453;
	std::string ip = "0.0.0.0";

	/**
	 * Argument Wrangling
	 *
	 * Note: not exactly the most friendly option parsing I've seen
	 */
	boost::program_options::options_description desc( "MetaServer Configuration" );

	/**
	 * Note: options inside the configuration file that are NOT listed here
	 *       become ignored and are not accessible.
	 */
	desc.add_options()
		( "help,h", "Display help message" )
		( "server.port,p", boost::program_options::value<int>(), "Server bind port. \nDefault:8543" )
		( "server.ip", boost::program_options::value<std::string>(), "Server bind IP. \nDefault:0.0.0.0" )
		( "server.daemon", boost::program_options::value<std::string>(), "Daemonize after startup [true|false].\nDefault: true" )
		( "server.logfile", boost::program_options::value<std::string>(), "Server logfile location.\nDefault: ~/.metaserver-ng/metaserver-ng.log" )
		( "server.client_stats", boost::program_options::value<std::string>(), "Keep internals stats [true|false].  This can affect performance.\nDefault: false" )
		( "server.server_stats", boost::program_options::value<std::string>(), "Keep internals stats [true|false].  This can affect performance.\nDefault: false" )
		( "logging.server_sessions", boost::program_options::value<std::string>(), "Output server sessions to logfile [true|false].  \nDefault: false" )
		( "logging.client_sessions", boost::program_options::value<std::string>(), "Output client sessions to logfile [true|false].  \nDefault: false" )
		( "security.auth_scheme", boost::program_options::value<std::string>(), "What method of authentication to use [none|server|delegate|both].\nDefault: none" )
		( "performance.max_server_sessions", boost::program_options::value<int>(), "Max number of server sessions [1-32768].\nDefault: 1024" )
		( "performance.max_client_sessions", boost::program_options::value<int>(), "Max number of client sessions [1-32768].\nDefault: 4096")
		( "performance.server_session_expiry_seconds", boost::program_options::value<int>(), "Expiry in seconds for server sessions [300-3600].\nDefault: 300" )
		( "performance.client_session_expiry_seconds", boost::program_options::value<int>(), "Expiry in seconds for client sessions [300-3600].\nDefault: 300" )
			;

	/**
	 * Create our metaserver
	 */
	MetaServer ms(io_service);

	try
	{

		boost::program_options::store(
				boost::program_options::parse_command_line(argc, argv, desc),
				vm
				);
		boost::program_options::store(
				boost::program_options::parse_config_file(config_file, desc, true),
				vm
				);

		boost::program_options::notify(vm);

		/**
		 * Special case for help
		 */
		if ( vm.count("help") )
		{
			std::cout << desc << std::endl;
			return 1;
		}

		/**
		 * The only real options that we need to process outside of the
		 * metaesrver as it affects the handlers
		 */
		if ( vm.count("server.port") )
			port=vm["server.port"].as<int>();

		if ( vm.count("server.ip") )
			ip=vm["server.ip"].as<std::string>();

		/**
		 * Register the configuration.
		 */
		ms.registerConfig(vm);

		/**
		 * Go daemon if needed
		 */
		if ( ms.isDaemon() )
		{
			ms.getLogger().info("Running as a daemon");
			daemon(0,0);
		}

		/**
		 * Define Handlers
		 */
		MetaServerHandlerTCP tcp(ms, io_service, ip, port);
		MetaServerHandlerUDP udp(ms, io_service, ip, port);

		/**
		 * This is the async loop
		 */
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	std::cout << "All Done!" << std::endl;
	return 0;
}
