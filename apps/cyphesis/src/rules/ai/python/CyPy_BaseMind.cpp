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

#include "CyPy_BaseMind.h"

#include "CyPy_MemMap.h"
#include "CyPy_MemEntity.h"
#include "rules/python/PythonWrapper_impl.h"
#include "rules/python/CyPy_Props_impl.h"
#include "rules/python/PythonScriptFactory_impl.h"
#include "rules/entityfilter/python/CyPy_EntityFilter_impl.h"
#include "rules/ai/AwareMind.h"
#include "rules/Script_impl.h"


#include "navigation/Steering.h"
#include "navigation/Awareness.h"

#include "common/id.h"
#include "CyPy_Steering.h"

template
struct EntityFilter::parser::query_parser<std::string::const_iterator, MemEntity>;

template<>
int CyPy_Props<MemEntity>::setattro(const Py::String& name, const Py::Object& attr) {
	return 0;
}

template<>
Py::Object wrapPython(BaseMind* value) {
	return CyPy_BaseMind::wrap(value);
}

CyPy_BaseMind::CyPy_BaseMind(Py::PythonClassInstance* self, Ref<BaseMind> value)
		: WrapperBase(self, std::move(value)) {

}

CyPy_BaseMind::CyPy_BaseMind(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
		: WrapperBase(self, args, kwds) {
	args.verify_length(1);

	auto arg = args.front();
//    if (arg.isString()) {
//        auto id = verifyString(args.front());
//
//        long intId = integerId(id);
//        if (intId == -1L) {
//            throw Py::TypeError("Mind() requires string/int ID");
//        }
//        m_value = new BaseMind(id, intId);
//    } else
	if (CyPy_BaseMind::check(arg)) {
		m_value = CyPy_BaseMind::value(arg);
	} else {
		throw Py::TypeError("Mind() requires string ID or Mind");
	}
}


CyPy_BaseMind::~CyPy_BaseMind() = default;


void CyPy_BaseMind::init_type() {
	behaviors().name("Mind");
	behaviors().doc("");

	behaviors().supportRichCompare();

	behaviors().supportStr();

	register_method<&CyPy_BaseMind::add_hook_set>("add_hook_set", PYCXX_SIG_DOC("add_hook_set(method_name)", "Adds a hook that will be called whenever an entity is added."));

	register_method<&CyPy_BaseMind::update_hook_set>("update_hook_set", PYCXX_SIG_DOC("update_hook_set(method_name)", "Adds a hook that will be called whenever an entity is updated."));
	register_method<&CyPy_BaseMind::delete_hook_set>("delete_hook_set", PYCXX_SIG_DOC("delete_hook_set(method_name)", "Adds a hook that will be called whenever an entity is deleted."));

	register_method<&CyPy_BaseMind::addPropertyCallback>("add_property_callback",
														 PYCXX_SIG_DOC("add_property_callback(property, method_name)", "Adds a callback that will be called whenever the specified property changes."));


	register_method<&CyPy_BaseMind::matchEntity>("match_entity", PYCXX_SIG_DOC("match_entity(filter, entity)", "Returns true if the filter applies to the entity."));
	register_method<&CyPy_BaseMind::matchEntities>("match_entities",
												   PYCXX_SIG_DOC("match_entities(filter, entities)",
																 "Processes the supplied entities and returns a list containing only those that matched the filter."));

	behaviors().readyType();
}

Py::Object CyPy_BaseMind::matchEntity(const Py::Tuple& args) {
	args.verify_length(2);

	auto& filter = verifyObject<CyPy_Filter<MemEntity>>(args.getItem(0));
	auto& entity = verifyObject<CyPy_MemEntity>(args.getItem(1));

	EntityFilter::QueryContext queryContext = CyPy_MemMap::createFilterContext(entity.get(), m_value->getMap());
	queryContext.actor = m_value->getEntity().get();

	return Py::Boolean(filter->match(queryContext));
}

Py::Object CyPy_BaseMind::matchEntities(const Py::Tuple& args) {
	args.verify_length(2);

	auto& filter = verifyObject<CyPy_Filter<MemEntity>>(args.getItem(0));
	auto entities = verifyList(args.getItem(1));
	Py::List list;

	for (auto entity: entities) {
		auto queryContext = CyPy_MemMap::createFilterContext(verifyObject<CyPy_MemEntity>(entity).get(), m_value->getMap());
		queryContext.actor = m_value->getEntity().get();

		if (filter->match(queryContext)) {
			list.append(entity);
		}
	}

	return list;
}


Py::Object CyPy_BaseMind::getattro(const Py::String& name) {
	auto nameStr = name.as_string();
	if (nameStr == "entity") {
		if (m_value->getEntity()) {
			return WrapperBase<Ref<MemEntity>, CyPy_MemEntity>::wrap(m_value->getEntity());
		}
		return Py::None();
	}
	if (nameStr == "id") {
		return Py::String(m_value->getIdAsString());
	}
	if (nameStr == "map") {
		return CyPy_MemMap::wrap(m_value->getMap());
	}


	if (nameStr == "steering") {
		auto awareMind = dynamic_cast<AwareMind*>(m_value.get());
		if (!awareMind) {
			throw Py::TypeError("Not an AwareMind");
		}

		return CyPy_Steering::wrap(Ref<AwareMind>(awareMind));

	}

	if (nameStr == "props") {
		return CyPy_Props<MemEntity>::wrap(m_value->getEntity());
	}


	//TODO: remove CyPy_WorldTime
//    if (nameStr == "time") {
//        return CyPy_WorldTime::wrap(m_value->getTime());
//    }

	return PythonExtensionBase::getattro(name);
}

int CyPy_BaseMind::setattro(const Py::String& name, const Py::Object& attr) {
	auto nameStr = name.as_string();

//    if (nameStr == "mind") {
//        if (!PyMind_Check(v)) {
//            PyErr_SetString(PyExc_TypeError, "Mind.mind requires Mind");
//            return -1;
//        }
//        self->m_entity.m = ((PyEntity*) v)->m_entity.m;
//        return 0;
//    }

	if (nameStr == "map") {
		throw Py::AttributeError("Setting map on mind is forbidden");
	}

	return PythonExtensionBase::setattro(name, attr);
}

Py::Object CyPy_BaseMind::addPropertyCallback(const Py::Tuple& args) {
	args.verify_length(2);
	m_value->addPropertyScriptCallback(verifyString(args[0]), verifyString(args[1]));
	return Py::None();
}

Py::Object CyPy_BaseMind::str() {
	if (m_value->getEntity()) {
		return Py::String(fmt::format("BaseMind {}, entity {}", m_value->getIdAsString(), m_value->getEntity()->describeEntity()));
	}
	return Py::String(fmt::format("BaseMind {}", m_value->getIdAsString()));
}


Py::Object CyPy_BaseMind::add_hook_set(const Py::Tuple& args) {
	args.verify_length(1);
	m_value->setAddHook(verifyString(args[0]));
	return Py::None();
}

Py::Object CyPy_BaseMind::update_hook_set(const Py::Tuple& args) {
	args.verify_length(1);
	m_value->setUpdateHook(verifyString(args[0]));
	return Py::None();
}

Py::Object CyPy_BaseMind::delete_hook_set(const Py::Tuple& args) {
	args.verify_length(1);
	m_value->setDeleteHook(verifyString(args[0]));
	return Py::None();
}


template<>
PythonScriptFactory<MemEntity, BaseMind>::PythonScriptFactory(const std::string& package,
															  const std::string& type) :
		PythonClass(package, type) {
}

template
class PythonScriptFactory<MemEntity, BaseMind>;
