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

#include "rules/simulation/LocatedEntity.h"

#include "server/EntityRuleHandler.h"

#include "server/EntityBuilder.h"

#include "common/TypeNode_impl.h"
#include "../TestPropertyManager.h"

#include <Atlas/Objects/Anonymous.h>

#include <cstdlib>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;

static TypeNode<LocatedEntity>* stub_addChild_result = nullptr;

int main() {
	EntityBuilder eb;
	TestPropertyManager<LocatedEntity> propertyManager;

	{
		RuleHandler* rh = new EntityRuleHandler(eb, propertyManager);
		delete rh;
	}

	// check() not a class
	{
		RuleHandler* rh = new EntityRuleHandler(eb, propertyManager);

		Anonymous description;
		description->setParent("foo");
		int ret = rh->check(description);

		assert(ret == -1);

		delete rh;
	}

	// check() stub says it's not a task
	{
		RuleHandler* rh = new EntityRuleHandler(eb, propertyManager);

		Anonymous description;
		description->setObjtype("class");
		description->setParent("foo");
		int ret = rh->check(description);

		assert(ret == 0);

		delete rh;
	}

	{
		RuleHandler* rh = new EntityRuleHandler(eb, propertyManager);
		std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate> changes;

		Anonymous description;
		description->setId("class_name");
		std::string dependent, reason;

		int ret = rh->install("class_name", "parent_name",
							  description, dependent, reason, changes);

		assert(ret == 1);
		assert(dependent == "parent_name");

		delete rh;
	}

	// Install a rule with addChild rigged to give a correct result
	{
		RuleHandler* rh = new EntityRuleHandler(eb, propertyManager);
		std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate> changes;

		Anonymous description;
		description->setId("class_name");
		std::string dependent, reason;

		stub_addChild_result = (TypeNode<LocatedEntity>*) malloc(sizeof(TypeNode<LocatedEntity>));
		int ret = rh->install("class_name", "parent_name",
							  description, dependent, reason, changes);

		assert(ret == 1);
		assert(dependent == "parent_name");

		free(stub_addChild_result);
		stub_addChild_result = 0;

		delete rh;
	}
	{
		RuleHandler* rh = new EntityRuleHandler(eb, propertyManager);
		std::map<const TypeNode<LocatedEntity>*, TypeNode<LocatedEntity>::PropertiesUpdate> changes;

		Anonymous description;
		int ret = rh->update("", description, changes);

		// FIXME Currently does nothing
		assert(ret == -1);

		delete rh;
	}


}

// stubs

#include "server/EntityFactory_impl.h"
#include "server/Player.h"

#include "rules/python/PythonScriptFactory.h"

#include "rules/simulation/Inheritance.h"
#include "common/log.h"
#include "rules/simulation/python/CyPy_LocatedEntity.h"


template<typename EntityT, typename ScriptObjectT>
PythonScriptFactory<EntityT, ScriptObjectT>::PythonScriptFactory(const std::string& package,
																 const std::string& type) :
		PythonClass(package,
					type) {
}

template<typename EntityT, typename ScriptObjectT>
int PythonScriptFactory<EntityT, ScriptObjectT>::setup() {
	return 0;
}

template<typename EntityT, typename ScriptObjectT>
const std::string& PythonScriptFactory<EntityT, ScriptObjectT>::package() const {
	return m_package;
}

template
class PythonScriptFactory<LocatedEntity, CyPy_LocatedEntity>;

template<typename EntityT, typename ScriptObjectT>
int PythonScriptFactory<EntityT, ScriptObjectT>::refreshClass() {
	return 0;
}

template<typename EntityT, typename ScriptObjectT>
std::unique_ptr<Script<EntityT>> PythonScriptFactory<EntityT, ScriptObjectT>::createScriptWrapper(ScriptObjectT& entity) const {
	return {};
}

TypeNode<LocatedEntity>* Inheritance::addChild(const Atlas::Objects::Root& obj) {
	return stub_addChild_result;
}

bool Inheritance::hasClass(const std::string& parent) {
	return true;
}

Root atlasOpDefinition(const std::string& name, const std::string& parent) {
	return Root();
}

int EntityBuilder::installFactory(const std::string& class_name, const Atlas::Objects::Root& class_desc, std::unique_ptr<EntityKit> factory) {
	return 0;
}

#include "common/TypeNode_impl.h"

#include "common/PropertyManager_impl.h"

#include "common/Property_impl.h"
#include "common/PropertyManager_impl.h"

