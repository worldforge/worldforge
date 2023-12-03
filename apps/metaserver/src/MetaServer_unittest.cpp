/**
 Worldforge Next Generation MetaServer

 Copyright (C) 2011 Alistair Riddoch <alriddoch@googlemail.com>
 	 	 	 	 	Sean Ryan <sryan@evercrack.com>

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

/*
 * System Includes
 */
#include <cppunit/TestCase.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <cassert>
#include <limits>


class MetaServer_unittest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(MetaServer_unittest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testInitTimers);
    CPPUNIT_TEST(testMonitorStats);
//    CPPUNIT_TEST(testAdminAddServer);
//    CPPUNIT_TEST(testAdminDelServer);
    CPPUNIT_TEST(testProcessAdminReq_ENUMERATE);
    CPPUNIT_TEST(testProcessAdminReq_ADDSERVER);
//    CPPUNIT_TEST(testProcessAdminReq_DELSERVER);
    CPPUNIT_TEST_SUITE_END();
  public:

    MetaServer_unittest() { }

    void testConstructor() {
        MetaServer * ms = new MetaServer();
        CPPUNIT_ASSERT(ms);
        delete ms;
    }

    void testInitTimers()
    {
    	boost::asio::io_service ios;
    	MetaServer* ms = new MetaServer();

    	ms->initTimers(ios);

    	/*
    	 * To catch any untoward happenings on init
    	 */
    	CPPUNIT_ASSERT(ms);

    	/*
    	 * Re-initialize timer (to cover failure and reinit)
    	 */
    	ms->initTimers(ios);
    	CPPUNIT_ASSERT(ms);

    	delete ms;
    }

    void testMonitorStats() {
    	MetaServer* ms = new MetaServer();
    	std::map<std::string,std::string> m;
    	m.clear();

    	// sanity check
    	CPPUNIT_ASSERT(m.size() == 0 );

    	// check that empty stats comes back with something
    	ms->getMSStats(m);
    	std::cout << std::endl << "Stats-Expect-non-zero: " << m.size() << std::endl;
    	CPPUNIT_ASSERT(m.size() != 0 );
    	for(auto& x: m)
    	{
    		std::cout << std::endl << "Stats-Expect full: " << x.first << " : " << x.second << std::endl;
    	}

    	// set a single entry, and expect only a single back
    	m.clear();
    	m["packet.sequence"] = "NIL";
    	ms->getMSStats(m);
    	std::cout << std::endl << "Stats-Expect-1:" << m.size() << std::endl;
    	for(auto& x: m)
    	{
    		std::cout << std::endl << "Stats-Expect : " << x.first << " : " << x.second << std::endl;
    	}
    	CPPUNIT_ASSERT(m.size() == 1 );
    	CPPUNIT_ASSERT(m["packet.sequence"] != "NIL" );

    	// set 2 entries, 1 is invalid (a stat doesn't exist).
    	// we expect to get back 1 entry
    	m.clear();
    	CPPUNIT_ASSERT(m.size() == 0 );
    	m["packet.sequence"] = "foo";
    	m["does.not.exist"] = "bar";
    	ms->getMSStats(m);
    	std::cout << std::endl << "Stats-Expect-2:" << m.size() << std::endl;
    	for(auto& x: m)
    	{
    		std::cout << std::endl << "Stats-Expect2 : " << x.first << " : " << x.second << std::endl;
    	}

    	// we should get 1
    	CPPUNIT_ASSERT(m.size() == 1 );
    	CPPUNIT_ASSERT(m["packet.sequence"] != "foo" );

    	delete ms;
    }
//    void testAdminAddServer() {
//    	CPPUNIT_FAIL("to be implemented");
//    }
//    void testAdminDelServer() {
//    	CPPUNIT_FAIL("to be implemented");
//    }

    void testProcessAdminReq_ENUMERATE() {

    	MetaServerPacket in,out;
    	const int ENUMERATE_COMMANDS = 2; // set to agree with MS command set hash
    	std::string msg = "";

    	/*
    	 * Create enum packet
    	 */
    	in.setPacketType(NMT_ADMINREQ);
    	in.addPacketData(NMT_ADMINREQ_ENUMERATE);

    	/*
    	 * Create instance
    	 */
    	MetaServer* ms = new MetaServer();

    	/*
    	 * Process with metaserver
    	 */
    	ms->processADMINREQ(in,out);

    	/*
    	 * Ensure it's an admin response
    	 */
    	CPPUNIT_ASSERT( out.getPacketType() == NMT_ADMINRESP );

    	/*
    	 * Ensure response is the correct type
    	 */
    	CPPUNIT_ASSERT( out.getIntData(4) == NMT_ADMINRESP_ENUMERATE );

    	/*
    	 * Check for response properties
    	 */
    	std::ostringstream oss;
    	int packet_commands = out.getIntData(8);

    	/*
    	 * Make sure the number of commands in testcase match with metaserver
    	 */
    	oss << packet_commands;
    	msg.append("m_adminCommandSet[");
    	msg.append( oss.str() );
    	msg.append("] mismatched with expected commands ");
    	oss.clear();
    	oss.str("");
    	oss << ENUMERATE_COMMANDS;
    	msg.append(oss.str());
    	CPPUNIT_ASSERT_MESSAGE( msg.c_str(), packet_commands == ENUMERATE_COMMANDS );

    	// debug output in case it's wrong
    	std::cout << std::endl << "   TYPE: " << out.getPacketType() << std::endl;
    	std::cout << "SUBTYPE: " << out.getIntData(4) << std::endl;
    	std::cout << "COMMANDS: " << out.getIntData(8) << std::endl;

    	/*
    	 * Ensure that the packed lengths for the given commands
    	 * match in size
    	 */
    	std::string inmsg;
    	std::list<int> index;
    	int index_total = 0;
    	for( int i = 1; i<=packet_commands; i++)
    	{
    		std::cout << "I: " << i << std::endl;
    		std::cout << "OFFSET: " << (8+(i*4)) << std::endl;
    		std::cout << "CMD " << i << " length is " << out.getIntData(8+(i*4)) << std::endl;
    		index.push_back(out.getIntData( 8+(i*4)) );
    		index_total += out.getIntData( 8+(i*4) );
    		std::cout << "INDEX TOTAL: " << index_total << std::endl;
    	}

    	// more debug info
    	std::cout << "8 + packet_commands * index.size() " << std::endl;
    	std::cout << "8 + " << packet_commands << " * " << index.size() << std::endl;
    	inmsg = out.getPacketMessage( 12+(4*index.size()) );
    	std::cout << "INMSG: " << inmsg  << std::endl;

    	// make sure that sizes match
    	CPPUNIT_ASSERT( inmsg.length() == index_total );

    	// check each item to make sure not minorly mangled
        int offset = 0;
    	for(auto &i : index)
    	{
    		std::string j = inmsg.substr(offset,i);
    		offset+=i;
    		std::cout << "i: " << i << std::endl;
    		std::cout << "j: " << j << std::endl;

    		CPPUNIT_ASSERT( j.length() == i );
    	}

    	delete ms;

    }

    void testProcessAdminReq_ADDSERVER() {
//		Need to think about maybe a refactor to calculate offsets based on sizeof
//		but need to be careful because if comminucating with systems that have
//		different values for unit32_t (normally 4 bytes).  Perhaps determine sizings
//		on metaserver, and communicate with clients

//    	std::cout << std::endl << "MIN : " << std::numeric_limits<uint32_t>::min() << std::endl;
//    	std::cout << "MAX : " << std::numeric_limits<uint32_t>::max() << std::endl;
//    	std::cout << "SIZE: " << sizeof(uint32_t) << std::endl;
//    	std::cout << "LONGMAX : " << std::numeric_limits<unsigned long long>::max() << std::endl;
//    	std::cout << "SIZE : " << sizeof(unsigned long) << std::endl;

        MetaServerPacket in,out;

        std::string msg = "";

        /*
         * Manufacture the circumstances needed.
         * We need to add the ip + port of the server to be added AS DATA
         * The MSP.getAddress and MSP.getPort are what the comms of where
         * the packet would have originated, which is NOT what server would
         * need to be added.
         */
        in.setPacketType(NMT_ADMINREQ);
        in.addPacketData(NMT_ADMINREQ_ADDSERVER);
        in.addPacketData( in.IpAsciiToNet("127.0.2.1"));
        in.addPacketData( 12345 );

        in.setAddress("123.123.123.123");
        in.setPort(11111);

        MetaServer* ms = new MetaServer();

        ms->processADMINREQ(in,out);

        uint32_t out_ip = out.getIntData(8);
        uint32_t out_port = out.getIntData(12);

        CPPUNIT_ASSERT( out.getPacketType() == NMT_ADMINRESP );
        CPPUNIT_ASSERT( out.getIntData(0) == NMT_ADMINRESP );
        CPPUNIT_ASSERT( out.getIntData(4) == NMT_ADMINRESP_ADDSERVER );

        /*
         * @see MetaServerPacket::IpAsciiToNet
         * MS looks at '127.0.2.1' as 16908415 because it is packed
         * as '1.2.0.127'.  The correct way of doing things is to have
         * it value as 2130706945.
         */

        /*
         * Ensure IP isn't set, it gets set in the transport part of
         * response in the handler.
         */
        CPPUNIT_ASSERT( out.getAddress() != "123.123.123.123" );
        CPPUNIT_ASSERT( out.getAddress() == "127.0.2.1" );

        /*
         * Ensure IP and port from data resp packet is the same on the way in
         */
        CPPUNIT_ASSERT( out_ip == 16908415 ); // packed addy 127.0.2.1
        CPPUNIT_ASSERT( out_port == 12345 );  // packed port

    }

//    void testProcessAdminReq_DELSERVER() {
//
//    	CPPUNIT_FAIL("to be implemented");
//
//    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MetaServer_unittest);


int main()
{
    CppUnit::TextTestRunner runner;
    CppUnit::Test* tp =
            CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    runner.addTest(tp);

    if (runner.run()) {
        return 0;
    } else {
        return 1;
    }
}
