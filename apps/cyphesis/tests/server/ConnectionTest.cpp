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

#include "server/Connection.h"

#include "server/Account.h"
#include "rules/simulation/ExternalMind.h"
#include "rules/simulation/MindsProperty.h"
#include "rules/simulation/LocatedEntity.h"
#include "server/Lobby.h"
#include "server/Player.h"
#include "server/ServerRouting.h"

#include "rules/simulation/Inheritance.h"
#include "common/log.h"
#include "common/CommSocket.h"
#include "common/Property_impl.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>
#include <cstdio>

#include <cassert>
#include "common/Property_impl.h"
#include "common/Monitors.h"

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Get;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Move;

Atlas::Objects::Factories factories;

class TestCommSocket : public CommSocket {
public:
	TestCommSocket() : CommSocket(*(boost::asio::io_context*) 0) {
	}

	virtual void disconnect() {
	}

	virtual int flush() {
		return 0;
	}

};

class Connectiontest : public Cyphesis::TestBase {
private:
	ServerRouting* m_server;
	CommSocket* m_tcc;
	Connection* m_connection;
	Inheritance* m_inheritance;


	static bool Router_error_called;
	static bool Router_clientError_called;
public:
	Connectiontest();

	void setup();

	void teardown();

	void test_addNewAccount();

	void test_CreateOperation_empty();

	void test_CreateOperation_root_arg();

	void test_CreateOperation_restricted();

	void test_CreateOperation_empty_arg();

	void test_CreateOperation_account_by_id();

	void test_CreateOperation_number_username();

	void test_CreateOperation_no_passed();

	void test_CreateOperation_empty_password();

	void test_CreateOperation_username();

	void test_CreateOperation();

	void test_foo();

	void test_disconnectObject_empty();

	void test_disconnectObject_unused_Entity();

	void test_disconnectObject_used_Entity();

	void test_disconnectObject_others_used_Entity();

	void test_disconnectObject_unlinked_Entity();

	void test_disconnectObject_non_Entity();

	static void set_Router_error_called();

	static void set_Router_clientError_called();
};

bool Connectiontest::Router_error_called = false;
bool Connectiontest::Router_clientError_called = false;

void Connectiontest::set_Router_error_called() {
	Router_error_called = true;
}

void Connectiontest::set_Router_clientError_called() {
	Router_clientError_called = true;
}

Connectiontest::Connectiontest() {
	ADD_TEST(Connectiontest::test_addNewAccount);
	ADD_TEST(Connectiontest::test_CreateOperation_empty);
	ADD_TEST(Connectiontest::test_CreateOperation_root_arg);
	ADD_TEST(Connectiontest::test_CreateOperation_restricted);
	ADD_TEST(Connectiontest::test_CreateOperation_empty_arg);
	ADD_TEST(Connectiontest::test_CreateOperation_account_by_id);
	ADD_TEST(Connectiontest::test_CreateOperation_number_username);
	ADD_TEST(Connectiontest::test_CreateOperation_no_passed);
	ADD_TEST(Connectiontest::test_CreateOperation_empty_password);
	ADD_TEST(Connectiontest::test_CreateOperation_username);
	ADD_TEST(Connectiontest::test_CreateOperation);
	ADD_TEST(Connectiontest::test_foo);
	ADD_TEST(Connectiontest::test_disconnectObject_empty);
	ADD_TEST(Connectiontest::test_disconnectObject_empty);
	ADD_TEST(Connectiontest::test_disconnectObject_unused_Entity);
	ADD_TEST(Connectiontest::test_disconnectObject_used_Entity);
	ADD_TEST(Connectiontest::test_disconnectObject_others_used_Entity);
	ADD_TEST(Connectiontest::test_disconnectObject_unlinked_Entity);
	ADD_TEST(Connectiontest::test_disconnectObject_non_Entity);
}

void Connectiontest::setup() {
	m_inheritance = new Inheritance();
	Router_error_called = false;

	m_server = new ServerRouting(*(BaseWorld*) 0,
								 *(Persistence*) nullptr,
								 "noruleset",
								 "unittesting",
								 2);

	m_tcc = new TestCommSocket();
	m_connection = new Connection(*m_tcc, *m_server, "addr", 3);
}

void Connectiontest::teardown() {
	delete m_connection;
	delete m_tcc;
	delete m_server;
	delete m_inheritance;
}

void Connectiontest::test_addNewAccount() {
	Account* ac = m_connection->addNewAccount("player", "bob", "foo");

	ASSERT_NOT_NULL(ac);
	auto I = m_connection->m_connectableRouters.find(ac->getIdAsInt());
	ASSERT_TRUE(I != m_connection->m_connectableRouters.end());

	m_connection->disconnectObject(I->second,
								   "test_event");

	ASSERT_EQUAL(m_connection->m_routers.size(), 0u);
}

void Connectiontest::test_CreateOperation_empty() {
	Create op;
	OpVector res;
	m_connection->operation(op, res);
	ASSERT_EQUAL(m_connection->m_routers.size(), 0u);
	ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_root_arg() {
	Create op;
	OpVector res;
	op->setArgs1(Root());
	m_connection->operation(op, res);
	ASSERT_EQUAL(m_connection->m_routers.size(), 0u);
	ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_restricted() {
	Create op;
	OpVector res;
	restricted_flag = true;
	m_connection->operation(op, res);
	ASSERT_EQUAL(m_connection->m_routers.size(), 0u);
	ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_empty_arg() {
	Create op;
	OpVector res;
	restricted_flag = false;
	Anonymous op_arg;
	op->setArgs1(op_arg);
	m_connection->operation(op, res);
	ASSERT_EQUAL(m_connection->m_routers.size(), 0u);
	ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_account_by_id() {
	Create op;
	OpVector res;
	Anonymous op_arg;
	op->setArgs1(op_arg);
	op_arg->setId("jim");
	// Legacy op
	m_connection->operation(op, res);
	ASSERT_EQUAL(m_connection->m_routers.size(), 0u);
	ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_number_username() {
	Create op;
	OpVector res;
	Anonymous op_arg;
	op->setArgs1(op_arg);
	op_arg->setAttr("username", 1);
	// Malformed username
	m_connection->operation(op, res);
	ASSERT_EQUAL(m_connection->m_routers.size(), 0u);
	ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_no_passed() {
	Create op;
	OpVector res;
	Anonymous op_arg;
	op->setArgs1(op_arg);
	op_arg->setAttr("username", "jim");
	// username, no password
	m_connection->operation(op, res);
	ASSERT_EQUAL(m_connection->m_routers.size(), 0u);
	ASSERT_TRUE(Router_error_called);
}

void Connectiontest::test_CreateOperation_empty_password() {
	Create op;
	OpVector res;
	Anonymous op_arg;
	op->setArgs1(op_arg);
	op_arg->setAttr("username", "jim");
	op_arg->setAttr("password", "");
	// zero length password
	m_connection->operation(op, res);
	ASSERT_EQUAL(m_connection->m_routers.size(), 0u);
	ASSERT_TRUE(Router_clientError_called);
}

void Connectiontest::test_CreateOperation_username() {
	Create op;
	OpVector res;
	Anonymous op_arg;
	op->setArgs1(op_arg);
	op_arg->setAttr("username", "");
	op_arg->setAttr("password", "foo");
	// zero length username
	m_connection->operation(op, res);
	ASSERT_EQUAL(m_connection->m_routers.size(), 0u);
	ASSERT_TRUE(Router_clientError_called);
}

void Connectiontest::test_CreateOperation() {
	Create op;
	OpVector res;
	Anonymous op_arg;
	op->setArgs1(op_arg);
	op_arg->setAttr("username", "jim");
	op_arg->setAttr("password", "foo");
	// valid username and password
	m_connection->operation(op, res);
	ASSERT_EQUAL(m_connection->m_routers.size(), 1u);
}

void Connectiontest::test_foo() {
	{
		Login op;
		OpVector res;
		m_connection->operation(op, res);
		op->setArgs1(Root());
		m_connection->operation(op, res);
		Anonymous op_arg;
		op->setArgs1(op_arg);
		m_connection->operation(op, res);
		op_arg->setId("bob");
		m_connection->operation(op, res);
		op_arg->setAttr("username", 1);
		m_connection->operation(op, res);
		op_arg->setAttr("username", "");
		m_connection->operation(op, res);
		op_arg->setAttr("username", "bob");
		m_connection->operation(op, res);
		op_arg->setAttr("password", "foo");
		m_connection->operation(op, res);
		m_connection->operation(op, res);
	}

	{
		Get op;
		OpVector res;
		m_connection->operation(op, res);
		Root op_arg;
		op->setArgs1(op_arg);
		m_connection->operation(op, res);
		op_arg->setId("1");
		m_connection->operation(op, res);
		op_arg->setId("game_entity");
		m_connection->operation(op, res);
	}

	{
		Logout op;
		OpVector res;
		m_connection->operation(op, res);
		op->setSerialno(24);
		m_connection->operation(op, res);
		Root op_arg;
		op->setArgs1(op_arg);
		m_connection->operation(op, res);
		op_arg->setId("-1");
		m_connection->operation(op, res);
		op_arg->setId("23");
		m_connection->operation(op, res);
		// How to determine the real ID?
		auto& rm = m_connection->m_routers;
		auto I = rm.begin();
		for (; I != rm.end(); ++I) {
			std::string object_id = fmt::format("{}", I->first);
			std::cout << "ID: " << object_id << std::endl;
			op_arg->setId(object_id);
			m_connection->operation(op, res);
		}
	}

}

void Connectiontest::test_disconnectObject_empty() {
	// setup
	Player ac(m_connection,
			  "jim",
			  "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
			  4);

	ac.setConnection(m_connection);
	m_connection->m_routers[ac.getIdAsInt()].router = &ac;
	m_connection->m_connectableRouters[ac.getIdAsInt()] = &ac;

	auto I = m_connection->m_connectableRouters.find(ac.getIdAsInt());
	assert(I != m_connection->m_connectableRouters.end());

	m_connection->disconnectObject(I->second, "test_disconnect_account");

	ASSERT_TRUE(m_connection->m_routers.find(ac.getIdAsInt()) ==
				m_connection->m_routers.end());
}

void Connectiontest::test_disconnectObject_unused_Entity() {
	// setup
	Player ac(m_connection,
			  "jim",
			  "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
			  4);

	ac.setConnection(m_connection);
	m_connection->m_routers[ac.getIdAsInt()].router = &ac;
	m_connection->m_connectableRouters[ac.getIdAsInt()] = &ac;

	auto I = m_connection->m_connectableRouters.find(ac.getIdAsInt());
	assert(I != m_connection->m_connectableRouters.end());

	Ref<LocatedEntity> avatar(new LocatedEntity(5));
	m_connection->m_routers[avatar->getIdAsInt()].router = avatar.get();
	ac.addCharacter(avatar);

	m_connection->disconnectObject(I->second, "test_disconnect_account");

	ASSERT_TRUE(m_connection->m_routers.find(ac.getIdAsInt()) ==
				m_connection->m_routers.end());

	//TODO: Needs to be an integration test.
//    ASSERT_TRUE(m_connection-> m_objects.find(avatar->getIntId()) ==
//                m_connection->m_objects.end());
}

void Connectiontest::test_disconnectObject_used_Entity() {
	// setup
	Player ac(m_connection,
			  "jim",
			  "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
			  4);

	ac.setConnection(m_connection);
	m_connection->m_routers[ac.getIdAsInt()].router = &ac;
	m_connection->m_connectableRouters[ac.getIdAsInt()] = &ac;

	auto I = m_connection->m_connectableRouters.find(ac.getIdAsInt());
	assert(I != m_connection->m_connectableRouters.end());

	Ref<LocatedEntity> avatar(new LocatedEntity(5));
	ExternalMind mind(6, avatar);
	avatar->requirePropertyClassFixed<MindsProperty>().addMind(&mind);
	mind.linkUp(m_connection);
	m_connection->m_routers[avatar->getIdAsInt()].router = avatar.get();
	ac.addCharacter(avatar);

	m_connection->disconnectObject(I->second, "test_disconnect_account");

	ASSERT_TRUE(m_connection->m_routers.find(ac.getIdAsInt()) ==
				m_connection->m_routers.end());

	// The LocatedEntity was in use, so it stays connected
	ASSERT_TRUE(m_connection->m_routers.find(avatar->getIdAsInt()) !=
				m_connection->m_routers.end());
}

void Connectiontest::test_disconnectObject_others_used_Entity() {
	// setup
	Player ac(m_connection,
			  "jim",
			  "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
			  4);

	ac.setConnection(m_connection);
	m_connection->m_routers[ac.getIdAsInt()].router = &ac;
	m_connection->m_connectableRouters[ac.getIdAsInt()] = &ac;

	auto I = m_connection->m_connectableRouters.find(ac.getIdAsInt());
	assert(I != m_connection->m_connectableRouters.end());

	TestCommSocket otcc{};
	Connection conn(otcc, *m_server, "addr", 6);

	Ref<LocatedEntity> avatar(new LocatedEntity(5));
	ExternalMind mind(6, avatar);
	avatar->requirePropertyClassFixed<MindsProperty>().addMind(&mind);
	mind.linkUp(m_connection);
	m_connection->m_routers[avatar->getIdAsInt()].router = avatar.get();
	ac.addCharacter(avatar);

	m_connection->disconnectObject(I->second, "test_disconnect_account");

	ASSERT_TRUE(m_connection->m_routers.find(ac.getIdAsInt()) ==
				m_connection->m_routers.end());

	// The LocatedEntity was in use by another connection, so it is removed
	// from this one.
	//TODO: Needs to be an integration test.

//    ASSERT_TRUE(m_connection-> m_objects.find(avatar->getIntId()) ==
//                m_connection->m_objects.end());
}

void Connectiontest::test_disconnectObject_unlinked_Entity() {
	// setup
	Player ac(m_connection,
			  "jim",
			  "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
			  4);

	ac.setConnection(m_connection);
	m_connection->m_routers[ac.getIdAsInt()].router = &ac;
	m_connection->m_connectableRouters[ac.getIdAsInt()] = &ac;

	auto I = m_connection->m_connectableRouters.find(ac.getIdAsInt());
	assert(I != m_connection->m_connectableRouters.end());

	Ref<LocatedEntity> avatar(new LocatedEntity(5));
	ExternalMind mind(6, avatar);
	avatar->requirePropertyClassFixed<MindsProperty>().addMind(&mind);
	m_connection->m_routers[avatar->getIdAsInt()].router = avatar.get();
	ac.addCharacter(avatar);

	m_connection->disconnectObject(I->second, "test_disconnect_account");

	ASSERT_TRUE(m_connection->m_routers.find(ac.getIdAsInt()) ==
				m_connection->m_routers.end());

	//TODO: Needs to be an integration test.

//    ASSERT_TRUE(m_connection-> m_objects.find(avatar->getIntId()) ==
//                m_connection->m_objects.end());
}

void Connectiontest::test_disconnectObject_non_Entity() {
	// setup
	Player ac(m_connection,
			  "jim",
			  "1e0ce8e9-304b-470c-83c4-feab11f9a2e4",
			  4);

	ac.setConnection(m_connection);
	m_connection->m_routers[ac.getIdAsInt()].router = &ac;
	m_connection->m_connectableRouters[ac.getIdAsInt()] = &ac;

	auto I = m_connection->m_connectableRouters.find(ac.getIdAsInt());
	assert(I != m_connection->m_connectableRouters.end());

	Ref<LocatedEntity> avatar(new LocatedEntity(5));
	m_connection->m_routers[avatar->getIdAsInt()].router = avatar.get();
	ac.addCharacter(avatar.get());

	m_connection->disconnectObject(I->second, "test_disconnect_account");

	ASSERT_TRUE(m_connection->m_routers.find(ac.getIdAsInt()) ==
				m_connection->m_routers.end());

	//TODO: Needs to be an integration test.
//    ASSERT_TRUE(m_connection-> m_objects.find(avatar->getIntId()) ==
//                m_connection->m_objects.end());
}

int main() {
	Monitors m;
	Connectiontest t;

	return t.run();
}

// Stubs

#include "rules/simulation/BaseWorld.h"

bool restricted_flag;

namespace Atlas {
namespace Objects {
namespace Operation {
int UPDATE_NO = -1;
}
}
}


int CommSocket::flush() {
	return 0;
}

// Simplified stub version to allow us to test Connection::disconnectObject
void Account::addCharacter(const Ref<LocatedEntity>& chr) {

	m_charactersDict[chr->getIdAsInt()] = chr;
}

void ExternalMind::linkUp(Link* c) {
	m_link = c;
}

void Router::error(const Operation& op,
				   const std::string& errstring,
				   OpVector& res,
				   const std::string& to) const {
	Connectiontest::set_Router_error_called();
}

void Router::clientError(const Operation& op,
						 const std::string& errstring,
						 OpVector& res,
						 const std::string& to) const {
	Connectiontest::set_Router_clientError_called();
}

#include "common/TypeNode_impl.h"
#include "rules/Location_impl.h"

const Atlas::Objects::Root& Inheritance::getClass(const std::string& typeName, Visibility) const {
	return noClass;
}

const TypeNode<LocatedEntity>* Inheritance::getType(const std::string& typeName) const {
	auto I = atlasObjects.find(typeName);
	if (I == atlasObjects.end()) {
		return 0;
	}
	return I->second.get();
}

void hash_password(const std::string& pwd, const std::string& salt,
				   std::string& hash) {
}

void addToEntity(const Vector3D& v, std::vector<double>& vd) {
	vd.resize(3);
	vd[0] = v[0];
	vd[1] = v[1];
	vd[2] = v[2];
}

int check_password(const std::string& pwd, const std::string& hash) {
	return 0;
}

#include <common/Shaker.h>

Shaker::Shaker() {
}

std::string Shaker::generateSalt(size_t length) {
	return "";
}

long stubId = 1;

RouterId newId() {
	return RouterId{stubId++};
}

void Account::store() const {
}

Account* ServerRouting::getAccountByName(const std::string& username) {
	return nullptr;
}
void Account::addToEntity(const Atlas::Objects::Entity::RootEntity& ent) const {
}

void ServerRouting::addToEntity(const RootEntity& ent) const {
}