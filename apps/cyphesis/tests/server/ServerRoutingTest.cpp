// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "server/ServerRouting.h"

#include "server/Account.h"

#include "rules/simulation/BaseWorld.h"
#include "common/id.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "../DatabaseNull.h"

#include <Atlas/Objects/Anonymous.h>

#include <iostream>

#include <cassert>
#include <server/Persistence.h>

class Entity;

static bool stub_generate_accounts = false;

#include "../TestWorld.h"

class TestRouter : public ConnectableRouter {
public:
	TestRouter(RouterId id) : ConnectableRouter(id) {}

	void externalOperation(const Operation&, Link&) override {}

	void operation(const Operation&, OpVector&) override {}

	void setConnection(Connection* connection) override {}

	Connection* getConnection() const override {
		return nullptr;
	}

	void addToEntity(const Atlas::Objects::Entity::RootEntity&) const override {}

};

class TestAccount : public Account {
public:
	TestAccount(Connection* conn, const std::string& username,
				const std::string& passwd,
				RouterId id) :
			Account(conn, username, passwd, std::move(id)) {
	}

	virtual int characterError(const Operation& op,
							   const Atlas::Objects::Root& ent,
							   OpVector& res) const {
		return 0;
	}
};

int main() {
	Monitors m;
	TestWorld world;

	std::string ruleset = "test_rules";
	std::string server_name = "test_svr";
	auto lobbyId = newId();


	{
		DatabaseNull database;
		Persistence persistence(database);
		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);
	}

	{
		DatabaseNull database;
		Persistence persistence(database);
		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);

		auto id = newId();

		server.addRouter(std::make_unique<TestRouter>(id));
		assert(server.getObjects().size() == 1);
	}

	{
		DatabaseNull database;
		Persistence persistence(database);
		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);

		auto id = newId();

		auto r = new TestRouter(id);
		server.addRouter(std::unique_ptr<TestRouter>(r));
		assert(server.getObjects().size() == 1);
	}

	{
		DatabaseNull database;
		Persistence persistence(database);
		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);

		auto id = newId();

		auto id2 = newId();

		ConnectableRouter* r = new TestRouter(id);
		server.addRouter(std::unique_ptr<ConnectableRouter>(r));
		assert(server.getObjects().size() == 1);

		ConnectableRouter* r2 = server.getObject(id);
		assert(r == r2);

		r2 = server.getObject(id2);
		assert(nullptr == r2);
	}

	{
		DatabaseNull database;
		Persistence persistence(database);
		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);

		auto id = newId();

		server.addAccount(std::make_unique<TestAccount>(nullptr, "bob", "", id));
		assert(server.getObjects().size() == 1);
	}

	{
		DatabaseNull database;
		Persistence persistence(database);
		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);

		auto id = newId();

		Account* ac = new TestAccount(0, "bob", "", id);
		server.addAccount(std::unique_ptr<Account>(ac));
		assert(server.getObjects().size() == 1);
		Account* rac = server.getAccountByName("bob");
		assert(rac == ac);
	}

	{
		DatabaseNull database;
		Persistence persistence(database);

		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);

		auto id = newId();

		Account* ac = new TestAccount(0, "bob", "", id);
		server.addAccount(std::unique_ptr<Account>(ac));
		assert(server.getObjects().size() == 1);
		Account* rac = server.getAccountByName("alice");
		assert(rac == 0);
	}

	{
		DatabaseNull database;
		Persistence persistence(database);
		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);

		auto id = newId();

		Account* rac = server.getAccountByName("alice");
		assert(rac == 0);
	}

	{
		stub_generate_accounts = true;
		DatabaseNull database;
		Persistence persistence(database);
		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);

		auto id = newId();

		Account* rac = server.getAccountByName("alice");
		assert(rac != 0);
	}

	{
		DatabaseNull database;
		Persistence persistence(database);
		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);

		Atlas::Message::MapType map;
		server.addToMessage(map);
		restricted_flag = true;
		server.addToMessage(map);
		restricted_flag = false;
	}

	{
		DatabaseNull database;
		Persistence persistence(database);
		ServerRouting server(world, persistence, ruleset, server_name, lobbyId);

		Atlas::Objects::Entity::Anonymous ent;
		server.addToEntity(ent);
		restricted_flag = true;
		server.addToEntity(ent);
		restricted_flag = false;
	}


	return 0;
}

// stubs

#include "server/Lobby.h"
#include "server/Persistence.h"

#include "common/const.h"
#include "common/Monitors.h"
#include "common/Variable.h"

#include <cstdio>
#include <cstdlib>


std::unique_ptr<Account> Persistence::getAccount(const std::string& name) {
	if (!stub_generate_accounts) {
		return 0;
	}

	auto id = newId();

	return std::make_unique<TestAccount>(nullptr, name, "", id);
}


#include "common/TypeNode_impl.h"


bool_config_register::bool_config_register(bool& var,
										   const char* section,
										   const char* setting,
										   const char* help) {
}


Ref<LocatedEntity> BaseWorld::getEntity(const std::string& id) const {
	return getEntity(integerId(id));
}

Ref<LocatedEntity> BaseWorld::getEntity(long id) const {
	auto I = m_eobjects.find(id);
	if (I != m_eobjects.end()) {
		assert(I->second);
		return I->second;
	} else {
		return nullptr;
	}
}


const char* const CYPHESIS = "cyphesis";

static const char* DEFAULT_INSTANCE = "cyphesis";

std::string instance(DEFAULT_INSTANCE);
int timeoffset = 0;
bool database_flag = false;
std::string assets_directory = "";


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