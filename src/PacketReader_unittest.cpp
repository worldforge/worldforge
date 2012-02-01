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

 */

/*
 * Local Includes
 */
#include "PacketReader.hpp"


/*
 * System Includes
 */
#include <cppunit/TestCase.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <cassert>

class PacketReader_unittest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(PacketReader_unittest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(test_getAppend_setAppend);
    CPPUNIT_TEST(test_hasPacket);
    CPPUNIT_TEST(test_push_pop);
    CPPUNIT_TEST_SUITE_END();
  public:
    PacketReader_unittest() { }

    PacketReader* p;

    void setUp()
    {
		p = new PacketReader();
    }

    void tearDown()
    {
    	delete p;
    }

    void testConstructor()
    {
    	CPPUNIT_ASSERT(p);
    }

    void test_getAppend_setAppend()
    {
    	/*
    	 * Verify object default construction
    	 */
    	CPPUNIT_ASSERT( p->getAppend() == false );

    	/*
    	 * Validate setter
    	 */
    	p->setAppend(true);
    	CPPUNIT_ASSERT( p->getAppend() == true );
    }

    void test_hasPacket()
    {

    	/*
    	 * We can test with an empty packet ( for now )
    	 */
    	MetaServerPacket* mp = new MetaServerPacket();

    	/*
    	 * Verify object default construction
    	 */
    	CPPUNIT_ASSERT( p->hasPacket() == false );

    	/*
    	 * Add packet and check again
    	 */
    	p->push(*mp);
    	CPPUNIT_ASSERT( p->hasPacket() == true );

    	delete mp;
    }

    void test_push_pop()
    {
    	/*
    	 * Move packets around and make sure all is still well
    	 */
    	MetaServerPacket* mp1 = new MetaServerPacket();
    	mp1->setPacketType(NMT_SERVERKEEPALIVE);
    	mp1->setSequence(1);
    	mp1->setTimeOffset(1);


    	MetaServerPacket* mp2 = new MetaServerPacket();
    	mp2->setPacketType(NMT_SERVERSHAKE);
    	mp2->addPacketData(12345);
    	mp2->setSequence(2);
    	mp2->setTimeOffset(2);

    	/*
    	 * Validate some expected preconditions
    	 */
    	CPPUNIT_ASSERT( p->hasPacket() == false );
    	CPPUNIT_ASSERT( p->packetCount() == 0 );

    	/*
    	 * Push Some Items on
    	 */
    	p->push(*mp1);
    	CPPUNIT_ASSERT( p->hasPacket() == true );
    	CPPUNIT_ASSERT( p->packetCount() == 1 );

    	p->push(*mp2);
    	CPPUNIT_ASSERT( p->packetCount() == 2 );

    	/*
    	 * Take some items off
    	 */
    	MetaServerPacket ret = p->pop();
    	CPPUNIT_ASSERT( ret.getSequence() == 1 );
    	CPPUNIT_ASSERT( p->packetCount() == 1 );

    	MetaServerPacket ret2 = p->pop();
    	CPPUNIT_ASSERT( ret2.getSequence() == 2 );
    	CPPUNIT_ASSERT( p->hasPacket() == false );
    	CPPUNIT_ASSERT( p->packetCount() == 0 );

    	delete mp1;
    	delete mp2;

    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PacketReader_unittest);


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
MetaServerPacket::~MetaServerPacket() {}
MetaServerPacket::MetaServerPacket() :
  m_packetPayload( *new boost::array<char,MAX_PACKET_BYTES>() ),
  m_Bytes(0),
  m_packetType(NMT_NULL),
  m_Port(0),
  m_AddressInt(0),
  m_needFree(true),
  m_Sequence(0),
  m_TimeOffset(0)
{}
void MetaServerPacket::setPacketType(unsigned int const&) {}
unsigned int MetaServerPacket::addPacketData(boost::uint32_t i) { return 4; }

std::ostream& operator<<( std::ostream &os, const MetaServerPacket &mp) { return os; }
std::istream & operator>>( std::istream& is, MetaServerPacket &mp ) { return is; }





