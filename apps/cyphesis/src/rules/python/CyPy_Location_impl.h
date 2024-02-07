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

#include "CyPy_Location.h"
#include "CyPy_Point3D.h"
#include "CyPy_Vector3D.h"
#include "CyPy_Quaternion.h"
#include "CyPy_Axisbox.h"
#include "CyPy_EntityLocation_impl.h"
#include "CyPy_RootEntity.h"
#include "CyPy_Element.h"

template<typename EntityT, typename PythonEntityT>
CyPy_Location<EntityT, PythonEntityT>::CyPy_Location(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
		: WrapperBase<Location<EntityT>, CyPy_Location<EntityT, PythonEntityT>>(self, args, kwds) {

	if (args.length() > 0) {

		//If there's only one argument it could be either an EntityLocation or an EntityT.
		//If it's a EntityT there could be an additional position too.
		if (args.length() == 1 && CyPy_EntityLocation<EntityT, PythonEntityT>::check(args[0])) {
			auto entityLocation = CyPy_EntityLocation<EntityT, PythonEntityT>::value(args[0]);
			this->m_value.m_parent = entityLocation.m_parent;
			this->m_value.m_pos = entityLocation.m_pos;
		} else {
			this->m_value.m_parent = verifyObject<PythonEntityT>(args[0]);
			if (args.length() == 2) {
				this->m_value.m_pos = CyPy_Point3D::parse(args[1]);
			}
		}
	}
}

template<typename EntityT, typename PythonEntityT>
CyPy_Location<EntityT, PythonEntityT>::CyPy_Location(Py::PythonClassInstance* self, Location<EntityT> value)
		: WrapperBase<Location<EntityT>, CyPy_Location<EntityT, PythonEntityT>>(self, std::move(value)) {

}

template<typename EntityT, typename PythonEntityT>
void CyPy_Location<EntityT, PythonEntityT>::init_type() {
	CyPy_Location<EntityT, PythonEntityT>::behaviors().name("Location");
	CyPy_Location<EntityT, PythonEntityT>::behaviors().doc("");

	CyPy_Location<EntityT, PythonEntityT>::template register_method<[](Location<EntityT>& value) -> Py::Object {
		return CyPy_Location<EntityT, PythonEntityT>::wrap(value);
	}>("copy");
	CyPy_Location<EntityT, PythonEntityT>::template register_method<[](Location<EntityT>& value, const Py::Tuple& args) -> Py::Object {
		args.verify_length(1);
		auto& rootEntity = verifyObject<CyPy_RootEntity>(args.front());
		value.addToEntity(rootEntity);
		return args.front();
	}>("add_to_entity");

	CyPy_Location<EntityT, PythonEntityT>::behaviors().supportNumberType(Py::PythonType::support_number_subtract);
	CyPy_Location<EntityT, PythonEntityT>::behaviors().supportRepr();
	CyPy_Location<EntityT, PythonEntityT>::behaviors().readyType();

	CyPy_Element::converters.emplace_back([](const Py::Object& o) -> std::optional<Atlas::Message::Element> {
		if (CyPy_Location<EntityT, PythonEntityT>::check(o)) {
			MapType map;
			CyPy_Location<EntityT, PythonEntityT>::value(o).addToMessage(map);
			return map;
		} else {
			return {};
		}
	});

	CyPy_RootEntity::importers.emplace_back([](Atlas::Objects::Entity::RootEntity& entity, const Py::Object& o) -> bool {
		if (CyPy_Location<EntityT, PythonEntityT>::check(o)) {
			CyPy_Location<EntityT, PythonEntityT>::value(o).addToEntity(entity);
			return true;
		} else {
			return false;
		}
	});

}

template<typename EntityT, typename PythonEntityT>
Py::Object CyPy_Location<EntityT, PythonEntityT>::getattro(const Py::String& name) {
	auto nameStr = name.as_string();
	if ("parent" == nameStr) {
		if (!this->m_value.m_parent) {
			return Py::None();
		}
		return wrapEntity(this->m_value.m_parent);
	}
	if ("pos" == nameStr) {
		return CyPy_Point3D::wrap(this->m_value.m_pos);
	}
	if ("velocity" == nameStr) {
		return CyPy_Vector3D::wrap(this->m_value.m_velocity);
	}
	if ("orientation" == nameStr) {
		return CyPy_Quaternion::wrap(this->m_value.orientation());
	}
	if ("bbox" == nameStr) {
		return CyPy_Axisbox::wrap(this->m_value.bBox());
	}
	if ("radius" == nameStr) {
		return Py::Float(this->m_value.radius());
	}
	return this->PythonExtensionBase::getattro(name);
}

template<typename EntityT, typename PythonEntityT>
int CyPy_Location<EntityT, PythonEntityT>::setattro(const Py::String& name, const Py::Object& attr) {

	auto nameStr = name.as_string();
	if (nameStr == "parent") {
		if (!PythonEntityT::check(attr)) {
			throw Py::TypeError("parent must be an entity");
		}
		this->m_value.m_parent = verifyObject<PythonEntityT>(attr);

		return 0;
	}
	if (nameStr == "bbox" && CyPy_Axisbox::check(attr)) {
		this->m_value.setBBox(Py::PythonClassObject<CyPy_Axisbox>::getCxxObject(attr)->m_value);
		return 0;
	}
	if (nameStr == "orientation" && CyPy_Quaternion::check(attr)) {
		this->m_value.m_orientation = Py::PythonClassObject<CyPy_Quaternion>::getCxxObject(attr)->m_value;
		return 0;
	}
	Vector3D vector;
	if (CyPy_Vector3D::check(attr)) {
		vector = Py::PythonClassObject<CyPy_Vector3D>::getCxxObject(attr)->m_value;
	} else if (CyPy_Point3D::check(attr)) {
		vector = WFMath::Vector<3>(Py::PythonClassObject<CyPy_Point3D>::getCxxObject(attr)->m_value);
	} else if (attr.isSequence()) {
		Py::Sequence seq(attr);
		if (seq.length() != 3) {
			throw Py::ValueError("value must be sequence of 3");
		}
		for (int i = 0; i < 3; i++) {
			if (seq[i].isNumeric()) {
				vector[i] = Py::Float(seq[i]).as_double();
			} else {
				throw Py::TypeError("value must be tuple of floats, or ints");
			}
		}
		vector.setValid();
	} else {
		throw Py::TypeError("value must be a vector");
	}
	if (nameStr == "pos") {
		this->m_value.m_pos = Point3D(vector.x(), vector.y(), vector.z());
		return 0;
	}
	if (nameStr == "velocity") {
		this->m_value.m_velocity = vector;
		return 0;
	}
	if (nameStr == "bbox") {
		this->m_value.setBBox(BBox(WFMath::Point<3>(0.f, 0.f, 0.f),
								   WFMath::Point<3>(vector.x(),
													vector.y(),
													vector.z())));
		return 0;
	}
	throw Py::AttributeError("unknown attribute");
}

template<typename EntityT, typename PythonEntityT>
Py::Object CyPy_Location<EntityT, PythonEntityT>::repr() {
	std::stringstream r;
	r << this->m_value;
	return Py::String(r.str());
}

template<typename EntityT, typename PythonEntityT>
Py::Object CyPy_Location<EntityT, PythonEntityT>::number_subtract(const Py::Object& other) {
	if (!CyPy_Location::check(other)) {
		throw Py::TypeError("Location must subtract Location");
	}

	auto& otherLocation = Py::PythonClassObject<CyPy_Location>::getCxxObject(other)->m_value;
	return CyPy_Vector3D::wrap(distanceTo(otherLocation, this->m_value));
}

