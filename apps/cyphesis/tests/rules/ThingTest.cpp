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

#include "rules/simulation/LocatedEntity.h"

#include "rules/simulation/Domain.h"

#include "common/const.h"
#include "common/id.h"
#include "common/log.h"
#include "common/Property_impl.h"
#include "common/TypeNode_impl.h"
#include "common/PropertyManager_impl.h"


#include "common/TypeNode_impl.h"
#include "rules/Location_impl.h"

#include "../TestDomain.h"
#include "common/Monitors.h"

#include <cstdlib>

#include <cassert>

Monitors monitors;

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

int main() {
	Ref<LocatedEntity> e(new LocatedEntity(1));
	TypeNode<LocatedEntity> type("thing");
	e->setType(&type);

	IGEntityExerciser ee(e);

	// Throw an op of every type at the entity
	ee.runOperations();

	// Subscribe the entity to every class of op
	std::set<std::string> opNames;
	ee.addAllOperations(opNames);

	// Throw an op of every type at the entity again now it is subscribed
	ee.runOperations();

	return 0;
}

//stubs



bool LocatedEntity::isVisibleForOtherEntity(const LocatedEntity& watcher) const {
	return true;
}


void LocatedEntity::makeContainer() {
	if (m_contains == 0) {
		m_contains.reset(new LocatedEntitySet);
	}
}


void LocatedEntity::changeContainer(const Ref<LocatedEntity>& new_loc) {
	assert(m_parent != nullptr);
	assert(m_parent->m_contains != nullptr);
	m_parent->m_contains->erase(this);
	if (m_parent->m_contains->empty()) {
		m_parent->onUpdated();
	}
	new_loc->makeContainer();
	bool was_empty = new_loc->m_contains->empty();
	new_loc->m_contains->insert(this);
	if (was_empty) {
		new_loc->onUpdated();
	}
	assert(m_parent->checkRef() > 0);
	auto oldLoc = m_parent;
	m_parent = new_loc.get();
	assert(m_parent->checkRef() > 0);

	onContainered(oldLoc);
}

void LocatedEntity::broadcast(const Atlas::Objects::Operation::RootOperation& op, OpVector& res, Visibility visibility) const {
	auto copy = op.copy();
	copy->setTo(getIdAsString());
	res.push_back(copy);
}

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


template<typename FloatT>
int fromStdVector(Point3D& p, const std::vector<FloatT>& vf) {
	if (vf.size() != 3) {
		return -1;
	}
	p[0] = vf[0];
	p[1] = vf[1];
	p[2] = vf[2];
	p.setValid();
	return 0;
}

template<typename FloatT>
int fromStdVector(Vector3D& v, const std::vector<FloatT>& vf) {
	if (vf.size() != 3) {
		return -1;
	}
	v[0] = vf[0];
	v[1] = vf[1];
	v[2] = vf[2];
	v.setValid();
	return 0;
}

template int fromStdVector<double>(Point3D& p, const std::vector<double>& vf);

template int fromStdVector<double>(Vector3D& v, const std::vector<double>& vf);


WFMath::CoordType squareDistance(const Point3D& u, const Point3D& v) {
	return 1.0;
}

#include "rules/PhysicalProperties_impl.h"

