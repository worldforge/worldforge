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

#include "server/TrustedConnection.h"

#include "rules/simulation/LocatedEntity.h"
#include "server/Account.h"
#include "rules/simulation/ExternalMind.h"
#include "server/Lobby.h"
#include "server/Player.h"
#include "server/ServerRouting.h"
#include "rules/simulation/Task.h"
#include "rules/simulation/WorldRouter.h"
#include "server/SystemAccount.h"

#include "rules/simulation/Inheritance.h"
#include "common/log.h"
#include "common/CommSocket.h"
#include "common/Monitors.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cstdlib>
#include <cstdio>

#include <cassert>

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

class TestConnection : public TrustedConnection {
public:
	TestConnection(CommSocket& cc, ServerRouting& svr,
				   const std::string& addr, RouterId id) :
			TrustedConnection(cc, svr, addr, id) {

	}

	std::unique_ptr<Account> test_newAccount(const std::string& type,
											 const std::string& username,
											 const std::string& passwd,
											 RouterId id) {
		return newAccount(type, username, passwd, id);
	}

	int test_verifyCredentials(const Account& ac,
							   const Atlas::Objects::Root& arg) const {
		return verifyCredentials(ac, arg);
	}

	size_t numObjects() const {
		return m_routers.size();
	}

	const std::map<long, RouterWithQueue>& getObjects() const {
		return m_routers;
	}

	void removeObject(Router* obj) {
		auto I = m_routers.find(obj->getIdAsInt());
		if (I != m_routers.end()) {
			m_routers.erase(I);
		}
	}
};

int main() {
	Monitors m;
	// WorldRouter world(SystemTime());
	// Entity & e = world.m_gameWorld;

	ServerRouting server(*(BaseWorld*) 0, *(Persistence*) nullptr, "noruleset", "unittesting", 2);

	TestCommSocket tcc{};
	TestConnection tc(tcc, server, "addr", 3);

	{
		auto ac = tc.test_newAccount("_non_type_",
									 "bob",
									 "unit_test_hash",
									 1);

		assert(ac.get() != 0);
	}

	{
		auto ac = tc.test_newAccount("system_account",
									 "bob",
									 "unit_test_hash",
									 1);

		assert(ac.get() != 0);
	}

	{
		auto ac = tc.test_newAccount("admin",
									 "bob",
									 "unit_test_hash",
									 1);

		assert(ac.get() != 0);
	}

	{
		auto ac = tc.test_newAccount("player",
									 "bob",
									 "unit_test_hash",
									 1);

		assert(ac.get() != 0);
	}

	{
		Account* ac = new Player(0, "bill", "unit_test_password", 2);
		Atlas::Objects::Root creds;

		int ret = tc.test_verifyCredentials(*ac, creds);

		assert(ret == -1);
		delete ac;
	}
}

// Stubs

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

void ExternalMind::linkUp(Link* c) {
	m_link = c;
}


#include "common/TypeNode_impl.h"
#include "rules/Location_impl.h"

const Atlas::Objects::Root& Inheritance::getClass(const std::string& parent, Visibility) const {
	return noClass;
}


const TypeNode<LocatedEntity>* Inheritance::getType(const std::string& parent) const {
	auto I = atlasObjects.find(parent);
	if (I == atlasObjects.end()) {
		return 0;
	}
	return I->second.get();
}

void encrypt_password(const std::string& pwd, std::string& hash) {
}

static long idGenerator = 0;

RouterId newId() {
	long new_id = ++idGenerator;
	return {new_id};
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
