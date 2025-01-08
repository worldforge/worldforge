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
#include "server/EntityRuleHandler.h"
#include "server/EntityFactory_impl.h"

#include "rules/simulation/AtlasProperties.h"
#include "rules/simulation/Domain.h"
#include "rules/simulation/Thing.h"

#include "common/globals.h"
#include "common/id.h"
#include "rules/simulation/Inheritance.h"

#include "common/operations/Tick.h"
#include "common/Variable.h"

#include <Atlas/Objects/Anonymous.h>


#include <cassert>
#include "rules/PhysicalProperties.h"

#include "../DatabaseNull.h"
#include "../TestPropertyManager.h"
#include "common/Monitors.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Tick;

Atlas::Objects::Factories factories;

auto timeProviderFn = [] { return std::chrono::steady_clock::now().time_since_epoch(); };

struct WorldRouterintegration : public Cyphesis::TestBase {

	Inheritance* m_inheritance;
	EntityBuilder* m_eb;
	DatabaseNull m_database;

	void setup() override;

	void teardown() override;

	void test_sequence();

	void test_creationAndDeletion() {
		{
			Ref<LocatedEntity> base = new Entity(0);
			WorldRouter test_world(base, *m_eb, timeProviderFn);

			Anonymous ent;
			ent->setLoc("0");
			auto ent1 = test_world.addNewEntity("thing", ent);

			ASSERT_EQUAL(2, test_world.m_entityCount);

			test_world.delEntity(ent1.get());
			//A single entity when removed should have all references removed too.
			ASSERT_EQUAL(1, test_world.m_entityCount);
			ASSERT_EQUAL(1, ent1->checkRef());
			test_world.shutdown();
		}

		{
			Ref<LocatedEntity> base = new Entity(0);
			WorldRouter test_world(base, *m_eb, timeProviderFn);

			Anonymous ent;
			ent->setLoc("0");
			auto ent1 = test_world.addNewEntity("thing", ent);

			ASSERT_EQUAL(2, test_world.m_entityCount);

			Anonymous ent2_arg{};
			ent2_arg->setLoc(ent1->getIdAsString());
			auto ent2 = test_world.addNewEntity("thing", ent2_arg);

			ASSERT_EQUAL(3, test_world.m_entityCount);
			//Make sure ent2 is a child of ent1.
			ASSERT_EQUAL(ent2->m_parent, ent1.get());

			//Make sure that a child when removed has all references removed too.
			test_world.delEntity(ent2.get());
			ASSERT_EQUAL(2, test_world.m_entityCount);
			ASSERT_EQUAL(1, ent2->checkRef());
			test_world.shutdown();

		}
	}


	WorldRouterintegration() {
		ADD_TEST(WorldRouterintegration::test_sequence);
		ADD_TEST(WorldRouterintegration::test_creationAndDeletion);

	}

};


void WorldRouterintegration::setup() {
	m_inheritance = new Inheritance();
	m_eb = new EntityBuilder();
	TestPropertyManager<LocatedEntity> propertyManager;

	class TestEntityRuleHandler : public EntityRuleHandler {
	public:
		explicit TestEntityRuleHandler(EntityBuilder& eb, const PropertyManager<LocatedEntity>& propertyManager) : EntityRuleHandler(eb, propertyManager) {}

		int test_installEntityClass(const std::string& class_name,
									const std::string& parent,
									const Atlas::Objects::Root& class_desc,
									std::string& dependent,
									std::string& reason,
									std::unique_ptr<EntityFactoryBase> factory) {
			std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate> changes;
			return installEntityClass(class_name, parent, class_desc, dependent, reason, std::move(factory), changes);
		}
	};

	TestEntityRuleHandler entityRuleHandler(*m_eb, propertyManager);

	auto composeDeclaration = [](std::string class_name, std::string parent, Atlas::Message::MapType rawAttributes) {

		Atlas::Objects::Root decl;
		decl->setObjtype("class");
		decl->setId(class_name);
		decl->setParent(parent);

		Atlas::Message::MapType composed;
		for (const auto& entry: rawAttributes) {
			composed[entry.first] = Atlas::Message::MapType{
					{"default", entry.second}
			};
		}

		decl->setAttr("attributes", composed);
		return decl;
	};
	std::string dependent, reason;
	{
		auto decl = composeDeclaration("thing", "game_entity", {});
		entityRuleHandler.test_installEntityClass(decl->getId(), decl->getParent(), decl, dependent, reason, std::make_unique<EntityFactory<Thing>>());
	}
	{
		auto decl = composeDeclaration("character", "thing", {});
		entityRuleHandler.test_installEntityClass(decl->getId(), decl->getParent(), decl, dependent, reason, std::make_unique<EntityFactory<Thing>>());
	}
}

void WorldRouterintegration::teardown() {
	delete m_eb;
	delete m_inheritance;
}

void WorldRouterintegration::test_sequence() {
	Ref<Entity> base = new Entity(0);
	WorldRouter test_world(base, *m_eb, timeProviderFn);

	auto ent1 = test_world.addNewEntity("__no_such_type__",
										Anonymous());
	assert(!ent1);

	Anonymous ent;
	ent->setLoc("0");
	ent1 = test_world.addNewEntity("thing", ent);
	assert(ent1);

	auto id = newId();

	Ref<Entity> ent2 = new Thing(id);
	assert(ent2 != 0);
	ent2->requirePropertyClassFixed<PositionProperty<LocatedEntity>>().data() = Point3D(0, 0, 0);
	test_world.addEntity(ent2, base);

	Tick tick;
	tick->setFutureMilliseconds(0);
	tick->setTo(ent2->getIdAsString());
	test_world.message(tick, *ent2);


	test_world.delEntity(base.get());
//    test_world.delEntity(ent4);
//    ent4 = 0;

	test_world.shutdown();
}

int main() {
	Monitors m;
	WorldRouterintegration t;

	t.m_database.idGeneratorFn = []() {
		static long id = 0;
		return ++id;
	};


	return t.run();
}

// stubs

#include "server/EntityFactory_impl.h"
#include "server/ArchetypeFactory.h"
#include "rules/simulation/CorePropertyManager.h"

#include "rules/simulation/AreaProperty.h"
#include "rules/simulation/AtlasProperties.h"
#include "rules/BBoxProperty_impl.h"
#include "rules/simulation/CalendarProperty.h"
#include "rules/simulation/EntityProperty.h"
#include "rules/simulation/StatusProperty.h"
#include "rules/simulation/TasksProperty.h"
#include "rules/simulation/TerrainProperty.h"
#include "rules/simulation/DomainProperty.h"

#include "rules/simulation/ExternalMind.h"
#include "rules/simulation/Task.h"

#include "rules/python/PythonScriptFactory_impl.h"
#include "rules/simulation/python/CyPy_LocatedEntity_impl.h"

#include "rules/BBoxProperty_impl.h"
#include "rules/Location_impl.h"


#include <Atlas/Objects/Operation.h>


sigc::signal<void()> python_reload_scripts;


