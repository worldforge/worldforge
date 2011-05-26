/**
 Worldforge Next Generation MetaServer

 Copyright (C) 2011 Alistair Riddoch <alriddoch@googlemail.com>

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

#include "MetaServerHandlerTCP.hpp"
#include "MetaServerHandlerUDP.hpp"
#include "MetaServerPacket.hpp"

#include <cppunit/TestCase.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <cassert>

class MetaServer_unittest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(MetaServer_unittest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST_SUITE_END();
  public:
    MetaServer_unittest() { }

    void testConstructor() {
        boost::asio::io_service io_service;

        MetaServer * ms = new MetaServer(io_service);
        CPPUNIT_ASSERT(ms);
        delete ms;
    }
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

// stubs

// MetaServer won't currently link to this file as it contains the main function

std::string
MetaServerPacket::getAddress()
{
    return "";
}

unsigned int
MetaServerPacket::getPacketType()
{
	return 0;
}

void
MetaServerPacket::setPacketType(unsigned int)
{
}

unsigned int
MetaServerPacket::addPacketData(uint32_t)
{
}

void
MetaServerPacket::dumpBuffer()
{
}

unsigned int
MetaServerPacket::addPacketData(std::string s)
{

}

uint32_t
MetaServerPacket::getIntData(unsigned int)
{
}

unsigned int
MetaServerPacket::getPort()
{
}

MetaServerPacket::~MetaServerPacket()
{
}
