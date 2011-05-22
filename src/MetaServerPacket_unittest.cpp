// WorldForge Next Generation MetaServer
// Copyright (C) 2011 Alistair Riddoch

#include "MetaServer.hpp"
#include "MetaServerPacket.hpp"

#include <cppunit/TestCase.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <cassert>

static const int TEST_MAX_BYTES = 1024;

class MetaServerPacket_unittest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(MetaServerPacket_unittest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testConstructor_zeroSize);
    CPPUNIT_TEST(testConstructor_nonzeroSize);
    CPPUNIT_TEST(testConstructor_negativeSize);
    CPPUNIT_TEST(test_setPacketType_returnmatch);
    CPPUNIT_TEST(test_getPacketType_constructor);
    CPPUNIT_TEST(test_getPacketType_setPacketType);

    CPPUNIT_TEST_SUITE_END();
  public:
    MetaServerPacket_unittest() { }

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

// stubs

// MetaServerPacket appears to be standalone and not require stubs
