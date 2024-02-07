// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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

#include "rules/simulation/CorePropertyManager.h"

#include "rules/simulation/Domain.h"
#include "rules/simulation/Entity.h"

#include "rules/simulation/Inheritance.h"
#include "common/PropertyFactory_impl.h"


#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

Atlas::Objects::Factories factories;

class MinimalProperty : public PropertyBase {
public:
	MinimalProperty() {}

	virtual int get(Atlas::Message::Element& val) const { return 0; }

	virtual void set(const Atlas::Message::Element& val) {}

	virtual MinimalProperty* copy() const { return 0; }
};

class CorePropertyManagertest : public Cyphesis::TestBase {
	CorePropertyManager* m_propertyManager;

public:
	CorePropertyManagertest();

	void setup();

	void teardown();

	void test_addProperty();

	void test_addProperty_named();

	void test_installFactory();

	Inheritance* m_inheritance;
};

CorePropertyManagertest::CorePropertyManagertest() {
	ADD_TEST(CorePropertyManagertest::test_addProperty);
	ADD_TEST(CorePropertyManagertest::test_addProperty_named);
	ADD_TEST(CorePropertyManagertest::test_installFactory);
}

void CorePropertyManagertest::setup() {
	m_inheritance = new Inheritance();
	m_propertyManager = new CorePropertyManager(*m_inheritance);
	m_propertyManager->installFactory(
			"named_type", {}, std::make_unique<PropertyFactory<MinimalProperty, LocatedEntity>>()
	);

}

void CorePropertyManagertest::teardown() {
	delete m_inheritance;
	delete m_propertyManager;
}

void CorePropertyManagertest::test_addProperty() {
	auto p = m_propertyManager->addProperty("non_existant_type");
	ASSERT_TRUE(p);
	ASSERT_NOT_NULL(dynamic_cast<SoftProperty<LocatedEntity>*>(p.get()));
}

void CorePropertyManagertest::test_addProperty_named() {
	auto p = m_propertyManager->addProperty("named_type");
	ASSERT_TRUE(p);
	ASSERT_NOT_NULL(dynamic_cast<MinimalProperty*>(p.get()));
}

void CorePropertyManagertest::test_installFactory() {
	int ret = m_propertyManager->installFactory(
			"new_named_type",
			Root(),
			std::make_unique<PropertyFactory<MinimalProperty, LocatedEntity>>()
	);

	ASSERT_EQUAL(ret, 0);
}

int main() {
	CorePropertyManagertest t;

	return t.run();
}


#include "common/globals.h"

#include "server/EntityFactory_impl.h"
#include "server/Juncture.h"

#include "rules/simulation/TasksProperty.h"

#include "rules/simulation/VisibilityProperty.h"
#include "rules/simulation/Thing.h"


template
class EntityFactory<Thing>;

template
class PropertyFactory<MinimalProperty, LocatedEntity>;

TypeNode<LocatedEntity>* Inheritance::addChild(const Root& obj) {
	auto result = atlasObjects.emplace(obj->getId(), std::make_unique<TypeNode<LocatedEntity>>(obj->getId()));
	return result.first->second.get();
}
#include "common/TypeNode_impl.h"