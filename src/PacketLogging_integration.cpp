/**
 Worldforge Next Generation MetaServer

 Copyright (C) 2012 Sean Ryan <sryan@evercrack.com>
 	 	 	 	 	Alistair Riddoch <alriddoch@googlemail.com>

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

 * NOTE: If you want to create a packet log for testing
 * ./metaserver-ng --server.daemon=false --logging.packet_logging=true --logging.packet_logfile=/tmp/footest.bin
 * then on separate terminal:
 * ./testserver
 *
 * Just CTRL-C the metaserver when done.  This produces the packet file for the
 * parseBinaryFile test.

 */

/*
 * Local Includes
 */
#include "PacketReader.hpp"
#include "PacketLogger.hpp"

/*
 * System Includes
 */
#include <cppunit/TestCase.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <cassert>

class PacketLogging_integration : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(PacketLogging_integration);
    CPPUNIT_TEST(test_initialSetup);
    CPPUNIT_TEST(test_PacketLogger_write);
    CPPUNIT_TEST(test_PacketReader_parseBinaryFile_empty);
    CPPUNIT_TEST(test_PacketReader_parseBinaryFile);
    CPPUNIT_TEST(test_PacketReader_parseBinaryFile_append);

    CPPUNIT_TEST_SUITE_END();
  public:
    PacketLogging_integration() { }

    PacketReader* p;
    PacketLogger* pl;
    MetaServerPacket* mp1;
    MetaServerPacket* mp2;
    std::string file_name;

    /*
     * I would like to thank this particular integration test
     * for showing me that the setUp is called for every test,
     * not once per testsuite as I had originally thought.
     */
    void setUp()
    {
    	mp1 = new MetaServerPacket();
    	mp1->setPacketType(NMT_SERVERKEEPALIVE);
    	mp1->setSequence(1);
    	mp1->setTimeOffset(1);

    	mp2 = new MetaServerPacket();
    	mp2->setPacketType(NMT_SERVERSHAKE);
    	mp2->addPacketData(12345);
    	mp2->setSequence(2);
    	mp2->setTimeOffset(2);

		p = new PacketReader();
		file_name = "/tmp/packet_test.bin";
		pl = new PacketLogger(file_name);

    }

    void tearDown()
    {
    	delete p;
    	delete pl;
    	delete mp1;
    	delete mp2;
    }

    void test_initialSetup()
    {
    	/*
    	 * Verify Initial Pointers
    	 */
    	CPPUNIT_ASSERT(p);
    	CPPUNIT_ASSERT(pl);

    	/*
    	 * Verify object default construction
    	 */
    	CPPUNIT_ASSERT( p->getAppend() == false );

    }

    void test_PacketLogger_write()
    {

    	/*
    	 * check logger is clean
    	 */
    	CPPUNIT_ASSERT( pl->getCount() == 0 );

    	/*
    	 * add some packets
    	 */
    	pl->LogPacket(*mp1);
    	pl->LogPacket(*mp2);

    	/*
    	 * Make sure that they are received
    	 */
    	CPPUNIT_ASSERT(pl->getCount() == 2 );

    	/*
    	 * Force write to disk
    	 */
    	pl->flush(0);

    	/*
    	 * Make sure they are gone
    	 */
    	CPPUNIT_ASSERT(pl->getCount() == 0 );

    }

    void test_PacketReader_parseBinaryFile_empty()
    {
    	unsigned int i = p->parseBinaryFile("/tmp/file_does_not_exist");

    	/*
    	 * Any sort of file operation that would fail, should produce
    	 * a 'zero packets parsed' return.
    	 */
    	CPPUNIT_ASSERT( i == 0 );

    }

    void test_PacketReader_parseBinaryFile()
    {

    	pl->LogPacket(*mp1);
    	pl->LogPacket(*mp2);
    	pl->flush(0);

    	unsigned int i = p->parseBinaryFile(pl->getFile());
    	CPPUNIT_ASSERT( i == 2 );
    }
    void test_PacketReader_parseBinaryFile_append()
    {
    	/*
    	 * Add our 2 packets
    	 */
    	pl->LogPacket(*mp1);
    	pl->LogPacket(*mp2);
    	pl->flush(0);

    	/*
    	 * First parse, should have 2
    	 */
    	unsigned int i = p->parseBinaryFile(pl->getFile());
    	CPPUNIT_ASSERT( i == 2 );

    	/*
    	 * Trigger append mode
    	 */
    	p->setAppend(true);

    	/*
    	 * Second parse.  Since no reset, should have 4
    	 */
    	i = p->parseBinaryFile(pl->getFile());
    	CPPUNIT_ASSERT( i = 4 );

    	/*
    	 * And back again
    	 */
    	p->setAppend(false);
    	i = p->parseBinaryFile(pl->getFile());
    	CPPUNIT_ASSERT( i == 2 );

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(PacketLogging_integration);


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
