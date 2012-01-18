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
 * NOTE: I'm sure there should be some way to test this more completely, however
 *       all this class is doing is serving as a tcp touchdown for the io_service.
 *
 *       The io_service handles the listen etc, and I don't see how to easily
 *       perform both a server/client operation
 */

/*
 * Local Includes
 */
#include "MetaServerHandlerTCP.hpp"
#include "MetaServer.hpp" // handler is tightly coupled with main class

/*
 * System Includes
 */
#include <cppunit/TestCase.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <log4cpp/Category.hh>

#include <cassert>
#include <string>



class MetaServerHandlerTCP_unittest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(MetaServerHandlerTCP_unittest);
    CPPUNIT_TEST(test_Constructor);
    CPPUNIT_TEST(test_sendPacket);
    CPPUNIT_TEST_SUITE_END();
  public:
    MetaServerHandlerTCP_unittest() { }

    boost::asio::io_service io;
    std::string host ;
    MetaServer* ms;
    MetaServerHandlerTCP* ms_tcp;

    void setUp()
    {
    	host = "127.0.0.1";
    	ms = new MetaServer(io);
    	ms_tcp = new MetaServerHandlerTCP(*ms, io, host, 50000 );
    }
    void tearDown()
    {
    	delete ms_tcp;
    	delete ms;
    }

    void test_Constructor()
    {
    	CPPUNIT_ASSERT(ms_tcp);
    }

    void test_sendPacket()
    {

    	/*
    	 *  NOTE: I can't get this to work.
    	 *  What I hope for is:
    	 *  	1. Put MetaServerHandlerTCP into listen mode, but not via io.run() ... which is blocking
    	 *  	2. Open a real tcp connection and fire off a real packet.  It was my hope to use io.post to do this
    	 *  	   so as not to block.
    	 *  	3. Using io.poll, catch the said packet
    	 *
    	 *  	As this is unit testing for the handler, we don't care what happens in the MS, only that
    	 *  	the packet reached the MS whole, or alternatively that the MSP created as a result
    	 *  	of this test request is as expected.
    	 */
//    	 CPPUNIT_FAIL("NOT IMPLEMENTED");
//        boost::asio::ip::tcp::tcp::resolver resolver(io);
//        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v6(), host, "50000" );
//        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
//
//        boost::asio::ip::tcp::socket s(io);
////        s.connect(s, iterator);
//
//
//        /* fixed size message */
//        char req[10] = "123456789";
//        io.post(func());
//
////        io.post( s.write(s, boost::asio::buffer(req, 10)) );
//
//        std::size_t p = io.poll();
//
//        std::cout << std::endl << "PACKET HANDLER: " << p << std::endl;

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(MetaServerHandlerTCP_unittest);


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

/*
 * Mock Stubs
 */
MetaServer::MetaServer(boost::asio::io_service& ios)
	: m_expiryDelayMilliseconds(1500),
	  m_updateDelayMilliseconds(4000),
	  m_handshakeExpirySeconds(30),
	  m_sessionExpirySeconds(3600),
	  m_clientExpirySeconds(300),
	  m_packetLoggingFlushSeconds(10),
	  m_keepServerStats(false),
	  m_keepClientStats(false),
	  m_maxServerSessions(1024),
	  m_maxClientSessions(4096),
	  m_isDaemon(false),
	  m_Logfile(""),
	  m_Logger( log4cpp::Category::getInstance("MetaServerHandlerTCP_unittest") ),
	  m_logServerSessions(false),
	  m_logClientSessions(false),
	  m_logPackets(false),
	  m_PacketLogfile(""),
	  m_PacketSequence(0),
	  m_startTime(boost::posix_time::microsec_clock::local_time())
{

}

MetaServer::~MetaServer()
{

}

log4cpp::Category& MetaServer::getLogger()
{
	return m_Logger;
}

DataObject::DataObject()
{

}

DataObject::~DataObject()
{

}

MetaServerHandler::MetaServerHandler()
{

}

MetaServerHandler::~MetaServerHandler()
{

}
