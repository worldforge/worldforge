// Eris Online RPG Protocol Library
// Copyright (C) 2007 Alistair Riddoch
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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Eris/Avatar.h>

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/Entity.h>
#include <Eris/View.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>
#include <Eris/TypeInfo.h>
#include <Eris/TypeService.h>
#include <Eris/EventService.h>

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

#include <wfmath/atlasconv.h>

#include "signalHelpers.h"

#include <iostream>

using namespace Eris;
using namespace Atlas::Objects::Operation;

boost::asio::io_context io_service;
Eris::EventService event_service(io_service);

class TestConnection : public Eris::Connection {
public:
	TestConnection(const std::string& name,
				   const std::string& host,
				   short port,
				   bool debug) :
			Eris::Connection(io_service, event_service, name, host, port) {}

	virtual void send(const Atlas::Objects::Root& obj) {
		std::cout << "Sending " << obj->getParent()
				  << std::endl;
	}
};

class TestAccount : public Eris::Account {
public:
	TestAccount(Eris::Connection& con) : Eris::Account(con) {}

	void setup_insertActiveCharacters(Eris::Avatar* ea) {
		m_activeAvatars.emplace(ea->getId(), std::unique_ptr<Eris::Avatar>(ea));
	}
};

class TestAvatar : public Eris::Avatar {
public:
	TestAvatar(Eris::Account* ac, std::string mind_id, std::string ent_id) :
			Eris::Avatar(*ac, mind_id, ent_id) {}

	void setup_setEntity(Eris::Entity* ent) {
		m_entity = ent;
		m_entityId = ent->getId();
	}
};

class TestEntity : public Eris::Entity {
public:
	TestEntity(const std::string& id, Eris::TypeInfo* ty) :
			Eris::Entity(id, ty, {
				.fetchEntity=[](const std::string&) { return nullptr; },
				.getEntity=[](const std::string&) { return nullptr; },
				.taskUpdated=[](Eris::Task&) {}
		}) {}

	void setup_setLocation(Eris::Entity* e) {
		setLocation(e);
	}

	void setup_setAttr(const std::string& p, const Atlas::Message::Element& v) {
		setProperty(p, v);
	}

	void setup_init(const Atlas::Objects::Entity::RootEntity& ge) {
		init(ge);
	}

	void setup_setFromRoot(const Atlas::Objects::Root& obj, bool allowMotion) {
		setFromRoot(obj, allowMotion);
	}


};

class TestTypeService : public Eris::TypeService {
public:
	TestTypeService(Eris::Connection& con) : Eris::TypeService(con) {
	}

	Eris::TypeInfo* setup_DefineBuiltin(const std::string& name, TypeInfo* parent) {
		return Eris::TypeService::defineBuiltin(name, parent);
	}

	void setup_recvTypeInfo(const Atlas::Objects::Root& atype) {
		Eris::TypeService::recvTypeInfo(atype);
	}

};

int main() {

	TestConnection con("name", "localhost", 6767, true);

	TestAccount acc(con);

	std::string fake_char_id("1");
	std::string fake_mind_id("12");
	TestAvatar* ea = new TestAvatar(&acc, fake_mind_id, fake_char_id);
	acc.setup_insertActiveCharacters(ea);
	TestEntity char_ent(fake_char_id, 0);
	ea->setup_setEntity(&char_ent);

	TestTypeService typeService(con);

	auto rootType = typeService.getTypeByName("root");
	assert(rootType);

	auto level1Type = typeService.getTypeByName("level1Type");
	assert(level1Type);
	assert(!level1Type->isBound());
	{
		Info typeInfo;
		typeInfo->setId("level1Type");
		typeInfo->setParent("root");
		Atlas::Message::MapType attributes;
		attributes["level"] = 1.0f;
		attributes["level1"] = true;

		typeInfo->setAttr("properties", attributes);
		typeService.setup_recvTypeInfo(typeInfo);
	}
	assert(level1Type->isBound());
	assert(level1Type->getProperties().find("level") != level1Type->getProperties().end());
	assert(level1Type->getProperties().find("level")->second.isNum());
	assert(level1Type->getProperties().find("level")->second.asNum() == 1.0f);

	assert(level1Type->getProperty("level1") && *(level1Type->getProperty("level1")) == Atlas::Message::Element(true));


	auto level2Type = typeService.getTypeByName("level2Type");
	assert(level2Type);
	assert(!level2Type->isBound());
	{
		Info typeInfo;
		typeInfo->setId("level2Type");
		typeInfo->setParent("level1Type");
		Atlas::Message::MapType attributes;
		Atlas::Message::MapType level;
		attributes["level"] = 2.0f;
		attributes["level2"] = true;
		attributes["velocity"] = WFMath::Vector<3>(3, 2, 1).toAtlas();
		typeInfo->setAttr("properties", attributes);
		typeService.setup_recvTypeInfo(typeInfo);
	}
	assert(level2Type->isBound());
	assert(level2Type->getParent());
	assert(level2Type->getParent() == level1Type);

	assert(level2Type->getProperties().find("level") != level2Type->getProperties().end());
	assert(level2Type->getProperties().find("level")->second.isNum());
	assert(level2Type->getProperties().find("level")->second.asNum() == 2.0f);

	assert(level2Type->getProperty("level1") && *level2Type->getProperty("level1") == Atlas::Message::Element(true));
	assert(level2Type->getProperty("level2") && *level2Type->getProperty("level2") == Atlas::Message::Element(true));

	{
		TestEntity ent("2", level1Type);
		ent.setup_init(Atlas::Objects::Entity::RootEntity());
		assert(ent.hasProperty("level"));
		assert(ent.valueOfProperty("level") == 1.0f);
	}

	{
		TestEntity ent("2", level2Type);
		ent.setup_init(Atlas::Objects::Entity::RootEntity());
		assert(ent.hasProperty("level"));
		assert(ent.valueOfProperty("level") == 2.0f);

		ent.setup_setAttr("level", "entity");
		assert(ent.valueOfProperty("level") == "entity");

		assert(!ent.getPosition().isValid());

		level1Type->setProperty("pos", WFMath::Point<3>(1, 2, 3).toAtlas());
		assert(ent.getPosition().isValid());

		assert(ent.getVelocity().isValid());
		assert(ent.getVelocity() == WFMath::Vector<3>(3, 2, 1));

		{
			Atlas::Objects::Entity::Anonymous what;
			what->setId(ent.getId());
			what->setAttr("velocity", WFMath::Vector<3>(30, 20, 10).toAtlas());
			ent.setup_setFromRoot(what, true);
		}
		assert(ent.getVelocity() == WFMath::Vector<3>(30, 20, 10));

		{
			Atlas::Objects::Entity::Anonymous what;
			what->setId(ent.getId());
			what->setAttr("foo", "bar");
			ent.setup_setFromRoot(what, true);
		}
		///Setting another attribute should not make the entity default to the type info attributes
		assert(ent.getVelocity() == WFMath::Vector<3>(30, 20, 10));
	}


	SignalCounter2<const std::string&, const Atlas::Message::Element&> level_1_Counter;
	level1Type->PropertyChanges.connect(sigc::mem_fun(level_1_Counter, &SignalCounter2<const std::string&, const Atlas::Message::Element&>::fired));
	SignalCounter2<const std::string&, const Atlas::Message::Element&> level_2_Counter;
	level2Type->PropertyChanges.connect(sigc::mem_fun(level_2_Counter, &SignalCounter2<const std::string&, const Atlas::Message::Element&>::fired));

	///In our first test, only the level 1 type should emit the attribute changes signal (since the level attribute is defined in the level 2 type also)
	level1Type->setProperty("level", 10);
	assert(level_1_Counter.fireCount() == 1);
	assert(level_2_Counter.fireCount() == 0);

	level_1_Counter.reset();
	level_2_Counter.reset();
	///In our second test, both ot the types should emit the signal
	level1Type->setProperty("level1", 10);
	assert(level_1_Counter.fireCount() == 1);
	assert(level_2_Counter.fireCount() == 1);


	return 0;
}
