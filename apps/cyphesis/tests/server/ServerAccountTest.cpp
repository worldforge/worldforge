// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../TestBase.h"
#include "../TestWorld.h"
#include "rules/simulation/LocatedEntity.h"
#include "server/ServerAccount.h"

#include "server/Connection.h"
#include "server/ServerRouting.h"
#include "server/PossessionAuthenticator.h"

#include "common/CommSocket.h"

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
#include "rules/simulation/AtlasProperties.h"
#include "rules/PhysicalProperties.h"
#include "common/Monitors.h"

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;

class ServerAccounttest : public Cyphesis::TestBase {
protected:
	long m_id_counter;

	ServerRouting* m_server;
	Connection* m_connection;
	ServerAccount* m_account;
	PossessionAuthenticator* m_possessionAuthenticator;
	TestWorld* m_world;


	static Ref<LocatedEntity> TestWorld_addNewEntity_ret_value;
public:
	ServerAccounttest();

	void setup();

	void teardown();

	void test_getType();


	static Ref<LocatedEntity> get_TestWorld_addNewEntity_ret_value();
};

Ref<LocatedEntity> ServerAccounttest::TestWorld_addNewEntity_ret_value;

Ref<LocatedEntity> ServerAccounttest::get_TestWorld_addNewEntity_ret_value() {
	return TestWorld_addNewEntity_ret_value;
}

ServerAccounttest::ServerAccounttest() : m_id_counter(0L),
										 m_server(0),
										 m_connection(0),
										 m_account(0) {
	ADD_TEST(ServerAccounttest::test_getType);

}

void ServerAccounttest::setup() {

	Ref<LocatedEntity> gw = new LocatedEntity(m_id_counter++);
	TestWorld::extension.addNewEntityFn = [&, gw](const std::string&,
												  const Atlas::Objects::Entity::RootEntity&) {
		auto ne = ServerAccounttest::get_TestWorld_addNewEntity_ret_value();
		if (ne) {
			ne->m_parent = gw.get();
			ne->requirePropertyClassFixed<PositionProperty<LocatedEntity>>().data() = Point3D(0, 0, 0);
		}
		return ne;
	};
	m_world = new TestWorld(gw);

	m_server = new ServerRouting(*m_world,
								 *(Persistence*) nullptr,
								 "5529d7a4-0158-4dc1-b4a5-b5f260cac635",
								 "bad621d4-616d-4faf-b9e6-471d12b139a9",
								 m_id_counter++);
	m_connection = new Connection(*(CommSocket*) 0, *m_server,
								  "8d18a4e8-f14f-4a46-997e-ada120d5438f",
								  m_id_counter++);
	m_account = new ServerAccount(m_connection,
								  "6c9f3236-5de7-4ba4-8b7a-b0222df0af38",
								  "fa1a03a2-a745-4033-85cb-bb694e921e62",
								  m_id_counter++);

	m_possessionAuthenticator = new PossessionAuthenticator();
}

void ServerAccounttest::teardown() {
	delete m_possessionAuthenticator;
	delete m_account;
	delete m_connection;
	delete m_server;
	delete m_world;
}

void ServerAccounttest::test_getType() {
	const char* type = m_account->getType();

	ASSERT_EQUAL(std::string("server"), type);
}


int main() {
	Monitors m;
	ServerAccounttest t;

	return t.run();
}

// stubs

#include "server/Persistence.h"

#include "common/globals.h"


#include "rules/PhysicalProperties_impl.h"
#include "common/Property_impl.h"

void Router::error(const Operation& op,
				   const std::string& errstring,
				   OpVector& res,
				   const std::string& to) const {
	res.push_back(Atlas::Objects::Operation::Error());
}


#include "rules/Location_impl.h"


bool database_flag = false;

#include <common/Shaker.h>
#include "rules/simulation/ExternalMind.h"

Shaker::Shaker() {
}

std::string Shaker::generateSalt(size_t length) {
	return "";
}

