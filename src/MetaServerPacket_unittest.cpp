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


#include "MetaServer.hpp"
#include "MetaServerPacket.hpp"

#include <cppunit/TestCase.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <cassert>

static const int TEST_MAX_BYTES = 1024;

class MetaServerPacket_unittest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MetaServerPacket_unittest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testConstructor_zeroSize);
    CPPUNIT_TEST(testConstructor_nonzeroSize);
    CPPUNIT_TEST(testConstructor_negativeSize);
    CPPUNIT_TEST(test_setPacketType_returnmatch);
    CPPUNIT_TEST(test_getPacketType_constructor);
    CPPUNIT_TEST(test_getPacketType_setPacketType);
    CPPUNIT_TEST(test_getIntData_constructor);
    CPPUNIT_TEST(test_IpAsciiToNet_returnmatch);
    CPPUNIT_TEST(test_setAddress_getAddress);
    CPPUNIT_TEST(test_getAddressStr_return);
    CPPUNIT_TEST(test_getAddressInt_return);

    CPPUNIT_TEST_SUITE_END();
  public:
    MetaServerPacket_unittest() { }

    void setUp() {}
    void tearDown() {}

    void testConstructor() {
        boost::array<char, TEST_MAX_BYTES> test_buffer;

        MetaServerPacket * msp = new MetaServerPacket(test_buffer);
        CPPUNIT_ASSERT(msp);
        delete msp;
    }

    /*
     * Check that an empty construction has zero size
     */
    void testConstructor_zeroSize() {
    	boost::array<char, TEST_MAX_BYTES> test_buffer;

    	MetaServerPacket * msp = new MetaServerPacket(test_buffer);
    	unsigned int s = msp->getSize();
    	CPPUNIT_ASSERT( s == 0 );

    	delete msp;
    }

    /*
     * Check that a non-empty construction has size
     */
    void testConstructor_nonzeroSize() {
    	boost::array<char, TEST_MAX_BYTES> test_buffer;

    	test_buffer[0] = 0;
    	test_buffer[1] = 1;
    	test_buffer[2] = 2;
    	MetaServerPacket * msp = new MetaServerPacket(test_buffer, 3);
    	unsigned int s = msp->getSize();
    	CPPUNIT_ASSERT( s == 3 );
    	delete msp;
    }

    /*
     * Negative packet size test
     */
    void testConstructor_negativeSize() {
    	boost::array<char, TEST_MAX_BYTES> test_buffer;

    	test_buffer[0] = 0;
    	test_buffer[1] = 1;
    	test_buffer[2] = 2;

    	MetaServerPacket * msp = new MetaServerPacket(test_buffer, 3);
    	unsigned int s = msp->getSize();
    	CPPUNIT_ASSERT_ASSERTION_FAIL( CPPUNIT_ASSERT( s == 1 ) );
    	delete msp;
    }

    /*
     * Data Integrity Test
     */
    void test_getPacketType_constructor() {
       	boost::array<char, TEST_MAX_BYTES> test_buffer;
       	NetMsgType nmt;

       	// this sets the first 4 bytes to 1 ... which
       	// is equal to NMT_SERVERKEEPALIVE
        test_buffer[0] = 0;
        test_buffer[1] = 0;
        test_buffer[2] = 0;
        test_buffer[3] = 1;

        MetaServerPacket * msp = new MetaServerPacket(test_buffer, 4);

        nmt = msp->getPacketType();

        CPPUNIT_ASSERT( nmt == NMT_SERVERKEEPALIVE );
        delete msp;
    }

    /*
     * Tests that getPacketType returns the correct type when
     * set via setPacketType
     */
    void test_getPacketType_setPacketType() {

    	std::cerr << std::endl << "test_getPacketType_setPacketType(): start" << std::endl;
       	boost::array<char, TEST_MAX_BYTES> test_buffer;
       	NetMsgType nmt;

        MetaServerPacket * msp = new MetaServerPacket(test_buffer);

        msp->setPacketType(NMT_SERVERKEEPALIVE);

        nmt = msp->getPacketType();

        CPPUNIT_ASSERT( nmt == NMT_SERVERKEEPALIVE );
        delete msp;

        std::cerr << std::endl << "test_getPacketType_setPacketType(): end" << std::endl;
    }


    void test_setPacketType_returnmatch() {
    	boost::array<char, TEST_MAX_BYTES> test_buffer;
    	NetMsgType nmt;

    	MetaServerPacket * msp = new MetaServerPacket(test_buffer);

    	msp->setPacketType(NMT_NULL);

    	nmt = msp->getPacketType();

    	CPPUNIT_ASSERT( nmt == NMT_NULL );

    	delete msp;
    }

    void test_getIntData_constructor() {

        std::cout << std::endl << "test_getIntData_constructor: start" << std::endl;
    	boost::array<char, TEST_MAX_BYTES> test_buffer;

    	NetMsgType nmt,nmt2;
    	uint32_t data;

    	char * p = test_buffer.c_array();

    	// artificially fill buffer with a handshake packet
    	test_pack_uint32(NMT_HANDSHAKE,p);
    	p+=4;
    	test_pack_uint32(123456,p);

    	MetaServerPacket * msp = new MetaServerPacket(test_buffer,8);
    	MetaServerPacket * rsp = new MetaServerPacket(test_buffer,8);

    	nmt = msp->getPacketType();
    	nmt2 = msp->getIntData(0); // same as getPacketType by parsing
    	data = msp->getIntData(4);

        std::cout << std::endl << "msp-type: nmt : " << msp->getPacketType() << std::endl;
        std::cout << std::endl << "msp-data-0: data : " << msp->getIntData(0) << std::endl;
        std::cout << std::endl << "msp-data-4: data : " << msp->getIntData(4) << std::endl;

        std::cout << std::endl << "rsp-type: nmt : " << rsp->getPacketType() << std::endl;
        std::cout << std::endl << "rsp-data-0: data : " << rsp->getIntData(0) << std::endl;
        std::cout << std::endl << "rsp-data-4: data : " << rsp->getIntData(4) << std::endl;

    	CPPUNIT_ASSERT( nmt == NMT_HANDSHAKE );
    	CPPUNIT_ASSERT( nmt2 == NMT_HANDSHAKE );
    	CPPUNIT_ASSERT( data == 123456 );
    	CPPUNIT_ASSERT ( msp->getPacketType() == rsp->getPacketType() );
    	CPPUNIT_ASSERT ( msp->getIntData(0) == rsp->getIntData(0) );
    	CPPUNIT_ASSERT ( msp->getIntData(4) == rsp->getIntData(4) );

    	delete msp;
    	delete rsp;
        std::cout << std::endl << "test_getIntData_constructor: end" << std::endl;

    }

    /*
     *  Make sure that the string address going in, comes back as the correct
     *  decimal value.
     *  127.0.2.1
     *
     * String value	1.2.0.127
     * Binary	00000001 . 00000010 . 00000000 . 01111111
   	 * Integer	16908415
     */
    void test_IpAsciiToNet_returnmatch()
    {
    	boost::uint32_t r;
    	MetaServerPacket * msp = new MetaServerPacket();

    	r = msp->IpAsciiToNet("127.0.2.1");
    	delete msp;

    	CPPUNIT_ASSERT( r == 16908415 );

    }

    /*
     *  Set the address and insure round trip
     */
    void test_setAddress_getAddress()
    {
    	boost::asio::ip::address a = boost::asio::ip::address::from_string("127.0.2.1");

    	MetaServerPacket * msp = new MetaServerPacket();

    	msp->setAddress( a );

    	CPPUNIT_ASSERT( a == msp->getAddress() );

    	delete msp;
    }

    /*
     * Set address and insure that the round trip string
     * matches
     */
    void test_getAddressStr_return()
    {

    	MetaServerPacket * msp = new MetaServerPacket();

    	msp->setAddress( boost::asio::ip::address::from_string("127.0.2.1") );

    	CPPUNIT_ASSERT( msp->getAddressStr() == "127.0.2.1" );

    	delete msp;
    }

    /*
     * Set address and insure that the round trip
     * int value matches
     */
    void test_getAddressInt_return()
    {
    	MetaServerPacket * msp = new MetaServerPacket();

    	msp->setAddress( boost::asio::ip::address::from_string("127.0.2.1") );

    	CPPUNIT_ASSERT( msp->getAddressInt() == 16908415 );
    }

    void
    test_pack_uint32(uint32_t data, char *dest)
    {
        uint32_t netorder;

        netorder = htonl(data);
        memcpy(dest, &netorder, sizeof(uint32_t));
    }

    void
    test_unpack_uint32(uint32_t *dest, char* src)
    {
        uint32_t netorder;

        memcpy(&netorder, src, sizeof(uint32_t));
        *dest = ntohl(netorder);

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(MetaServerPacket_unittest);

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

// MetaServerPacket appears to be standalone and not require stubs
