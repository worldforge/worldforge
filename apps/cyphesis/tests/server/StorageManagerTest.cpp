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

#include "server/StorageManager.h"
#include "server/Persistence.h"

#include "rules/simulation/WorldRouter.h"

#include "rules/simulation/Entity.h"
#include "server/MindProperty.h"

#include "common/Property_impl.h"
#include "../DatabaseNull.h"
#include "../TestPropertyManager.h"

#include <cassert>
#include <server/EntityBuilder.h>
#include "common/Monitors.h"

using Atlas::Message::Element;

struct TestStorageManager : public StorageManager {
	TestStorageManager(WorldRouter& w, Database& db, EntityBuilder& eb, PropertyManager<LocatedEntity>& propertyManager) : StorageManager(w, db, eb, propertyManager) {}


	void test_entityInserted(LocatedEntity& e) {
		entityInserted(e);
	}

	void test_entityUpdated(LocatedEntity& e) {
		entityUpdated(e);
	}

	void test_encodeProperty(PropertyBase& p, std::string& s) {
		encodeProperty(p, s);
	}

	void test_restoreProperties(LocatedEntity& e) {
		restorePropertiesRecursively(e);
	}

	void test_insertEntity(LocatedEntity& e) {
		insertEntity(e);
	}

	void test_updateEntity(LocatedEntity& e) {
		updateEntity(e);
	}

	void test_restoreChildren(LocatedEntity& e) {
		restoreChildren(e);
	}


};

int main() {
	Monitors m;
	EntityBuilder eb;
	DatabaseNull database;
	Persistence persistence(database);
	TestPropertyManager<LocatedEntity> propertyManager;

	Ref<LocatedEntity> le(new Entity(0));

	{
		WorldRouter world(le, eb, {});

		StorageManager store(world, database, eb, propertyManager);
	}

	{
		WorldRouter world(le, eb, {});

		StorageManager store(world, database, eb, propertyManager);

		store.initWorld(le);
	}

	{
		WorldRouter world(le, eb, {});

		StorageManager store(world, database, eb, propertyManager);

		store.restoreWorld(le);
	}

	{
		WorldRouter world(le, eb, {});

		StorageManager store(world, database, eb, propertyManager);

		store.tick();
	}

	{
		WorldRouter world(le, eb, {});

		TestStorageManager store(world, database, eb, propertyManager);
		Ref<Entity> e1(new Entity(1));
		store.test_entityInserted(*e1);
	}

	{
		WorldRouter world(le, eb, {});

		TestStorageManager store(world, database, eb, propertyManager);
		Ref<Entity> e1(new Entity(1));
		store.test_entityUpdated(*e1);
	}

	{
		WorldRouter world(le, eb, {});

		TestStorageManager store(world, database, eb, propertyManager);

		std::string val;

		// store.test_encodeProperty(0, val);
	}

	{
		WorldRouter world(le, eb, {});

		TestStorageManager store(world, database, eb, propertyManager);

		Ref<Entity> e1(new Entity(1));
		store.test_restoreProperties(*e1);
	}

	{
		WorldRouter world(le, eb, {});

		TestStorageManager store(world, database, eb, propertyManager);

		Ref<Entity> e1(new Entity(1));
		store.test_insertEntity(*e1);
	}

	{
		WorldRouter world(le, eb, {});

		TestStorageManager store(world, database, eb, propertyManager);

		Ref<Entity> e1(new Entity(1));
		store.test_updateEntity(*e1);
	}

	{
		WorldRouter world(le, eb, {});

		TestStorageManager store(world, database, eb, propertyManager);

		Ref<Entity> e1(new Entity(1));
		store.test_restoreChildren(*e1);
	}


	return 0;
}

// stubs

#include "rules/Script.h"

#include "modules/WeakEntityRef.h"
#include "rules/Location_impl.h"

#include "common/Database.h"
#include "common/globals.h"
#include "common/PropertyManager.h"
#include "common/Variable.h"

#include <cstdlib>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;


DatabaseResult Database::selectEntities(const std::string& loc) {
	return DatabaseResult(std::make_unique<DatabaseNullResultWorker>());
}

DatabaseResult Database::selectProperties(const std::string& loc) {
	return DatabaseResult(std::make_unique<DatabaseNullResultWorker>());
}

DatabaseResult Database::selectThoughts(const std::string& loc) {
	return DatabaseResult(std::make_unique<DatabaseNullResultWorker>());
}

template<typename T>
Variable<T>::Variable(const T& variable) : m_variable(variable) {
}

template<typename T>
Variable<T>::~Variable() {
}

template<typename T>
void Variable<T>::send(std::ostream& o) {
	o << m_variable;
}

template<typename T>
bool Variable<T>::isNumeric() const {
	return false;
}

template
class Variable<int>;

template
class Variable<const char*>;

template
class Variable<std::string>;

long forceIntegerId(const std::string& id) {
	long intId = strtol(id.c_str(), 0, 10);
	if (intId == 0 && id != "0") {
		abort();
	}

	return intId;
}


bool database_flag = true;

namespace consts {

// Id of root world entity
const char* rootWorldId = "0";
// Integer id of root world entity
const long rootWorldIntId = 0L;

}


int EntityBuilder::installFactory(const std::string& class_name, const Root& class_desc, std::unique_ptr<EntityKit> factory) {
	return 0;
}

long Database::entitiesCount() const {
	return 0;
}

int Database::insertEntity(const std::string& id,
						   const std::string& loc,
						   const std::string& type,
						   int seq,
						   const std::string& value) {
	return 0;
}

int Database::updateEntity(const std::string& id,
						   int seq,
						   const std::string& location_data,
						   const std::string& location_entity_id) {
	return 0;

}

int Database::updateEntityWithoutLoc(const std::string& id,
									 int seq,
									 const std::string& location_data) {
	return 0;

}
