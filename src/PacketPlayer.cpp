/**
 Worldforge Next Generation MetaServer

 Copyright (C) 2012 Sean Ryan <sryan@evercrack.com>

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
#include "MetaServer.hpp"
#include "PacketReader.hpp"

/*
 * System Includes
 */
#include <boost/asio/ip/udp.hpp>
#include <boost/algorithm/string.hpp>
#include <iomanip>

/* defs */
typedef std::vector <std::string> attribute_list;

int main(int argc, char** argv)
{

	/**
	 * Argument Wrangling
	 *
	 */
	boost::program_options::options_description desc( "PacketPlayer" );
	boost::program_options::variables_map vm;
	boost::asio::io_service io_service;
	boost::array<char, MAX_PACKET_BYTES> recvBuffer;
	boost::asio::ip::udp::endpoint sender_endpoint;
	size_t bytes_recvd;
	PacketReader* p;


	/**
	 * Note: options inside the configuration file that are NOT listed here
	 *       become ignored and are not accessible.
	 */
	desc.add_options()
		( "help,h", "Display help message" )
		( "server", boost::program_options::value<std::string>()->default_value("localhost"), "MetaServer host. \nDefault:localhost" )
		( "port", boost::program_options::value<int>()->default_value(8453), "MetaServer port. \nDefault:8453" )
		( "playonly", boost::program_options::value<std::string>(), "Play output info only.\nDefault: false" )
		( "file", boost::program_options::value<std::string>(), "Binary Packet logfile.\nDefault: none" )
			;

	try
	{

		boost::program_options::store(
				boost::program_options::parse_command_line(argc, argv, desc),
				vm
				);
		boost::program_options::notify(vm);

		/**
		 * Special case for help
		 */
		if ( vm.count("help") )
		{
			std::cout << desc << std::endl;
			return 0;
		}

		std::cout << "Server       : " << vm["server"].as<std::string>() << std::endl;
		std::cout << "Port         : " << vm["port"].as<int>() << std::endl;
		std::cout << "---------------" << std::endl;

		for (boost::program_options::variables_map::iterator it=vm.begin(); it!=vm.end(); ++it )
		{
			if ( it->second.value().type() == typeid(int) )
			{
				std::cout << it->first.c_str() << "=" << it->second.as<int>() << std::endl;
			}
			else if (it->second.value().type() == typeid(std::string) )
			{
				std::cout << it->first.c_str() << "=" << it->second.as<std::string>().c_str() << std::endl;
			}
			else if (it->second.value().type() == typeid(attribute_list) )
			{
				std::cout << it->first.c_str() << "=Attribute List" << std::endl;
			}
		}

		std::cout << "-------------------------" << std::endl;

		/**
		 * because boost query is too stupid to take port as an int
		 */
		std::stringstream port_str;
		port_str << vm["port"].as<int>();

		/**
		 * Wrangle the file etc options
		 */
		std::string file = "";
		bool playOnly = false;

		if( vm.count("playonly") )
		{
			std::string s = vm["playonly"].as<std::string>();
			if ( boost::iequals(s,"true") )
			{
				playOnly = true;
			}
		}

		if( vm.count("file") )
		{
			file = vm["file"].as<std::string>();
		}

		if ( file.empty() )
		{
			std::cout << "--file option not specified, but is required" << std::endl;
			return 13;
		}


		/**
		 *	Initialize reader
		 */
		p = new PacketReader();
		int pkts = p->parseBinaryFile(file);

		if( playOnly )
		{
			while(p->hasPacket())
			{
				MetaServerPacket msp = p->pop();
				unsigned int i = msp.getPacketType();
				std::string s = msp.getOutBound() ? ">>>>" : "<<<<";
				std::cout << "Packet [";
				std::cout << std::setfill('0') << std::setw(10) << msp.getSequence();
				std::cout << "] " << s << " : " << NMT_PRETTY[i] << std::endl;
			}
		}
		else
		{
			std::cout << "TODO: play packets to listed metaserver" << std::endl;
		}

		std::cout << "Packet File : " << file << std::endl;
		std::cout << "Packet Count: " << pkts << std::endl;

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	std::cout << "All Done!" << std::endl;
	return 0;
}
