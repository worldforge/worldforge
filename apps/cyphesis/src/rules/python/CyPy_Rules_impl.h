/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#pragma once

#include "CyPy_Rules.h"
#include "CyPy_Props_impl.h"
#include "CyPy_Location_impl.h"
#include "CyPy_EntityLocation_impl.h"
#include "CyPy_RootEntity.h"
#include "rules/PhysicalProperties_impl.h"
#include <wfmath/atlasconv.h>

template<typename EntityT, typename PythonEntityT>
CyPy_Rules<EntityT, PythonEntityT>::CyPy_Rules() : Py::ExtensionModule<CyPy_Rules<EntityT, PythonEntityT>>("rules") {
	CyPy_Props<EntityT>::init_type();
	CyPy_Location<EntityT, PythonEntityT>::init_type();
	CyPy_EntityLocation<EntityT, PythonEntityT>::init_type();

	this->add_varargs_method("isLocation", &CyPy_Rules<EntityT, PythonEntityT>::is_location, "");
	this->add_varargs_method("extract_location", &CyPy_Rules<EntityT, PythonEntityT>::extract_location, "Extracts all location data from the entity into the root entity message.");

	this->initialize("Rules");

	Py::Dict d(this->moduleDictionary());

	d["Location"] = CyPy_Location<EntityT, PythonEntityT>::type();
	d["EntityLocation"] = CyPy_EntityLocation<EntityT, PythonEntityT>::type();

}

template<typename EntityT, typename PythonEntityT>
Py::Object CyPy_Rules<EntityT, PythonEntityT>::is_location(const Py::Tuple& args) {
	args.verify_length(1, 1);
	return Py::Boolean(CyPy_Location<EntityT, PythonEntityT>::check(args[0]));
}

template<typename EntityT, typename PythonEntityT>
Py::Object CyPy_Rules<EntityT, PythonEntityT>::extract_location(const Py::Tuple& args) {
	args.verify_length(2);
	auto& entity = verifyObject<PythonEntityT>(args[0]);
	auto& rootEntity = verifyObject<CyPy_RootEntity>(args[1]);

	if (entity->m_parent != nullptr) {
		rootEntity->setLoc(entity->m_parent->getIdAsString());
	}
	if (auto prop = entity->template getPropertyClassFixed<PositionProperty<EntityT>>()) {
		::addToEntity(prop->data(), rootEntity->modifyPos());
	}
	if (auto prop = entity->template getPropertyClassFixed<VelocityProperty<EntityT>>()) {
		::addToEntity(prop->data(), rootEntity->modifyVelocity());
	}
	if (auto prop = entity->template getPropertyClassFixed<OrientationProperty<EntityT>>()) {
		rootEntity->setAttr("orientation", prop->data().toAtlas());
	}
	if (auto prop = entity->template getPropertyClassFixed<AngularVelocityProperty<EntityT>>()) {
		rootEntity->setAttr("angular", prop->data().toAtlas());
	}

	return args[1];
}

template<typename EntityT, typename PythonEntityT>
std::string CyPy_Rules<EntityT, PythonEntityT>::init() {
	PyImport_AppendInittab("rules", []() {
		static CyPy_Rules<EntityT, PythonEntityT> module;
		return module.module().ptr();
	});
	return "rules";
}
