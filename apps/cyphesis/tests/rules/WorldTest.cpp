// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

#include "../IGEntityExerciser.h"
#include "../allOperations.h"
#include "../TestWorld.h"


#include "rules/simulation/AtlasProperties.h"
#include "rules/BBoxProperty_impl.h"
#include "rules/simulation/CalendarProperty.h"
#include "rules/simulation/Domain.h"
#include "rules/Script.h"
#include "rules/simulation/StatusProperty.h"
#include "rules/simulation/TerrainProperty.h"

#include "common/id.h"
#include "rules/simulation/Inheritance.h"
#include "common/log.h"
#include "common/Property_impl.h"
#include "common/TypeNode_impl.h"
#include "common/PropertyManager_impl.h"


#include "rules/Location_impl.h"
#include "rules/PhysicalProperties_impl.h"
#include "common/Monitors.h"


#include <cstdlib>

#include <cassert>
#include <rules/simulation/LocatedEntity.h>
#include <rules/simulation/WorldProperty.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

template
class Vector3Property<LocatedEntity>;

template
class VelocityProperty<LocatedEntity>;

template
class Point3Property<LocatedEntity>;

template
class OrientationProperty<LocatedEntity>;

template
class PositionProperty<LocatedEntity>;

int main() {
	Monitors m;
	Ref<LocatedEntity> e(new LocatedEntity(0));
	TypeNode<LocatedEntity> type("world");
	e->setType(&type);
	e->requirePropertyClassFixed<WorldProperty>();

	IGEntityExerciser ee(e);

	assert(e->m_parent == nullptr);

	// Throw an op of every type at the entity
	ee.runOperations();

	// Subscribe the entity to every class of op
	std::set<std::string> opNames;
	ee.addAllOperations(opNames);

	// Throw an op of every type at the entity again now it is subscribed
	ee.runOperations();

	return 0;
}

// stubs


void addToEntity(const Point3D& p, std::vector<double>& vd) {
	vd.resize(3);
	vd[0] = p[0];
	vd[1] = p[1];
	vd[2] = p[2];
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




