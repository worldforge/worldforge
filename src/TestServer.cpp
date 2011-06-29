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
typedef std::vector <std::string> attribute_list;

int main(int argc, char** argv)
{

	/**
	 * Argument Wrangling
	 *
	 */
	boost::program_options::options_description desc( "TestServer" );
	boost::program_options::variables_map vm;
	boost::asio::io_service io_service;


	/**
	 * Note: options inside the configuration file that are NOT listed here
	 *       become ignored and are not accessible.
	 */
	desc.add_options()
		( "help,h", "Display help message" )
		( "server", boost::program_options::value<std::string>()->default_value("localhost"), "MetaServer host. \nDefault:localhost" )
		( "port", boost::program_options::value<int>()->default_value(8453), "MetaServer port. \nDefault:8453" )
		( "attribute", boost::program_options::value<attribute_list>(), "Set server attribute.\nDefault: none" )
		( "keepalive-interval", boost::program_options::value<int>()->default_value(5), "Interval between keepalives\nDefault: 5s" )
		( "keepalives", boost::program_options::value<int>()->default_value(5), "Number of keeplives before exit.\nDefault: 5" )
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
		std::cout << "KeepAlivesInt: " << vm["keepalive-interval"].as<int>() << std::endl;
		std::cout << "KeepAlives   : " << vm["keepalives"].as<int>() << std::endl;

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
				std::cout << "Attribute List Added" << std::endl;
			}
		}

		std::cout << "-------------------------" << std::endl;

		/**
		 * because boost query is too stupid to take port as an int
		 */
		std::stringstream port_str;
		port_str << vm["port"].as<int>();

		boost::asio::ip::udp::socket s(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
		boost::asio::ip::udp::resolver resolver(io_service);
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), vm["server"].as<std::string>(), port_str.str() );
		boost::asio::ip::udp::resolver::iterator iterator = resolver.resolve(query);

		/**
		 *  Step 1 : keepalive + register
		 */

		/**
		 *    1.1 - send keepalive
		 */
		MetaServerPacket keep;
		keep.setPacketType(NMT_SERVERKEEPALIVE);
		s.send_to(boost::asio::buffer(keep.getBuffer(), keep.getSize()), *iterator );

		/**
		 *    1.2 - receive handshake
		 */
		boost::array<char, MAX_PACKET_BYTES> recvBuffer;
		boost::asio::ip::udp::endpoint sender_endpoint;
		size_t bytes_recvd;

		bytes_recvd = s.receive_from( boost::asio::buffer(recvBuffer), sender_endpoint );

		MetaServerPacket shake( recvBuffer, bytes_recvd );
		shake.setAddress(sender_endpoint.address());
		shake.setPort(sender_endpoint.port());

		unsigned int shake_key = shake.getIntData(4);

		/**
		 *    1.3 - send servershake
		 */
		MetaServerPacket servershake;
		servershake.setPacketType(NMT_SERVERSHAKE);
		servershake.addPacketData(shake_key);
		servershake.setAddress( shake.getAddress() );
		s.send_to(boost::asio::buffer(servershake.getBuffer(), servershake.getSize()), *iterator );

		/**
		 *  Step 2 : register attributes if any
		 */
		if ( vm.count("attribute") )
		{
			attribute_list v = vm["attribute"].as<attribute_list>();
			while(!v.empty())
			{
				std::string ele = v.back();
				size_t pos = ele.find_first_of("=");
				if( pos != std::string::npos )
				{
					std::string n = ele.substr(0,pos);
					std::string v = ele.substr(pos+1);
					std::cout << " register: " << n << std::endl;
					std::cout << "    value: " << v << std::endl;
					MetaServerPacket a;
					a.setPacketType(NMT_SERVERATTR);
					a.addPacketData(n.length());
					a.addPacketData(v.length());
					a.addPacketData(n);
					a.addPacketData(v);
					s.send_to(boost::asio::buffer(a.getBuffer(), a.getSize()), *iterator );
				}
				else
				{
					std::cout << " Attribute Ignored : " << ele << std::endl;
				}
				v.pop_back();
			}
		}

		/**
		 *  Step 3 : keepalive loop until we're done
		 */
		for(int i = 0; i < vm["keepalives"].as<int>() ; ++i)
		{

			// 1.1
			s.send_to(boost::asio::buffer(keep.getBuffer(), keep.getSize()), *iterator );

			// 1.2
			bytes_recvd = s.receive_from( boost::asio::buffer(recvBuffer), sender_endpoint );
			MetaServerPacket in(recvBuffer, bytes_recvd );
			unsigned int pkey = in.getIntData(4);

			// 1.3
			MetaServerPacket out;
			servershake.setPacketType(NMT_SERVERSHAKE);
			servershake.addPacketData(pkey);
			servershake.setAddress( in.getAddress() );
			s.send_to(boost::asio::buffer(out.getBuffer(), out.getSize()), *iterator );

			std::cout << "Sleeping between keepalives : " << vm["keepalive-interval"].as<int>() << std::endl;
			sleep( vm["keepalive-interval"].as<int>() );

		}

		/**
		 *  Step 4: send terminate
		 */
		sleep(5);
		MetaServerPacket term;
		term.setPacketType(NMT_TERMINATE);
		term.setAddress( shake.getAddress());
		s.send_to(boost::asio::buffer(term.getBuffer(), term.getSize()), *iterator );


	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	std::cout << "All Done!" << std::endl;
	return 0;
}
