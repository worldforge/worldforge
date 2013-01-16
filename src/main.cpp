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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 */

/*
 * Local Includes
 */
#include "MetaServerProtocol.hpp"
#include "MetaServer.hpp"
#include "MetaServerHandlerTCP.hpp"
#include "MetaServerHandlerUDP.hpp"

/*
 * System Includes
 */
#include <unistd.h> /* daemon() */
#include <cstdlib> /* getenv() because boost po env parsing sucks */
#include <fstream>
#include <glog/logging.h>
#include <boost/filesystem.hpp>

/*
	Entry point
*/
int main(int argc, char** argv)
{

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
		( "logging.packet_logging", boost::program_options::value<std::string>(), "Output all packets to logfile [true|false].  \nDefault: false" )
		( "logging.packet_logfile", boost::program_options::value<std::string>(), "Packet logfile location.\nDefault: ~/.metaserver-ng/packetlog.bin" )
		( "security.auth_scheme", boost::program_options::value<std::string>(), "What method of authentication to use [none|server|delegate|both].\nDefault: none" )
		( "performance.max_server_sessions", boost::program_options::value<int>(), "Max number of server sessions [1-32768].\nDefault: 1024" )
		( "performance.max_client_sessions", boost::program_options::value<int>(), "Max number of client sessions [1-32768].\nDefault: 4096")
		( "performance.server_session_expiry_seconds", boost::program_options::value<int>(), "Expiry in seconds for server sessions [300-3600].\nDefault: 300" )
		( "performance.client_session_expiry_seconds", boost::program_options::value<int>(), "Expiry in seconds for client sessions [300-3600].\nDefault: 300" )
			;

	/*
	 * Initialize Logging
	 */
	google::InitGoogleLogging("");


	/*
	 * Environment Wrangling
	 */
	const char *wfenv = std::getenv("WFHOME");
	const char *home  = std::getenv("HOME");
	bool config_found = false;
	boost::filesystem::path config_file_path;
	boost::filesystem::path home_dot_dir;

	/*
	 * Create a 'special' directory
	 */
	if ( home != NULL )
	{

		home_dot_dir = home;
		home_dot_dir /= "/.metaserver-ng";

		if ( ! boost::filesystem::exists(home_dot_dir) )
		{
			/*
			 * NB: this throws, may need to fix that later.
			 * We don't care about the result though, either it made it or it
			 * didn't.  Any process that depends on the directory will check for it.
			 */
			boost::filesystem::create_directory(home_dot_dir);
		}

	}

	/*
	 * If WFHOME is setup, try to load config,
	 * else try to load from ~/.metaserver-ng.conf
	 * last resort is just to have a "$CWD/metaserver-ng.conf"
	 */
	if ( wfenv != NULL )
	{
		std::cout << "WFHOME: " << wfenv << std::endl;
		/*
		 * PREFIX/etc/metaserver-ng.conf
		 */
		config_file_path /= wfenv;
		config_file_path /= "/etc/metaserver-ng.conf";
		std::cout << "Searching configuration : " << config_file_path.string();

		if( boost::filesystem::is_regular_file(config_file_path))
		{
			std::cout << "Accepted.";
			config_found = true;
		}
		std::cout << std::endl;

	}

	/*
	 * HOME/.metaserver-ng.conf
	 *
	 * Try for fallback
	 */
	if ( !config_found && home != NULL )
	{
		std::cout << "HOME : " << home << std::endl;
		config_file_path = home;
		config_file_path /= "/.metaserver-ng.conf";
		std::cout << "Searching configuration : " << config_file_path.string() << " ... ";

		if( boost::filesystem::is_regular_file(config_file_path))
		{
			std::cout << "Accepted.";
			config_found = true;
		}
		std::cout << std::endl;
	}

	/*
	 * Last resort
	 */
	if( !config_found )
	{
		config_file_path = "metaserver-ng.conf";
		std::cout << "Default configuration : " <<  config_file_path.string() << std::endl;
		if ( boost::filesystem::is_regular_file(config_file_path) )
		{
			std::cout << "Using CWD Path: " << config_file_path.string() << std::endl;
		}
		else
		{
			std::cout << "No configuration file found!  All critical options are required to be passed via the command-line";
		}
	}

	/**
	 * Create our metaserver
	 */
	std::cout << "Create MetaServer instance" << std::endl;
	std::cout << "Metaserver Config File: " << config_file_path.string() << std::endl;
	MetaServer ms(io_service);

	try
	{

		std::ifstream config_file(config_file_path.string());
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
		std::cout << "Attempting to register MetaServer configuration ... " << std::endl;
		ms.registerConfig(vm);
		std::cout << "Logging to logfile: " << ms.getLogFile() << std::endl;
		VLOG(5) << "Configuration Registered";

		/**
		 * Go daemon if needed
		 */
		if ( ms.isDaemon() )
		{
			std::cout << "Running as daemon." << std::endl;
			LOG(INFO) << "Running as a daemon";
			daemon(0,0);
		}

		/**
		 * Define Handlers
		 */
		//MetaServerHandlerTCP tcp(ms, io_service, ip, port);
		VLOG(5) << "Start UPD Handler";
		MetaServerHandlerUDP udp(ms, io_service, ip, port);

		/**
		 * This is the async loop
		 */
		for(;;)
		{
			LOG(INFO) << "Enter ASYNC loop";
			try
			{
				/*
				 * We run and complete normally
				 */
				io_service.run();
				break;
			}
			catch(std::exception ex)
			{
				/*
				 * This will catch exceptions inside the io_service loop/handler.
				 *
				 * If we have a handler exception this should account as a reasonable
				 * effort to resume operation despite the error
				 */
				std::cerr << "IOService Loop Exception:" << ex.what() << std::endl;
				LOG(ERROR) << "IOService Loop Exception:" << ex.what();
			}
		}

	}
	catch (std::exception& e)
	{
		/*
		 * This will catch exceptions during the startup etc
		 */
		std::cerr << "Exception: " << e.what() << std::endl;
		google::FlushLogFiles(google::INFO);
	}
	LOG(INFO) << "All Done!";
	google::FlushLogFiles(google::INFO);
	return 0;
}
