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

#include "rules/simulation/LocatedEntity.h"
#include "rules/simulation/TerrainModProperty.h"
#include "rules/simulation/TerrainProperty.h"

#include "common/OperationRouter.h"
#include "common/PropertyFactory_impl.h"
#include "common/PropertyManager_impl.h"
#include "rules/simulation/BaseWorld.h"

#include "rules/Location_impl.h"

#include <Atlas/Objects/Operation.h>
#include "rules/PhysicalProperties_impl.h"
#include "rules/python/PythonWrapper_impl.h"
#include "rules/python/PythonScriptFactory_impl.h"

using Atlas::Message::MapType;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Move;

#include "../TestWorld.h"

class TerrainModPropertyintegration : public Cyphesis::TestBase {
private:
	Ref<LocatedEntity> m_rootEntity;
	Ref<LocatedEntity> m_entity;
	std::unique_ptr<TestWorld> m_world;
public:
	TerrainModPropertyintegration();

	void setup();

	void teardown();

	void test_move_handler();

	void test_delete_handler();
};

TerrainModPropertyintegration::TerrainModPropertyintegration() {
	ADD_TEST(TerrainModPropertyintegration::test_move_handler);
	ADD_TEST(TerrainModPropertyintegration::test_delete_handler);
}

void TerrainModPropertyintegration::setup() {
	m_rootEntity = new LocatedEntity(0);

	m_world.reset();
	m_world.reset(new TestWorld(m_rootEntity));

	m_entity = new LocatedEntity(1);
	m_entity->requirePropertyClassFixed<PositionProperty<LocatedEntity>>().data() = Point3D(5.f, 5.f, 5.f);
	m_entity->m_parent = m_rootEntity.get();

	PropertyFactory<TerrainModProperty, LocatedEntity> terrainmod_property_factory;

	auto terrainProperty = new TerrainProperty;
	terrainProperty->install(*m_rootEntity, "terrain");
	auto tProp = m_rootEntity->setProperty("terrain", std::unique_ptr<PropertyBase>(terrainProperty));
	tProp->apply(*m_rootEntity);
	m_rootEntity->propertyApplied("terrain", *tProp);

	auto terrainModProp = terrainmod_property_factory.newProperty();
	terrainModProp->install(*m_entity, "terrainmod");
	auto prop = m_entity->setProperty("terrainmod", std::move(terrainModProp));
	prop->apply(*m_entity);
	m_entity->propertyApplied("terrainmod", *prop);
}

void TerrainModPropertyintegration::teardown() {

}

void TerrainModPropertyintegration::test_move_handler() {
	Move m;
	// FIXME Move needs some args, and also probably requires the position to
	// to be set up on the entity first

	OpVector res;
	m_entity->operation(m, res);

	// FIXME Check what gives
}

void TerrainModPropertyintegration::test_delete_handler() {
	Delete d;

	OpVector res;
	m_entity->operation(d, res);

	// FIXME Check what gives
}

int main() {
	TerrainModPropertyintegration t;

	return t.run();
}
