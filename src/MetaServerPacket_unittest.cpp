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
