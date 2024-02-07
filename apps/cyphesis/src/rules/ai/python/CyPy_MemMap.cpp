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

#include "rules/PhysicalProperties_impl.h"
#include "CyPy_MemMap.h"
#include "CyPy_MemEntity.h"
#include "rules/python/CyPy_RootEntity.h"
#include "rules/python/CyPy_Location_impl.h"
#include "rules/python/CyPy_Element.h"
#include "rules/entityfilter/python/CyPy_EntityFilter_impl.h"

#include "rules/entityfilter/Providers_impl.h"

#include <Atlas/Objects/Factories.h>
#include "rules/entityfilter/ParserDefinitions_impl.h"
#include "rules/ai/MemEntity.h"

template
struct EntityFilter::parser::query_parser<std::string::const_iterator, MemEntity>;
//namespace EntityFilter {
//template<>
//void ContainsProvider<MemEntity>::value(Atlas::Message::Element& value,
//										const MemEntity& entity) const {
//	value = Atlas::Message::PtrType(&entity.m_contains);
//}
//
//template<>
//bool ContainsRecursiveFunctionProvider<MemEntity>::checkContainer(std::set<Ref<MemEntity>>* container,
//																  const QueryContext<MemEntity>& context) const {
//	if (container) {
//		QueryContext childContext = context;
//		for (auto& item: *container) {
//			childContext.child = item.get();
//			if (m_condition->isMatch(childContext)) {
//				return true;
//			} else {
//				//If an item we're looking at also contains other items - check them too using recursion
//				if (m_recursive && !item->m_contains.empty()) {
//					if (this->checkContainer(&item->m_contains, context)) {
//						return true;
//					}
//				}
//			}
//		}
//	}
//	return false;
//}
//
//}


using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

CyPy_MemMap::CyPy_MemMap(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
		: WrapperBase(self, args, kwds) {
	throw Py::TypeError("MemMap() can't be instanced from Python.");
}

CyPy_MemMap::CyPy_MemMap(Py::PythonClassInstance* self, MemMap* value)
		: WrapperBase(self, value) {

}

void CyPy_MemMap::init_type() {
	behaviors().name("MemMap");
	behaviors().doc("");

	register_method<&CyPy_MemMap::find_by_location>("find_by_location");
	register_method<&CyPy_MemMap::find_by_type>("find_by_type");
//	register_method<&CyPy_MemMap::updateAdd>("add");
//	register_method<&CyPy_MemMap::updateAdd>("update");
//	register_method<&CyPy_MemMap::delete_>("delete");
	register_method<&CyPy_MemMap::get_all>("get_all");
	register_method<&CyPy_MemMap::get>("get");
	register_method<&CyPy_MemMap::get_add>("get_add");
	register_method<&CyPy_MemMap::find_by_filter>("find_by_filter");
	register_method<&CyPy_MemMap::find_by_location_query>("find_by_location_query");
	register_method<&CyPy_MemMap::add_entity_memory>("add_entity_memory",
													 "add_entity_memory(entity_id, name, memory)\n--\n\nAdds memory for an entity. First parameter is entity id, second is memory name, and third is the memory contents.");
	register_method<&CyPy_MemMap::remove_entity_memory>("remove_entity_memory",
														"Removes memory for an entity. First parameter is entity id, second is memory name. If no memory name is supplied all memory for that entity will be removed");
	register_method<&CyPy_MemMap::recall_entity_memory>("recall_entity_memory");


	behaviors().readyType();
}

Py::Object CyPy_MemMap::find_by_location(const Py::Tuple& args) {
	Py::List list;
	args.verify_length(3);
	auto& location = verifyObject<CyPy_Location<MemEntity, CyPy_MemEntity>>(args[0]);
	if (!location.isValid()) {
		throw Py::RuntimeError("Location is incomplete");
	}
	auto radius = verifyNumeric(args[1]);
	auto type = verifyString(args[2]);
	auto res = m_value->findByLocation(location, radius, type);

	for (auto& entry: res) {
		list.append(CyPy_MemEntity::wrap(entry));
	}
	return list;
}

Py::Object CyPy_MemMap::find_by_type(const Py::Tuple& args) {
	Py::List list;

	args.verify_length(1);
	auto what = verifyString(args.front());
	auto res = m_value->findByType(what);

	for (auto& entry: res) {
		list.append(CyPy_MemEntity::wrap(entry));
	}
	return list;
}
Py::Object CyPy_MemMap::get(const Py::Tuple& args) {
	args.verify_length(1);

	auto ret = m_value->get(verifyString(args[0]));
	if (!ret) {
		return Py::None();
	}
	return CyPy_MemEntity::wrap(ret.get());
}

Py::Object CyPy_MemMap::get_add(const Py::Tuple& args) {
	args.verify_length(1);

	auto ret = m_value->getAdd(verifyString(args[0]));
	if (!ret) {
		return Py::None();
	}
	return CyPy_MemEntity::wrap(ret.get());
}

Py::Object CyPy_MemMap::get_all() {
	Py::List list;

	for (auto& entry: m_value->getEntities()) {
		list.append(CyPy_MemEntity::wrap(entry.second.get()));
	}
	return list;
}

///\brief Return Python list of entities that match a given Filter
Py::Object CyPy_MemMap::find_by_filter(const Py::Tuple& args) {
	args.verify_length(1);
	auto& filter = verifyObject<CyPy_Filter<MemEntity>>(args[0]);

	Py::List list;

	for (auto& entry: m_value->getEntities()) {
		EntityFilter::QueryContext queryContext = createFilterContext(entry.second.get(), m_value);

		if (filter->match(queryContext)) {
			list.append(CyPy_MemEntity::wrap(entry.second.get()));
		}
	}
	return list;
}

///\brief find entities using a query in a specified location
Py::Object CyPy_MemMap::find_by_location_query(const Py::Tuple& args) {
	args.verify_length(3);

	auto& location = verifyObject<CyPy_Location<MemEntity, CyPy_MemEntity>>(args[0]);
	auto radius = verifyNumeric(args[1]);
	auto& filter = verifyObject<CyPy_Filter<MemEntity>>(args[2]);

	if (!location.isValid()) {
		throw Py::RuntimeError("Location is incomplete");
	}

	//Create a vector and fill it with entities that match the given filter and are in range
	float square_range = radius * radius;

	Py::List list;
	if (location.m_parent) {
		for (const auto& entry: location.m_parent->m_contains) {
			EntityFilter::QueryContext queryContext = createFilterContext(entry.get(), m_value);

			if (filter->match(queryContext)) {
				auto pos = PositionProperty<MemEntity>::extractPosition(*entry);
				if (pos.isValid() && squareDistance(location.pos(), pos) < square_range) {
					list.append(CyPy_MemEntity::wrap(entry));
				}
			}
		}
	}

	return list;
}

Py::Object CyPy_MemMap::add_entity_memory(const Py::Tuple& args) {

	args.verify_length(3);
	auto id = verifyString(args[0]);
	auto memory_name = verifyString(args[1]);

	m_value->addEntityMemory(id, memory_name, CyPy_Element::asElement(args[2]));

	return Py::None();
}

Py::Object CyPy_MemMap::remove_entity_memory(const Py::Tuple& args) {

	args.verify_length(1, 2);
	auto id = verifyString(args[0]);
	if (args.length() == 2) {
		m_value->removeEntityMemory(id, verifyString(args[1]));
	} else {
		m_value->removeEntityMemory(id, "");
	}

	return Py::None();
}

Py::Object CyPy_MemMap::recall_entity_memory(const Py::Tuple& args) {

	args.verify_length(2, 3);
	auto id = verifyString(args[0]);
	auto memory_name = verifyString(args[1]);

	Atlas::Message::Element element_val;
	m_value->recallEntityMemory(id, memory_name, element_val);
	if (element_val.isNone()) {
		//Check if a default value was specified as third parameter.
		if (args.length() == 3) {
			return args[2];
		}
		return Py::None();
	}
	return CyPy_Element::wrap(element_val);
}

EntityFilter::QueryContext<MemEntity> CyPy_MemMap::createFilterContext(MemEntity* entity, MemMap* memMap) {
	EntityFilter::QueryContext<MemEntity> queryContext{.entityLoc = EntityFilter::QueryEntityLocation(*entity)};
	queryContext.type_lookup_fn = [memMap](const std::string& id) -> const TypeNode<MemEntity>* {
		return memMap->getTypeStore().getType(id);
	};
	queryContext.entity_lookup_fn = [memMap](const std::string& id) -> Ref<MemEntity> {
		return memMap->get(id);
	};
	queryContext.memory_lookup_fn = [memMap](const std::string& id) -> const Atlas::Message::MapType& {
		auto I = memMap->getEntityRelatedMemory().find(id);
		if (I != memMap->getEntityRelatedMemory().end()) {
			return I->second;
		}
		static Atlas::Message::MapType empty;
		return empty;
	};
	return queryContext;
}


