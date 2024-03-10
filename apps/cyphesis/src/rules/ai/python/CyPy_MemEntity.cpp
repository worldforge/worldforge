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

#include "CyPy_MemEntity.h"

#include <utility>
#include "common/id.h"
#include "Atlas/Objects/SmartPtr.h"
#include "common/TypeNode_impl.h"
#include "common/Property_impl.h"
#include "rules/Location_impl.h"
#include "rules/PhysicalProperties_impl.h"
#include "rules/BBoxProperty_impl.h"
#include "rules/python/CyPy_RootEntity.h"
#include "rules/python/EntityHelper.h"
#include "rules/python/PythonWrapper_impl.h"
#include "rules/python/CyPy_Point3D.h"
#include "rules/python/CyPy_Operation.h"
#include "rules/python/CyPy_EntityLocation_impl.h"
#include "rules/python/CyPy_Props_impl.h"
#include "rules/python/CyPy_Location_impl.h"


template<>
Py::Object wrapPython(MemEntity* value) {
	return CyPy_MemEntity::wrap(value);
}


CyPy_MemEntity::CyPy_MemEntity(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
		: WrapperBase(self, args, kwds) {
	args.verify_length(1);

	auto arg = args.front();
//	if (arg.isString()) {
//		auto id = verifyString(args.front());
//
//		long intId = integerId(id);
//		if (intId == -1L) {
//			throw Py::TypeError("MemEntity() requires string/int ID");
//		}
//		m_value = new MemEntity(RouterId(intId));
//	} else
	if (CyPy_MemEntity::check(arg)) {
		m_value = CyPy_MemEntity::value(arg);
	} else {
		throw Py::TypeError("MemEntity() requires string ID or MemEntity");
	}
}

CyPy_MemEntity::~CyPy_MemEntity() = default;

namespace {
/**
 * Older compilers can't handle the lambdas being local in the ::init_type method, so we need to put them here instead.
 * This might change when all compilers catch up to C++17 and the more lax requirements of letting lambdas be constexpr by default.
 */
auto get_child = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	args.verify_length(1);
	auto child_id = verifyString(args.front());
	for (auto& child: value->m_contains) {
		if (child_id == child->getIdAsString()) {
			return CyPy_MemEntity::wrap(child);
		}
	}
	return Py::None();
};
auto describe_entity = [](Ref<MemEntity>& value) -> Py::Object {
	return Py::String(value->describeEntity());
};
auto is_type = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	args.verify_length(1);

	auto type = value->getType();
	if (!type) {
		return Py::False();
	}
	return Py::Boolean(type->isTypeOf(verifyString(args[0])));
};
auto get_prop_num_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return get_prop_num(*value, args);
};
auto has_prop_num_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return has_prop_num(*value, args);
};
auto get_prop_float_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return get_prop_float(*value, args);
};
auto has_prop_float_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return has_prop_float(*value, args);
};
auto get_prop_int_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return get_prop_int(*value, args);
};
auto has_prop_int_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return has_prop_int(*value, args);
};
auto get_prop_string_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return get_prop_string(*value, args);
};
auto has_prop_string_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return has_prop_string(*value, args);
};
auto get_prop_bool_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return get_prop_bool(*value, args);
};
auto has_prop_bool_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return has_prop_bool(*value, args);
};
auto get_prop_list_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return get_prop_list(*value, args);
};
auto has_prop_list_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return has_prop_list(*value, args);
};
auto get_prop_map_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return get_prop_map(*value, args);
};
auto has_prop_map_fn = [](Ref<MemEntity>& value, const Py::Tuple& args) -> Py::Object {
	return has_prop_map(*value, args);
};
}

void CyPy_MemEntity::init_type() {


	behaviors().name("MemEntity");
	behaviors().doc("");

	behaviors().supportRichCompare();
	behaviors().supportStr();


	register_method<get_child>("get_child");


	register_method<describe_entity>("describe_entity");


	register_method<is_type>("is_type", "Returns true if the current entity is of supplied string type.");


	register_method<get_prop_num_fn>("get_prop_num");
	register_method<has_prop_num_fn>("has_prop_num");

	register_method<get_prop_float_fn>("get_prop_float");
	register_method<has_prop_float_fn>("has_prop_float");

	register_method<get_prop_int_fn>("get_prop_int");
	register_method<has_prop_int_fn>("has_prop_int");

	register_method<get_prop_string_fn>("get_prop_string");
	register_method<has_prop_string_fn>("has_prop_string");

	register_method<get_prop_bool_fn>("get_prop_bool");
	register_method<has_prop_bool_fn>("has_prop_bool");

	register_method<get_prop_list_fn>("get_prop_list");
	register_method<has_prop_list_fn>("has_prop_list");

	register_method<get_prop_map_fn>("get_prop_map");
	register_method<has_prop_map_fn>("has_prop_map");


	behaviors().readyType();

}

CyPy_MemEntity::CyPy_MemEntity(Py::PythonClassInstance* self, Ref<MemEntity> value)
		: WrapperBase(self, std::move(value)) {

}

Py::Object CyPy_MemEntity::getattro(const Py::String& name) {
	auto nameStr = name.as_string();
	if (nameStr == "id") {
		return Py::String(this->m_value->getIdAsString());
	}

	if (nameStr == "parent") {
		if (this->m_value->m_parent) {
			return wrap(this->m_value->m_parent);
		} else {
			return Py::None();
		}
	}

	if (nameStr == "props") {
		return CyPy_Props<MemEntity>::wrap(this->m_value);
	}

	if (nameStr == "type") {
		if (!this->m_value->getType()) {
			return Py::String("");
		}
		return Py::String(this->m_value->getType()->name());
	}


	if (nameStr == "location") {
		Location<MemEntity> location;
		if (this->m_value->m_parent != nullptr) {
			location.m_parent = this->m_value->m_parent;
		}
		if (auto prop = this->m_value->template getPropertyClassFixed<PositionProperty<MemEntity>>()) {
			location.m_pos = prop->data();
		}
		if (auto prop = this->m_value->template getPropertyClassFixed<VelocityProperty<MemEntity>>()) {
			location.m_velocity = prop->data();
		}
		if (auto prop = this->m_value->template getPropertyClassFixed<OrientationProperty<MemEntity>>()) {
			location.m_orientation = prop->data();
		}
		if (auto prop = this->m_value->template getPropertyClassFixed<AngularVelocityProperty<MemEntity>>()) {
			location.m_angularVelocity = prop->data();
		}
		if (auto prop = this->m_value->template getPropertyClassFixed<BBoxProperty<MemEntity>>()) {
			location.setBBox(prop->data());
		}

		return CyPy_Location<MemEntity, CyPy_MemEntity>::wrap(location);
	}

	if (nameStr == "contains") {
		Py::List list;
		for (auto& child: this->m_value->m_contains) {
			list.append(wrap(child));
		}

		return list;
	}

	if (nameStr == "visible") {
		return Py::Boolean(!this->m_value->m_flags.hasFlags(mem_entity_destroyed));
	}

	if (nameStr == "is_destroyed") {
		return Py::Boolean(this->m_value->m_flags.hasFlags(mem_entity_destroyed));
	}

	return Py::PythonExtensionBase::getattro(name);
}

template<>
Py::Object wrapEntity(Ref<MemEntity> le) {
	//If there's already a script entity use that (as a cache mechanism)
	if (le->m_scriptEntity.has_value()) {
		auto wrapper = std::any_cast<Py::Object>(le->m_scriptEntity);
		if (!wrapper.isNone()) {
			return wrapper;
		}
	}
	auto wrapped = WrapperBase<Ref<MemEntity>, CyPy_MemEntity>::wrap(le);
	if (!wrapped.isNone()) {
		le->m_scriptEntity = std::any(wrapped);
		return wrapped;
	} else {
		return Py::None();
	}
}

Py::Object CyPy_MemEntity::wrap(Ref<MemEntity> value) {
	return wrapEntity<MemEntity>(std::move(value));
}

Py::Object CyPy_MemEntity::rich_compare(const Py::Object& other, int op) {

	if (CyPy_MemEntity::check(other)) {
		auto otherEntity = CyPy_MemEntity::value(other);
		if (op == Py_EQ) {
			return (this->m_value == otherEntity) ? Py::True() : Py::False();
		} else if (op == Py_NE) {
			return (this->m_value != otherEntity) ? Py::True() : Py::False();
		}

	} else {
		if (op == Py_EQ) {
			return Py::False();
		} else if (op == Py_NE) {
			return Py::True();
		}
	}

	throw Py::NotImplementedError("Not implemented");
}

Py::Object CyPy_MemEntity::str() {
	return Py::String(this->m_value->describeEntity());
}

