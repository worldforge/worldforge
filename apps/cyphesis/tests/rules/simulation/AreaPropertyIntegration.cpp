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

#include "../../TestBase.h"

#include "rules/simulation/AreaProperty.h"
#include "rules/simulation/LocatedEntity.h"

#include "common/TypeNode_impl.h"
#include "common/Property_impl.h"
#include "common/PropertyManager_impl.h"
#include "common/Monitors.h"

using Atlas::Message::ListType;
using Atlas::Message::MapType;

// Check what happens when two instance of a type both instantiate
// this property when there is a script. The underlying instance of
// Shape needs to be copied 
class AreaPropertyintegration : public Cyphesis::TestBase {
private:
	TypeNode<LocatedEntity>* m_char_type;
	PropertyBase* m_char_property;
	Ref<LocatedEntity> m_char1;
	Ref<LocatedEntity> m_char2;
public:
	AreaPropertyintegration();

	void setup();

	void teardown();

	void test_copy();
};

AreaPropertyintegration::AreaPropertyintegration() {
	ADD_TEST(AreaPropertyintegration::test_copy);
}

void AreaPropertyintegration::setup() {
	m_char_type = new TypeNode<LocatedEntity>("char_type");

	m_char_property = new AreaProperty;
	m_char_property->addFlags(prop_flag_class);
	m_char_property->set(
			MapType{
					std::make_pair("shape", MapType{
							std::make_pair("type", "polygon"),
							std::make_pair("points", ListType(3, ListType(2, 1.f)))
					})
			}
	);
	m_char_type->injectProperty("char_type", std::unique_ptr<PropertyBase>(m_char_property));

	m_char1 = new LocatedEntity(1);
	m_char1->setType(m_char_type);
	m_char_property->install(*m_char1, "char_prop");
	m_char_property->apply(*m_char1);
	m_char1->propertyApplied("char_prop", *m_char_property);

	m_char2 = new LocatedEntity(2);
	m_char2->setType(m_char_type);
	m_char_property->install(*m_char2, "char_prop");
	m_char_property->apply(*m_char2);
	m_char2->propertyApplied("char_prop", *m_char_property);
}

void AreaPropertyintegration::teardown() {
	m_char1 = nullptr;
	m_char2 = nullptr;
	delete m_char_type;
}

void AreaPropertyintegration::test_copy() {
	AreaProperty* pb =
			m_char1->modPropertyClass<AreaProperty>("char_type");

	ASSERT_NOT_EQUAL(pb, m_char_property);
}

int main() {
	Monitors m;
	AreaPropertyintegration t;

	return t.run();
}

// stubs

#include "common/log.h"

const TerrainProperty* TerrainEffectorProperty::getTerrain(const LocatedEntity& owner, const LocatedEntity**) {
	return 0;
}

// stubs

#include "rules/simulation/AtlasProperties.h"
#include "rules/simulation/Domain.h"


#include "common/TypeNode_impl.h"


#include "rules/Script_impl.h"
#include "rules/Location_impl.h"
#include "rules/PhysicalProperties_impl.h"

