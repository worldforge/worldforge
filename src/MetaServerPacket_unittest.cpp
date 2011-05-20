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
    CPPUNIT_TEST_SUITE_END();
  public:
    MetaServerPacket_unittest() { }

    void testConstructor() {
        boost::array<char, TEST_MAX_BYTES> test_buffer;

        MetaServerPacket * msp = new MetaServerPacket(test_buffer);
        CPPUNIT_ASSERT(msp);
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
