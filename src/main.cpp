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
#include <unistd.h> /* daemon(), getpid() */
#include <signal.h>
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
		( "server.logfile", boost::program_options::value<std::string>(), "Server logfile location.\nDefault: logdir/metaserver-ng.log" )
		( "server.pidfile", boost::program_options::value<std::string>(), "Server pidfile location.\nDefault: rundir/metaserver-ng.pid" )
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
	 * HOME/.metaserver-ng.conf
	 *
	 * Try for local configuration first
	 */
	if ( home != NULL )
	{
		std::cout << "HOME : " << home << std::endl;
		config_file_path = home;
		config_file_path /= "/.metaserver-ng.conf";
		std::cout << "Searching configuration : " << config_file_path.string() << " ... ";

		if( boost::filesystem::is_regular_file(config_file_path))
		{
			std::cout << " Accepted.";
			config_found = true;
		}
		std::cout << std::endl;
	}

	/*
	 * If WFHOME is setup, try to load distribution configuration
	 */
	if ( !config_found && wfenv != NULL )
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
			std::cout << " Accepted.";
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
	std::cout << "Server Version: " << SERVER_VERSION << std::endl;
	std::cout << "API    Version: " << API_VERSION << std::endl;
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
		 *	Create pid file
		 */
//		std::cout << "Checking server.pidfile ..." << std::endl;
		boost::filesystem::path pid_file_path;
		if ( vm.count("server.pidfile") )
		{

			/*
			 * Create rundir as required
			 */
			pid_file_path = vm["server.pidfile"].as<std::string>();
			LOG(INFO) << "Pidfile: " << pid_file_path.string();
			std::cout << "Pidfile: " << pid_file_path.string() << std::endl;

			if ( ! boost::filesystem::is_directory(pid_file_path.parent_path().string()))
			{
				LOG(INFO) << "Creating Run Directory : " << pid_file_path.parent_path().string();
				std::cout << "Creating Run Directory : " << pid_file_path.parent_path().string() << std::endl;
				bool pf = boost::filesystem::create_directory(pid_file_path.parent_path());
				if(!pf)
					throw std::runtime_error("Can not create run directory");
			}

			/*
			 * Drop in pid
			 */
			if ( boost::filesystem::is_regular_file(pid_file_path) )
			{
				std::cout << "Pidfile Exists: " << pid_file_path.string() << std::endl;
				LOG(INFO) << "Pidfile Exists: " << pid_file_path.string();

				/*
				 * PIDFILE is present, check is process is running
				 * if so, refuse to start, if not, clean up pidfile
				 */
				std::ifstream pif(pid_file_path.string());
				int pid = 0;
				pif >> pid;
				std::cout << "Checking pid : " << pid << std::endl;
				LOG(INFO) << "Checking pid : " << pid;

				if( kill(pid,0) == -1 )
				{
					std::cout << "Pidfile is stale, removing." << std::endl;
					LOG(INFO) << "Pidfile is stale, removing.";
					boost::filesystem::remove(pid_file_path);
				}
				else
				{
					LOG(INFO) << "Refusing to start!";
					throw std::runtime_error("Pidfile exists and process is running, refusing to start");
				}
			}

		}
		else
		{
			LOG(INFO) << "Pidfile Not Specified";
			std::cout << "Pidfile Not Specified " << std::endl;
		}

		/**
		 * Go daemon if needed
		 *
		 * NOTE: It is critical that the pidfile checking go BEFORE daemon
		 * because the potential to inform the user via std::cout disappears
		 * but is is critical that the pid file be filled AFTER the daemon,
		 * as the daemon is forking a child, and that will be the running
		 * process, not here.
		 */
		if ( ms.isDaemon() )
		{
			std::cout << "Running as daemon." << std::endl;
			std::cout << "NOTE: Be sure to check the logfile for any messages, as console output is now disabled." << std::endl;
			LOG(INFO) << "Running as a daemon";
			daemon(0,0);
		}


		/*
		 * Write the getpid
		 *
		 * NOTE: must occur AFTER daemon
		 */
		if ( ! boost::filesystem::is_regular_file(pid_file_path) )
		{
			std::ofstream pidfile;
			pidfile.open(pid_file_path.c_str());
			pidfile.clear();
			pidfile << getpid();
			pidfile.close();
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
		while(! ms.isShutdown() )
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

				/*
				 * TODO: just being lazy here, should probably subclass a shutdown
				 *       exception and catch it separately.  This will be totally
				 *       bogus in the event that a real exception occurs during
				 *       the shutdown process, but the risk is minimal as it's
				 *       going down anyway.
				 */
				if ( ms.isShutdown() )
				{
					std::cout << "Shutdown sequence initiated." << std::endl;
					LOG(INFO) << "Shutdown sequence initiated.";

				}
				else
				{
					std::cerr << "IOService Loop Exception:" << ex.what() << std::endl;
					LOG(ERROR) << "IOService Loop Exception:" << ex.what();
				}
			}
		}
		LOG(INFO) << "Shutting Down Metaserver";

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
