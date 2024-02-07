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

#include "client/cyclient/BaseClientLegacy.h"
#include "../NullPropertyManager.h"
#include "client/SimpleTypeStore.h"

#include <Atlas/Objects/RootOperation.h>

#include <cassert>
#include "common/Property_impl.h"

using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;

Atlas::Objects::Factories factories;

class TestBaseClient : public BaseClientLegacy {
public:
	explicit TestBaseClient(boost::asio::io_context& io_context, TypeStore<MemEntity>& typeStore) : BaseClientLegacy(io_context, factories, typeStore) {}

	void idle() override {}
};

int main() {
	NullPropertyManager<MemEntity> propertyManager;
	SimpleTypeStore typeStore(propertyManager);
	boost::asio::io_context io_context;
	{
		auto bc = new TestBaseClient{io_context, typeStore};

		delete bc;
	}

	{
		auto bc = new TestBaseClient{io_context, typeStore};

		bc->createAccount("8e7e4452-f666-11df-8027-00269e5444b3", "84abee0c-f666-11df-8f7e-00269e5444b3");

		delete bc;
	}

	{
		auto bc = new TestBaseClient{io_context, typeStore};

		bc->createSystemAccount();

		delete bc;
	}

	{
		auto bc = new TestBaseClient{io_context, typeStore};

		bc->createCharacter("9e7f4004-f666-11df-a327-00269e5444b3");

		delete bc;
	}

	{
		auto bc = new TestBaseClient{io_context, typeStore};

		bc->logout();

		delete bc;
	}

	{
		auto bc = new TestBaseClient{io_context, typeStore};

		bc->handleNet();

		delete bc;
	}

	return 0;
}

// stubs

#include "client/cyclient/CreatorClient.h"

#include "common/log.h"

#include <cstdlib>


#include "common/TypeNode_impl.h"


RootOperation ClientConnection::pop() {
	return RootOperation(nullptr);
}


std::string create_session_username() {
	return "admin_test";
}
#include "rules/Location_impl.h"
