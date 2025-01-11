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

#include "rules/simulation/WorldRouter.h"

#include "server/EntityBuilder.h"

#include "rules/simulation/Domain.h"
#include "rules/simulation/LocatedEntity.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/id.h"
#include "rules/simulation/Inheritance.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/operations/Tick.h"
#include "common/Variable.h"

#include <Atlas/Objects/Anonymous.h>

#include <cstdio>
#include <cstdlib>

#include <cassert>
#include "rules/Script_impl.h"

using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Tick;

static bool stub_deny_newid = false;
Atlas::Objects::Factories factories;

class WorldRoutertest : public Cyphesis::TestBase {
	WorldRouter* test_world;
	EntityBuilder* m_eb;
public:
	WorldRoutertest();

	void setup();

	void teardown();

	void test_constructor();

	void test_addNewEntity_unknown();

	void test_addNewEntity_thing();

	void test_addNewEntity_idfail();

	void test_addEntity();

	void test_addEntity_tick();

	void test_addEntity_tick_get();

	void test_delEntity();

	void test_delEntity_world();

	Inheritance* m_inheritance;
	Ref<LocatedEntity> m_rootEntity;
};

WorldRoutertest::WorldRoutertest() {
	ADD_TEST(WorldRoutertest::test_addNewEntity_thing);
	ADD_TEST(WorldRoutertest::test_constructor);
	ADD_TEST(WorldRoutertest::test_addNewEntity_unknown);
	ADD_TEST(WorldRoutertest::test_addEntity);
	ADD_TEST(WorldRoutertest::test_addEntity_tick);
	ADD_TEST(WorldRoutertest::test_addEntity_tick_get);
	ADD_TEST(WorldRoutertest::test_delEntity);
	ADD_TEST(WorldRoutertest::test_delEntity_world);
}

void WorldRoutertest::setup() {
	m_rootEntity = new LocatedEntity(0);
	m_inheritance = new Inheritance();
	m_eb = new EntityBuilder();
	test_world = new WorldRouter(m_rootEntity, *m_eb, []() { return std::chrono::milliseconds(0); });
}

void WorldRoutertest::teardown() {
	m_rootEntity = nullptr;
	delete m_eb;
	delete test_world;
	delete m_inheritance;
}

void WorldRoutertest::test_constructor() {
}

void WorldRoutertest::test_addNewEntity_unknown() {

	auto ent1 = test_world->addNewEntity("__no_such_type__",
										 Anonymous());
	assert(ent1.get() == 0);
}

void WorldRoutertest::test_addNewEntity_thing() {
	Anonymous ent;
	ent->setLoc("0");
	auto ent1 = test_world->addNewEntity("thing", ent);
	assert(ent1.get() != 0);
}

void WorldRoutertest::test_addEntity() {
	auto id = newId();

	auto ent2 = new LocatedEntity(id);
	ent2->incRef();
	assert(ent2 != 0);
	m_rootEntity->addChild(*ent2);
	test_world->addEntity(ent2, m_rootEntity);
}

void WorldRoutertest::test_addEntity_tick() {
	auto id = newId();

	auto ent2 = new LocatedEntity(id);
	assert(ent2 != 0);
	m_rootEntity->addChild(*ent2);
	test_world->addEntity(ent2, m_rootEntity);

	Tick tick;
	tick->setFutureMilliseconds(0);
	tick->setTo(ent2->getIdAsString());
	test_world->message(tick, *ent2);
}


void WorldRoutertest::test_addEntity_tick_get() {
	auto id = newId();

	auto ent2 = new LocatedEntity(id);
	assert(ent2 != 0);
	m_rootEntity->addChild(*ent2);
	test_world->addEntity(ent2, m_rootEntity);

	Tick tick;
	tick->setFutureMilliseconds(0);
	tick->setTo(ent2->getIdAsString());
	test_world->message(tick, *ent2);

}


void WorldRoutertest::test_delEntity() {
	auto id = newId();

	auto ent2 = new LocatedEntity(id);
	assert(ent2 != 0);
	m_rootEntity->addChild(*ent2);
	test_world->addEntity(ent2, m_rootEntity);

	test_world->delEntity(ent2);
	test_world->delEntity(m_rootEntity.get());
}

void WorldRoutertest::test_delEntity_world() {
	test_world->delEntity(m_rootEntity.get());
}

int main() {
	Monitors m;
	WorldRoutertest t;

	return t.run();
}

// Stubs

int timeoffset = 0;

namespace consts {
const char* rootWorldId = "0";
const long rootWorldIntId = 0L;
}

namespace Atlas {
namespace Objects {
namespace Operation {
int TICK_NO = -1;
}
}
}


#include "common/TypeNode_impl.h"

template
class OperationsDispatcher<LocatedEntity>;

template
struct OpQueEntry<LocatedEntity>;

// Deletions and reference count decrements are required to ensure map
// memory management works correctly.
LocatedEntity::~LocatedEntity() {
	if (m_parent) {
		m_parent = nullptr;
	}
}


void LocatedEntity::makeContainer() {
	if (!m_contains) {
		m_contains.reset(new LocatedEntitySet);
	}
}


long integerId(const std::string& id) {
	long intId = strtol(id.c_str(), 0, 10);
	if (intId == 0 && id != "0") {
		intId = -1L;
	}

	return intId;
}


static inline float sqr(float x) {
	return x * x;
}

#include "rules/Location_impl.h"

static long idGenerator = 2;

RouterId newId() {
	if (stub_deny_newid) {
		return -1;
	}
	long new_id = ++idGenerator;
	return {new_id};
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


const TypeNode<LocatedEntity>* Inheritance::getType(const std::string& typeName) const {
	auto I = atlasObjects.find(typeName);
	if (I == atlasObjects.end()) {
		return 0;
	}
	return I->second.get();
}


Ref<LocatedEntity> EntityBuilder::newEntity(RouterId id,
									 const std::string& type,
									 const RootEntity& attributes) const {
	if (type == "thing") {
		auto e = new LocatedEntity(id);
		return e;
	}
	return 0;
}


int EntityBuilder::installFactory(const std::string& class_name, const Root& class_desc, std::unique_ptr<EntityKit> factory) {
	return 0;
}

