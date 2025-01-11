// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#include "server/EntityKit.h"

#include "common/ScriptKit.h"
#include "common/TypeNode_impl.h"
#include "rules/simulation/LocatedEntity.h"
#include "../TestPropertyManager.h"
#include "rules/simulation/python/CyPy_LocatedEntity_impl.h"
#include <Atlas/Message/Element.h>

#include <cassert>

using Atlas::Message::MapType;

class TestEntityKit : public EntityKit {
public:
	virtual ~TestEntityKit() {}

	Ref<LocatedEntity> newEntity(RouterId id,
						  const Atlas::Objects::Entity::RootEntity& attributes) override { return nullptr; }

	static EntityKit* duplicateFactory() { return nullptr; }

	void addProperties(const PropertyManager<LocatedEntity>&) override {}

	void updateProperties(std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate>& changes, const PropertyManager<LocatedEntity>&) override {
	}
};

class TestScriptKit : public ScriptKit<LocatedEntity, CyPy_LocatedEntity> {
public:
	std::string m_package;

	virtual const std::string& package() const { return m_package; }

	virtual int addScript(LocatedEntity* entity) const { return 0; }

	virtual int refreshClass() { return 0; }
};

class EntityKittest : public Cyphesis::TestBase {
private:
	EntityKit* m_ek;
public:
	EntityKittest();

	void setup();

	void teardown();

	void test_addProperties();

	void test_updateProperties();
};

EntityKittest::EntityKittest() {
	ADD_TEST(EntityKittest::test_addProperties);
	ADD_TEST(EntityKittest::test_updateProperties);
}

void EntityKittest::setup() {
	m_ek = new TestEntityKit;
	m_ek->m_type = new TypeNode<LocatedEntity>("foo");
}

void EntityKittest::teardown() {
	delete m_ek->m_type;
	delete m_ek;
}

void EntityKittest::test_addProperties() {
	TestPropertyManager<LocatedEntity> propertyManager{};
	m_ek->addProperties(propertyManager);
}

void EntityKittest::test_updateProperties() {
	TestPropertyManager<LocatedEntity> propertyManager{};
	std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate> changes;
	m_ek->updateProperties(changes, propertyManager);
}

int main() {
	EntityKittest t;

	return t.run();
}

// stubs

