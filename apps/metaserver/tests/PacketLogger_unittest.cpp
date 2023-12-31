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

class PacketLogger_unittest : public CppUnit::TestCase {
	CPPUNIT_TEST_SUITE(PacketLogger_unittest);
	CPPUNIT_TEST(testConstructor);

	CPPUNIT_TEST_SUITE_END();

public:
	PacketLogger_unittest() {}

	PacketLogger* p;
	std::string fn;

	void setUp() {
		fn = "/tmp/pllist";
		p = new PacketLogger(fn);
	}

	void tearDown() {
		p->close();
		delete p;
	}

	void testConstructor() {
		CPPUNIT_ASSERT(p);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PacketLogger_unittest);


int main() {
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

std::ostream& operator<<(std::ostream& os, const MetaServerPacket& mp) { return os; }


