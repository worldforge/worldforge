/*
 Copyright (C) 2014 Erik Ogenvik

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

#include "Providers.h"

#include "common/TypeNode_impl.h"
#include "physics/BBox.h"

#include <algorithm>
#include <wfmath/atlasconv.h>

class MemEntity;

namespace EntityFilter {


template<typename EntityT>
FixedElementProvider<EntityT>::FixedElementProvider(Atlas::Message::Element element)
		: m_element(std::move(element)) {
}

template<typename EntityT>
void FixedElementProvider<EntityT>::value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const {
	value = m_element;
}

template<typename EntityT>
FixedTypeNodeProvider<EntityT>::FixedTypeNodeProvider(std::shared_ptr<Consumer<TypeNode<EntityT>>> consumer, const TypeNode<EntityT>& type)
		: ConsumingProviderBase<TypeNode<EntityT>, QueryContext<EntityT>>(std::move(consumer)), m_type(type) {
}

template<typename EntityT>
void FixedTypeNodeProvider<EntityT>::value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const {
	if (this->m_consumer) {
		this->m_consumer->value(value, m_type);
	} else {
		value = static_cast<Atlas::Message::PtrType>(const_cast<TypeNode<EntityT>*>(&m_type));
	}
}

template<typename EntityT>
const std::type_info* FixedTypeNodeProvider<EntityT>::getType() const {
	if (this->m_consumer) {
		return this->m_consumer->getType();
	} else {
		return &typeid(const TypeNode<EntityT>*);
	}
}

template<typename EntityT>
DynamicTypeNodeProvider<EntityT>::DynamicTypeNodeProvider(std::shared_ptr<Consumer<TypeNode<EntityT>>> consumer, std::string type)
		: ConsumingProviderBase<TypeNode<EntityT>, QueryContext<EntityT>>(std::move(consumer)), m_type(std::move(type)) {
}

template<typename EntityT>
void DynamicTypeNodeProvider<EntityT>::value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const {
	if (context.type_lookup_fn) {
		auto type = context.type_lookup_fn(m_type);

		if (this->m_consumer && type) {
			this->m_consumer->value(value, *type);
		} else {
			value = static_cast<Atlas::Message::PtrType>(const_cast<TypeNode<EntityT>*>(type));
		}
	}
}

template<typename EntityT>
const std::type_info* DynamicTypeNodeProvider<EntityT>::getType() const {
	if (this->m_consumer) {
		return this->m_consumer->getType();
	} else {
		return &typeid(const TypeNode<EntityT>*);
	}
}

template<typename EntityT>
MemoryProvider<EntityT>::MemoryProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer)
		: ConsumingProviderBase<Atlas::Message::Element, QueryContext<EntityT>>(std::move(consumer)) {

}

template<typename EntityT>
void MemoryProvider<EntityT>::value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const {
	if (context.memory_lookup_fn) {
		const Atlas::Message::MapType& memory = context.memory_lookup_fn(context.entityLoc.entity.getId());

		if (this->m_consumer) {
			this->m_consumer->value(value, memory);
			return;
		} else {
			value = memory;
		}
	}
}

template<typename EntityT>
EntityProvider<EntityT>::EntityProvider(std::shared_ptr<Consumer<EntityT>> consumer)
		: ConsumingProviderBase<EntityT, QueryContext<EntityT>>(std::move(consumer)) {
}

template<typename EntityT>
void EntityProvider<EntityT>::value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const {
	if (this->m_consumer) {
		this->m_consumer->value(value, context.entityLoc.entity);
	} else {
		value = (Atlas::Message::PtrType*) (&context.entityLoc.entity);
	}
}

template<typename EntityT>
const std::type_info* EntityProvider<EntityT>::getType() const {
	if (this->m_consumer) {
		return this->m_consumer->getType();
	} else {
		return &typeid(const EntityT*);
	}
}

template<typename EntityT>
EntityLocationProvider<EntityT>::EntityLocationProvider(std::shared_ptr<Consumer<EntityT>> consumer)
		: ConsumingProviderBase<EntityT, QueryContext<EntityT>>(std::move(consumer)) {
}

template<typename EntityT>
void EntityLocationProvider<EntityT>::value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const {
	if (this->m_consumer) {
		this->m_consumer->value(value, context.entityLoc.entity);
	} else {
		value = static_cast<Atlas::Message::PtrType>(const_cast<QueryEntityLocation<EntityT>*>(&context.entityLoc));
	}
}

template<typename EntityT>
const std::type_info* EntityLocationProvider<EntityT>::getType() const {
	if (this->m_consumer) {
		return this->m_consumer->getType();
	} else {
		return &typeid(const QueryEntityLocation<EntityT>*);
	}
}

template<typename EntityT>
ActorProvider<EntityT>::ActorProvider(std::shared_ptr<Consumer<EntityT>> consumer)
		: EntityProvider<EntityT>(std::move(consumer)) {
}

template<typename EntityT>
void ActorProvider<EntityT>::value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const {
	if (this->m_consumer && context.actor) {
		this->m_consumer->value(value, *context.actor);
	} else {
		value = static_cast<Atlas::Message::PtrType>(const_cast<EntityT*>(context.actor));
	}
}

template<typename EntityT>
ToolProvider<EntityT>::ToolProvider(std::shared_ptr<Consumer<EntityT>> consumer)
		: EntityProvider<EntityT>(std::move(consumer)) {
}

template<typename EntityT>
void ToolProvider<EntityT>::value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const {
	if (this->m_consumer && context.tool) {
		this->m_consumer->value(value, *context.tool);
	} else {
		value = static_cast<Atlas::Message::PtrType>(const_cast<EntityT*>(context.tool));
	}
}

template<typename EntityT>
ChildProvider<EntityT>::ChildProvider(std::shared_ptr<Consumer<EntityT>> consumer)
		: EntityProvider<EntityT>(std::move(consumer)) {
}

template<typename EntityT>
void ChildProvider<EntityT>::value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const {
	if (this->m_consumer && context.child) {
		this->m_consumer->value(value, *context.child);
	} else {
		value = static_cast<Atlas::Message::PtrType>(const_cast<EntityT*>(context.child));
	}
}

template<typename EntityT>
SelfEntityProvider<EntityT>::SelfEntityProvider(std::shared_ptr<Consumer<EntityT>> consumer) :
		ConsumingProviderBase<EntityT, QueryContext<EntityT>>(std::move(consumer)) {
}

template<typename EntityT>
void SelfEntityProvider<EntityT>::value(Atlas::Message::Element& value,
										const QueryContext<EntityT>& context) const {
	if (!context.self_entity) {
		return;
	} else if (this->m_consumer) {
		return this->m_consumer->value(value, *context.self_entity);
	} else {
		value = (Atlas::Message::PtrType) (context.self_entity);
	}
}

template<typename EntityT>
const std::type_info* SelfEntityProvider<EntityT>::getType() const {
	if (this->m_consumer) {
		return this->m_consumer->getType();
	} else {
		return &typeid(const EntityT*);
	}
}

template<typename EntityT>
EntityTypeProvider<EntityT>::EntityTypeProvider(std::shared_ptr<Consumer<TypeNode<EntityT>>> consumer)
		: ConsumingProviderBase<TypeNode<EntityT>, EntityT>(std::move(consumer)) {

}

template<typename EntityT>
void EntityTypeProvider<EntityT>::value(Atlas::Message::Element& value, const EntityT& entity) const {
	if (!entity.getType()) {
		return;
	}

	if (this->m_consumer) {
		this->m_consumer->value(value, *entity.getType());
	} else {
		value = static_cast<Atlas::Message::PtrType>(const_cast<TypeNode<EntityT>*>(entity.getType()));
	}
}

template<typename EntityT>
const std::type_info* EntityTypeProvider<EntityT>::getType() const {
	if (this->m_consumer) {
		return this->m_consumer->getType();
	} else {
		return &typeid(const TypeNode<EntityT>*);
	}
}

template<typename EntityT>
void EntityIdProvider<EntityT>::value(Atlas::Message::Element& value,
									  const EntityT& entity) const {
	value = Atlas::Message::Element(entity.getIntId());
}

template<typename EntityT>
TypeNodeProvider<EntityT>::TypeNodeProvider(std::string attribute_name)
		: m_attribute_name(std::move(attribute_name)) {

}

template<typename EntityT>
void TypeNodeProvider<EntityT>::value(Atlas::Message::Element& value, const TypeNode<EntityT>& type) const {
	if (m_attribute_name == "name") {
		value = type.name();
	}
}

template<typename EntityT>
BBoxProvider<EntityT>::BBoxProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, Measurement measurement)
		: ConsumingProviderBase<Atlas::Message::Element, EntityT>(std::move(consumer)), m_measurement(measurement) {

}

template<typename EntityT>
void BBoxProvider<EntityT>::value(Atlas::Message::Element& value, const EntityT& entity) const {
	Atlas::Message::Element data;
	if (entity.getAttr("bbox", data) == 0) {
		BBox bbox;
		bbox.fromAtlas(data);
		switch (m_measurement) {
			case Measurement::WIDTH:
				value = bbox.highCorner().x() - bbox.lowCorner().x();
				break;
			case Measurement::DEPTH:
				value = bbox.highCorner().z() - bbox.lowCorner().z();
				break;
			case Measurement::HEIGHT:
				value = bbox.highCorner().y() - bbox.lowCorner().y();
				break;
			case Measurement::VOLUME:
				value = (bbox.highCorner().x() - bbox.lowCorner().x()) * (bbox.highCorner().y() - bbox.lowCorner().y()) * (bbox.highCorner().z() - bbox.lowCorner().z());
				break;
			case Measurement::AREA:
				value = (bbox.highCorner().x() - bbox.lowCorner().x()) * (bbox.highCorner().z() - bbox.lowCorner().z());
				break;
		}
	}
}


template<typename EntityT>
SoftPropertyProvider<EntityT>::SoftPropertyProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, const std::string& attribute_name) :
		ConsumingNamedAttributeProviderBase<Atlas::Message::Element, EntityT>(std::move(consumer), attribute_name) {
}

template<typename EntityT>
void SoftPropertyProvider<EntityT>::value(Atlas::Message::Element& value, const EntityT& entity) const {
	auto prop = entity.getProperty(this->m_attribute_name);
	if (!prop) {
		return;
	}
	if (this->m_consumer) {
		Atlas::Message::Element propElem;
		prop->get(propElem);
		this->m_consumer->value(value, propElem);
	} else {
		prop->get(value);
	}
}

inline MapProvider::MapProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, const std::string& attribute_name) :
		ConsumingNamedAttributeProviderBase<Atlas::Message::Element, Atlas::Message::Element>(std::move(consumer), attribute_name) {
}

inline void MapProvider::value(Atlas::Message::Element& value, const Atlas::Message::Element& parent_element) const {
	if (!parent_element.isMap()) {
		return;
	}
	auto I = parent_element.Map().find(m_attribute_name);
	if (I == parent_element.Map().end()) {
		return;
	}
	if (m_consumer) {
		m_consumer->value(value, I->second);
	} else {
		value = I->second;
	}
}

template<typename EntityT>
void ContainsProvider<EntityT>::value(Atlas::Message::Element& value,
									  const EntityT& entity) const {
	value = Atlas::Message::PtrType{const_cast<std::set<Ref<EntityT>>*>( entity.containsAsPtr())};
}

template<typename EntityT>
const std::type_info* ContainsProvider<EntityT>::getType() const {
	return &typeid(const std::set<Ref<EntityT>>*);
}

template<typename EntityT>
ContainsRecursiveFunctionProvider<EntityT>::ContainsRecursiveFunctionProvider(std::shared_ptr<Consumer<QueryContext<EntityT>>> container,
																			  std::shared_ptr<Predicate<EntityT>> condition,
																			  bool recursive) :
		m_condition(std::move(condition)),
		m_consumer(std::move(container)),
		m_recursive(recursive) {
	if (m_consumer->getType() != &typeid(const std::set<Ref<EntityT>>*)) {
		throw std::invalid_argument(
				"first argument of contains_recursive must return a valid entity container");
	}
}

template<typename EntityT>
void ContainsRecursiveFunctionProvider<EntityT>::value(Atlas::Message::Element& value,
													   const QueryContext<EntityT>& context) const {
	Atlas::Message::Element container;
	m_consumer->value(container, context);
	if (container.isPtr()) {
		value = checkContainer((std::set<Ref<EntityT>>*) container.Ptr(), context);
	} else {
		value = false;
	}
}

template<typename EntityT>
bool ContainsRecursiveFunctionProvider<EntityT>::checkContainer(const std::set<Ref<EntityT>>* container,
																const QueryContext<EntityT>& context) const {
	if (container) {
		QueryContext childContext = context;
		for (auto& item: *container) {
			childContext.child = item.get();
			if (m_condition->isMatch(childContext)) {
				return true;
			} else {
				//If an item we're looking at also contains other items - check them too using recursion
				if (m_recursive && item->containsAsPtr() && !item->containsAsPtr()->empty()) {
					if (this->checkContainer(item->containsAsPtr(), context)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}


template<typename EntityT>
GetEntityFunctionProvider<EntityT>::GetEntityFunctionProvider(std::shared_ptr<Consumer<QueryContext<EntityT>>> entity_provider, std::shared_ptr<Consumer<EntityT>> consumer)
		: ConsumingProviderBase<EntityT, QueryContext<EntityT>>(std::move(consumer)),
		  m_entity_provider(std::move(entity_provider)) {

}

template<typename EntityT>
void GetEntityFunctionProvider<EntityT>::value(Atlas::Message::Element& value,
											   const QueryContext<EntityT>& context) const {

	if (m_entity_provider && context.entity_lookup_fn) {
		Atlas::Message::Element innerValue;
		m_entity_provider->value(innerValue, context);
		if (innerValue.isMap()) {
			auto I = innerValue.Map().find("$eid");
			if (I != innerValue.Map().end() && I->second.isString()) {
				auto entity = context.entity_lookup_fn(I->second.String());
				if (entity) {
					if (this->m_consumer) {
						return this->m_consumer->value(value, *entity);
					} else {
						value = entity.get();
					}
				}
			}
		}
	}
}

template<typename EntityT>
const std::type_info* GetEntityFunctionProvider<EntityT>::getType() const {
	if (this->m_consumer) {
		return this->m_consumer->getType();
	} else {
		return &typeid(const EntityT*);
	}
}

}
