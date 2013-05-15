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

 http://doc.powerdns.com/backend-writers-guide.html
 http://doc.powerdns.com/pipebackend-dynamic-resolution.html
 http://doc.powerdns.com/backends-detail.html#pipebackend
 http://doc.powerdns.com/backends-detail.html#pipebackend-protocol

 */

#include "MetaServer.hpp"
#include <boost/asio/ip/udp.hpp>
#include <boost/algorithm/string.hpp>
#include <glog/logging.h>

int main(int argc, char** argv)
{

	/**
	 * Argument Wrangling
	 *
	 */
	boost::program_options::options_description desc( "MetaServer PDNS Pipe" );
	boost::program_options::variables_map vm;
	boost::asio::io_service io_service;
	boost::array<char, MAX_PACKET_BYTES> recvBuffer;
	boost::asio::ip::udp::endpoint sender_endpoint;
	size_t bytes_recvd;
	std::string domain,banner;

	/**
	 * Note: options inside the configuration file that are NOT listed here
	 *       become ignored and are not accessible.
	 */
	desc.add_options()
		( "help,h", "Display help message." )
		( "server", boost::program_options::value<std::string>()->default_value("localhost"), "MetaServer host. \nDefault:localhost" )
		( "port", boost::program_options::value<int>()->default_value(8453), "MetaServer port. \nDefault:8453" )
		( "domain", boost::program_options::value<std::string>()->default_value("ms.worldforge.org"), "Domain for processing queries. \nDefault:ms.worldforge.org" )
		( "banner", boost::program_options::value<std::string>()->default_value("WorldForge Metaserver PDNS Pipe"), "Domain for processing queries. \nDefault:WorldForge Metaserver PDNS Pipe" )
		( "debug", boost::program_options::value<std::string>()->implicit_value("true"), "Create Logfile for debug in /tmp/")
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

		/*
		 * Debug
		 */
		google::InitGoogleLogging("");
		//google::SetLogDestination(google::INFO,"/tmp/pdnspipe.log");

		/**
		 * domain and banner
		 */
		domain = vm["domain"].as<std::string>();
		banner = vm["banner"].as<std::string>();

		/**
		 * because boost query is too stupid to take port as an int
		 */
		std::stringstream port_str;
		port_str << vm["port"].as<int>();

		boost::asio::ip::udp::socket s(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
		boost::asio::ip::udp::resolver resolver(io_service);
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), vm["server"].as<std::string>(), port_str.str() );
		boost::asio::ip::udp::resolver::iterator iterator = resolver.resolve(query);

		/*
		 * Turn off stdin/stdout buffering
		 */

		/**
		 *  Step 1 : Initialise
		 *  	i) read from STDIN
		 *  	ii) If result is "HELO\t1" print "OK\tBanner"
		 *  	iii) else print "FAIL\n"
		 *  	iv) error to stderr
		 *  	v) exit
		 */
		 std::string helo;
		 int proto;
		 std::cin >> helo >> proto;

		 std::cerr << "Received: " << helo << "\t" << proto << std::endl;
		 LOG(INFO) << "Received Helo: " << helo << "\t" << proto;

		 if ( !(boost::equals(helo,"HELO")) || proto != 1 )
		 {
			 std::cerr << "ERROR: Failure to initialise!" << std::endl;
			 std::cout << "FAIL" << std::endl;
			 exit(1);
		 }

		 /*
		  *  Banner
		  */
		 std::cout << "OK " << banner << std::endl;


		/**
		 *  Step 2 : Main Pump
		 *  while(1)
		 *      read in line
		 *      parse line into fields
		 *      0 = query
		 *      1 = qname
		 *      2 = qclass
		 *      3 = qtype
		 *      4 = id
		 *      5 = remote ip
		 */
		 std::string fields[6] = "";
		 std::string line;

		 while(1)
		 {
			 std::string token;
			 std::stringstream iss;
			 int field_cnt = 0;

			 // read in whole line
			 std::getline(std::cin,line);

			 if ( line.empty() )
				 continue;

			 std::cerr << "Received: " << line << std::endl;
			 LOG(INFO) << "Received Line: " << line;

			 iss << line;

			 while( std::getline(iss,token,'\t') )
			 {
				 //std::cerr << "Read : " << field_cnt << " : " << token << std::endl;
				 fields[field_cnt] = token;
				 ++field_cnt;
			 }

			 /*
			  * PING is a special case
			  */
			 if ( boost::iequals(fields[0],"PING") )
			 {
				 std::cout << "OK" << std::endl;
				 continue;
			 }

			 /*
			  * EXIT being another special case
			  */
			 if ( boost::iequals(fields[0],"EXIT") )
			 {
				 std::cout << "OK" << std::endl;
				 break;
			 }

			 /*
			  * AXFR not supported
			  *
			  * Instead we will fake it ala:
			  *
			  *
			  */
			 if ( boost::iequals(fields[0],"AXFR"))
			 {
//				 std::cout << "LOG AXFR requests not supported" << std::endl;
//				 std::cout << "FAIL" << std::endl;
				 std::cout << "DATA\tdnsfail.worldforge.org\tIN\tSOA\t3600\t1\tdnsfail.worldforge.org\t1.2.3.4" << std::endl;
				 std::cout << "DATA\tdnsfail.worldforge.org\tIN\tNS\t3600\t1\tnsfail.worldforge.org" << std::endl;
				 std::cout << "DATA\tnsfail.worldforge.org\tIN\tA\t3600\t1\t127.0.0.1" << std::endl;
				 std::cout << "END" << std::endl;
				 continue;
			 }

			 /*
			  * Check if request looks complete
			  */
			 if ( field_cnt < 6 )
			 {
				 std::cout << "LOG	Unparsable line: " << field_cnt << ":" << fields[0]
						   << ":" << fields[1] << ":" << fields[2] << ":" << fields[3]
						   << ":" << fields[4] << ":" << fields[5] << std::endl;
				 std::cout << "FAIL" << std::endl;
				 std::cerr << "field_cnt : " << field_cnt << std::endl;
				 continue;
			 }


			 /**
			  * Grab the name
			  */
			 std::string name = fields[1];

			 //TODO: maybe add something where we filter the request by domain
			 //boost::ireplace_last(name,"."+domain,"");

			 MetaServerPacket req;

			 req.addPacketData(NMT_DNSREQ);

			 /**
			  * Like DNS ... A=forward PTR=reverse
			  */
			 if ( boost::iequals(fields[3],"A") )
			 {
				 req.addPacketData(DNS_TYPE_A);
			 }
			 else if ( boost::iequals(fields[3],"PTR" ) )
			 {
				 req.addPacketData(DNS_TYPE_PTR);
			 }
			 else if ( boost::iequals(fields[3],"SOA") || boost::iequals(fields[3],"ANY"))
			 {
				 /*
				  * I don't like this
				  */
				 req.addPacketData(DNS_TYPE_ALL);
			 }
			 else
			 {
				 std::cout << "LOG	Query type must be IN or A, not [" << fields[3] << "]" << std::endl;
				 std::cout << "LOG " << fields[0] << ":" << fields[1] << ":" << fields[2] << ":"
						   << fields[3] << ":" << fields[4] << ":" << fields[5] << std::endl;
				 std::cout << "FAIL" << std::endl;
				 continue;
			 }

			 /**
			  * Pack in message
			  */
			 req.addPacketData(name.length());
			 req.addPacketData(name);

			 /*
			  * Ship it off
			  */
			 s.send_to(boost::asio::buffer(req.getBuffer(), req.getSize()), *iterator );

			 /**
			  * Receive response or timeout
			  *
			  * if response, then analyse response
			  * 	if results
			  * 		send results
			  * 	else
			  * 		fail
			  * else
			  * 	fail
			  */
			 boost::asio::ip::udp::endpoint sender_endpoint;
			 bytes_recvd = s.receive_from( boost::asio::buffer(recvBuffer), sender_endpoint );


			 MetaServerPacket resp( recvBuffer, bytes_recvd );
			 if( resp.getPacketType() == NMT_DNSRESP )
			 {
				 uint32_t total_servers = resp.getIntData(4);
				 //uint32_t packed_servers = resp.getIntData(8);
				 std::vector<uint32_t> len_arr;

//				 std::cout << "LOG name=" << name << "::total_servers=" << total_servers
//						   << "::packed_servers=" << packed_servers
//						   << "field1=" << fields[1] << std::endl;
//
				 if( !(total_servers>0) )
				 {
					 /*
					  * If we're zero or less than that means we have bubkis
					  */
//					 std::cout << "LOG no match found for [" << name << "]" << std::endl;
					 std::cout << "FAIL" << std::endl;
					 continue;
				 }



				 /*
				  * We have one or more responses
				  */
				 std::cerr << "Servers Matched: " << total_servers << std::endl;
				 for ( unsigned int i=1; i<=total_servers; ++i )
				 {
					 /*
					  * We know that there are total_servers, and that as a uint that
					  * is 4 bytes ( or 32 bits =).  The initial offset thus will be 8 bytes
					  * byte 0 - packet type
					  * byte 4 - total_servers
					  * byte 8 - packed servers ( not in use yet )
					  * byte +4 - one 4byte block per
					  */
					 len_arr.push_back( resp.getIntData(((i*4)+8)));

				 }
				 /*
				  * e.g 2 item response
				  * 0 - packet type
				  * 4 - total servers
				  * 8 - packed
				  * 12 - server 1 length
				  * 16 - server 2 length
				  * 20+ - message
				  */
				 std::string resp_msg = resp.getPacketMessage( ((total_servers*4)+8+4) );

				 /*
				  * Starting at 0 index, previous loop needed to start at 1
				  *      0 = query
				  *      1 = qname
				  *      2 = qclass
				  *      3 = qtype
				  *      4 = id
				  *      5 = remote ip
				  */
				 int offset = 0;
				 for ( unsigned int i=0; i < total_servers; ++i )
				 {
					 std::cout << "DATA\t" <<
							      name << "\t" <<
							      fields[2] << "\t" <<
							      fields[3] << "\t" <<
							      "3600" << "\t" <<
							      "1" << "\t" <<
							      resp_msg.substr(offset,len_arr[i]) << std::endl;
					 offset+=len_arr[i];
				 }
				 std::cout << "END" << std::endl;

			 }
			 else
			 {
				 /*
				  * It's not expected .. carry on
				  */
				 std::cout << "LOG	Packet response is malformed, ignoring" << std::endl;
				 std::cout << "FAIL" << std::endl;
				 continue;
			 }



//			 std::string resp[6];

			 // fake record
//			 resp[0] = name;
//			 resp[1] = fields[2];
//			 resp[2] = fields[3];
//			 resp[3] = "3600";
//			 resp[4] = "1";
//			 resp[5] = "1.2.3.4";

			 /**
			  * Send DNS Response
			  */
//			 std::cout << "DATA\t"
//					   << resp[0] << "\t"
//					   << resp[1] << "\t"
//			           << resp[2] << "\t"
//			           << resp[3] << "\t"
//			           << resp[4] << "\t"
//			           << resp[5] << "\t"
//			           << std::endl;

		 }

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	std::cerr << "All Done" << std::endl;
}
