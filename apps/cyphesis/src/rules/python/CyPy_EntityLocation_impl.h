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

#include "CyPy_EntityLocation.h"
#include "rules/EntityLocation_impl.h"
#include "CyPy_Location_impl.h"
#include "CyPy_Point3D.h"
#include "CyPy_Vector3D.h"
#include "CyPy_Quaternion.h"
#include "CyPy_Axisbox.h"
#include "EntityHelper.h"

template<typename EntityT, typename PythonEntityT>
CyPy_EntityLocation<EntityT, PythonEntityT>::CyPy_EntityLocation(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
		: WrapperBase<EntityLocation<EntityT>, CyPy_EntityLocation<EntityT, PythonEntityT>>(self, args, kwds) {

	if (args.length() > 0) {

		this->m_value.m_parent = verifyObject<PythonEntityT>(args[0]);

		if (args.length() == 2) {
			this->m_value.m_pos = CyPy_Point3D::parse(args[1]);
		}
	}
}

template<typename EntityT, typename PythonEntityT>
CyPy_EntityLocation<EntityT, PythonEntityT>::CyPy_EntityLocation(Py::PythonClassInstance* self, EntityLocation<EntityT> value)
		: WrapperBase<EntityLocation<EntityT>, CyPy_EntityLocation<EntityT, PythonEntityT>>(self, std::move(value)) {

}

template<typename EntityT, typename PythonEntityT>
void CyPy_EntityLocation<EntityT, PythonEntityT>::init_type() {
	CyPy_EntityLocation<EntityT, PythonEntityT>::behaviors().name("EntityLocation");
	CyPy_EntityLocation<EntityT, PythonEntityT>::behaviors().doc("");

	CyPy_EntityLocation<EntityT, PythonEntityT>::template register_method<[](EntityLocation<EntityT>& value) -> Py::Object {
		return CyPy_EntityLocation<EntityT, PythonEntityT>::wrap(value);
	}>("copy");

	//behaviors().supportNumberType(Py::PythonType::support_number_subtract);

	CyPy_EntityLocation<EntityT, PythonEntityT>::behaviors().readyType();

}


template<typename EntityT, typename PythonEntityT>
Py::Object CyPy_EntityLocation<EntityT, PythonEntityT>::getattro(const Py::String& name) {
	auto nameStr = name.as_string();
	if ("parent" == nameStr || "entity" == nameStr) {
		if (!this->m_value.m_parent) {
			return Py::None();
		}
		return wrapEntity(this->m_value.m_parent);
	}
	if ("pos" == nameStr) {
		return CyPy_Point3D::wrap(this->m_value.m_pos);
	}
	return this->PythonExtensionBase::getattro(name);
}

template<typename EntityT, typename PythonEntityT>
int CyPy_EntityLocation<EntityT, PythonEntityT>::setattro(const Py::String& name, const Py::Object& attr) {

	auto nameStr = name.as_string();
	if ("parent" == nameStr || "entity" == nameStr) {
		if (!PythonEntityT::check(attr)) {
			throw Py::TypeError("parent must be an entity");
		}
		this->m_value.m_parent = verifyObject<PythonEntityT>(attr);

		return 0;
	}

	if ("pos" == nameStr) {
		this->m_value.m_pos = CyPy_Point3D::parse(attr);
		return 0;
	}
	throw Py::AttributeError("unknown attribute");
}

//Py::Object CyPy_EntityLocation::repr()
//{
//    std::stringstream r;
//    r << m_value;
//    return Py::String(r.str());
//}

//Py::Object CyPy_EntityLocation::number_subtract(const Py::Object& other)
//{
//    return CyPy_Vector3D::wrap(distanceTo(CyPy_EntityLocation::value(other), m_value));
//}
