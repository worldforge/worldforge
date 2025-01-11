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

#ifndef PROVIDERS_H_
#define PROVIDERS_H_

#include "Predicates.h"

#include "modules/Ref.h"
#include "common/Router.h"
#include "common/TypeNode.h"


#include <Atlas/Message/Element.h>

#include <wfmath/point.h>


#include <map>
#include <functional>
#include <string>

template<typename>
class TypeNode;

namespace EntityFilter {

template<typename>
class Predicate;

/**
 * A simple struct for storing both an entity and a position.
 */
template<typename EntityT>
struct QueryEntityLocation {
	EntityT& entity;

	const WFMath::Point<3>* pos = nullptr;

	QueryEntityLocation() = delete;

	QueryEntityLocation(EntityT& entity_in) : entity(entity_in) {
	}

	QueryEntityLocation(EntityT& entity_in, const WFMath::Point<3>* pos_in) : entity(entity_in), pos(pos_in) {
	}
};

template<typename EntityT>
struct QueryContext {
	/**
	 * The main entity (with an optional position) which the filter is applied on. This is required, all other parameters are optional.
	 */
	QueryEntityLocation<EntityT> entityLoc;
	/**
	 * Represents an actor, mainly used together with the "tool" field. The "actor" is the entity which uses the "tool".
	 */
	EntityT* actor = nullptr;

	/**
	 * Represents something being used, often along with an "actor".
	 */
	EntityT* tool = nullptr;

	/**
	 * Do not set this field yourself. This is used when iterating through Entity sets, and is set automatically by the Providers.
	 */
	EntityT* child = nullptr;

	/**
	 * A memory, using when filters are attached to Minds.
	 */
	std::function<const Atlas::Message::MapType&(const std::string&)> memory_lookup_fn;

	//This field can be used by client code to specify entity for "self.*" query
	EntityT* self_entity = nullptr;

	/**
	 * Gets entities by their id. Used by the get_entity function. Optional.
	 */
	std::function<Ref<EntityT>(const std::string&)> entity_lookup_fn;

	/**
	 * Looks up types by name. Used by the DynamicTypeNodeProvider. Optional.
	 */
	std::function<const TypeNode<EntityT>*(const std::string&)> type_lookup_fn;

	/**
	 * If provided allows predicates to report on failures to match. This can be used to provide better messages to clients.
	 * Used by the DescribePredicate.
	 */
	std::function<void(const std::string&)> report_error_fn;
};

struct TypedProvider {
	inline virtual ~TypedProvider() = default;

	/**
	 * @brief Gets the type of the pointer provided by this instance, if that can be determined.
	 *
	 * This will only return a valid value if
	 * 1) An Element containing a pointer will always be returned.
	 * 2) The exact same type always will be returned.
	 * @return Null, or a type info instance defining the type returned as a pointer Element.
	 */
	virtual const std::type_info* getType() const = 0;
};


template<typename T>
class Consumer : public TypedProvider, public std::enable_shared_from_this<Consumer<T>> {
public:
	~Consumer() override = default;

	virtual void value(Atlas::Message::Element& value, const T& parent_value) const = 0;

	const std::type_info* getType() const override;

	bool isType(const std::type_info& typeInfo) const;

	template<typename TypeT>
	bool isType() const;
};

template<typename T>
bool Consumer<T>::isType(const std::type_info& typeInfo) const {
	auto type = getType();
	return type != nullptr && *type == typeInfo;
}

template<typename T>
template<typename TypeT>
bool Consumer<T>::isType() const {
	auto type = getType();
	return type != nullptr && *type == typeid(TypeT);
}


template<typename T>
const std::type_info* Consumer<T>::getType() const {
	return nullptr;
}


template<typename T>
class ProviderBase {
public:
	explicit ProviderBase(std::shared_ptr<Consumer<T>> consumer);

	virtual ~ProviderBase();

protected:
	std::shared_ptr<Consumer<T>> m_consumer;
};

template<typename T>
inline ProviderBase<T>::ProviderBase(std::shared_ptr<Consumer<T>> consumer)
	: m_consumer(std::move(consumer)) {
}

template<typename T>
inline ProviderBase<T>::~ProviderBase() = default;

template<typename TProviding, typename TConsuming>
class ConsumingProviderBase : public ProviderBase<TProviding>, public Consumer<TConsuming> {
public:
	explicit ConsumingProviderBase(std::shared_ptr<Consumer<TProviding>> consumer);

	~ConsumingProviderBase() override = default;

	const std::type_info* getType() const override;
};

template<typename TProviding, typename TConsuming>
inline ConsumingProviderBase<TProviding, TConsuming>::ConsumingProviderBase(std::shared_ptr<Consumer<TProviding>> consumer)
	: ProviderBase<TProviding>(consumer) {
}

template<typename TProviding, typename TConsuming>
inline const std::type_info* ConsumingProviderBase<TProviding, TConsuming>::getType() const {
	if (this->m_consumer) {
		return this->m_consumer->getType();
	}
	return nullptr;
}


template<typename T>
class NamedAttributeProviderBase : public ProviderBase<T> {
public:
	NamedAttributeProviderBase(std::shared_ptr<Consumer<T>> consumer, std::string attribute_name);

protected:
	const std::string m_attribute_name;
};

template<typename T>
inline NamedAttributeProviderBase<T>::NamedAttributeProviderBase(std::shared_ptr<Consumer<T>> consumer, std::string attribute_name)
	: ProviderBase<T>(consumer), m_attribute_name(std::move(attribute_name)) {
}

template<typename TProviding, typename TConsuming>
class ConsumingNamedAttributeProviderBase : public NamedAttributeProviderBase<TProviding>, public Consumer<TConsuming> {
public:
	ConsumingNamedAttributeProviderBase(std::shared_ptr<Consumer<TProviding>> consumer, const std::string& attribute_name);

	~ConsumingNamedAttributeProviderBase() override = default;

	const std::type_info* getType() const override;
};

template<typename TProviding, typename TConsuming>
inline ConsumingNamedAttributeProviderBase<TProviding, TConsuming>::ConsumingNamedAttributeProviderBase(std::shared_ptr<Consumer<TProviding>> consumer, const std::string& attribute_name)
	: NamedAttributeProviderBase<TProviding>(std::move(consumer), attribute_name) {
}

template<typename TProviding, typename TConsuming>
inline const std::type_info* ConsumingNamedAttributeProviderBase<TProviding, TConsuming>::getType() const {
	if (this->m_consumer) {
		return this->m_consumer->getType();
	}
	return nullptr;
}


template<typename EntityT>
class FixedElementProvider : public Consumer<QueryContext<EntityT>> {
public:
	explicit FixedElementProvider(Atlas::Message::Element element);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const override;

	const Atlas::Message::Element m_element;
};

template<typename EntityT>
class DynamicTypeNodeProvider : public ConsumingProviderBase<TypeNode<EntityT>, QueryContext<EntityT>> {
public:
	DynamicTypeNodeProvider(std::shared_ptr<Consumer<TypeNode<EntityT>>> consumer, std::string type);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const

	override;

	const std::type_info* getType() const

	override;

	const std::string m_type;
};

template<typename EntityT>
class FixedTypeNodeProvider : public ConsumingProviderBase<TypeNode<EntityT>, QueryContext<EntityT>> {
public:
	FixedTypeNodeProvider(std::shared_ptr<Consumer<TypeNode<EntityT>>> consumer, const TypeNode<EntityT>& type);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const override;

	const std::type_info* getType() const override;

	const TypeNode<EntityT>& m_type;
};

template<typename EntityT>
class MemoryProvider : public ConsumingProviderBase<Atlas::Message::Element, QueryContext<EntityT>> {
public:
	explicit MemoryProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>&) const override;
};

template<typename EntityT>
class EntityProvider : public ConsumingProviderBase<EntityT, QueryContext<EntityT>> {
public:
	explicit EntityProvider(std::shared_ptr<Consumer<EntityT>> consumer);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const override;

	const std::type_info* getType() const override;
};

template<typename EntityT>
class EntityLocationProvider : public ConsumingProviderBase<EntityT, QueryContext<EntityT>> {
public:
	explicit EntityLocationProvider(std::shared_ptr<Consumer<EntityT>> consumer);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const override;

	const std::type_info* getType() const override;
};

template<typename EntityT>
class ActorProvider : public EntityProvider<EntityT> {
public:
	explicit ActorProvider(std::shared_ptr<Consumer<EntityT>> consumer);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const override;

};

template<typename EntityT>
class ToolProvider : public EntityProvider<EntityT> {
public:
	explicit ToolProvider(std::shared_ptr<Consumer<EntityT>> consumer);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const override;

};

template<typename EntityT>
class ChildProvider : public EntityProvider<EntityT> {
public:
	explicit ChildProvider(std::shared_ptr<Consumer<EntityT>> consumer);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const override;

};

template<typename EntityT>
class SelfEntityProvider : public ConsumingProviderBase<EntityT, QueryContext<EntityT>> {
public:
	explicit SelfEntityProvider(std::shared_ptr<Consumer<EntityT>> consumer);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const override;

	const std::type_info* getType() const override;
};

template<typename EntityT>
class EntityTypeProvider : public ConsumingProviderBase<TypeNode<EntityT>, EntityT> {
public:
	explicit EntityTypeProvider(std::shared_ptr<Consumer<TypeNode<EntityT>>> consumer);

	void value(Atlas::Message::Element& value, const EntityT& entity) const override;

	const std::type_info* getType() const override;
};

//Provides integer ID of an entity
template<typename EntityT>
class EntityIdProvider : public Consumer<EntityT> {
public:
	void value(Atlas::Message::Element& value, const EntityT& entity) const override;
};

template<typename EntityT>
class TypeNodeProvider : public Consumer<TypeNode<EntityT>> {
public:
	explicit TypeNodeProvider(std::string attribute_name);

	void value(Atlas::Message::Element& value, const TypeNode<EntityT>& type) const override;

	const std::string m_attribute_name;
};

template<typename EntityT>
class BBoxProvider : public ConsumingProviderBase<Atlas::Message::Element, EntityT> {
public:
	enum class Measurement {
		HEIGHT, WIDTH, DEPTH, VOLUME, AREA
	};

	BBoxProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, Measurement measurement);

	void value(Atlas::Message::Element& value, const EntityT& prop) const override;

protected:
	Measurement m_measurement;
};

template<typename TProperty, typename EntityT>
class PropertyProvider : public ConsumingNamedAttributeProviderBase<TProperty, EntityT> {
public:
	PropertyProvider(std::shared_ptr<Consumer<TProperty>> consumer, const std::string& attribute_name);

	~PropertyProvider() override = default;

	void value(Atlas::Message::Element& value, const EntityT& entity) const override;
};

template<typename TProperty, typename EntityT>
PropertyProvider<TProperty, EntityT>::PropertyProvider(std::shared_ptr<Consumer<TProperty>> consumer, const std::string& attribute_name)
	: ConsumingNamedAttributeProviderBase<TProperty, EntityT>(consumer, attribute_name) {
}

template<typename TProperty, typename EntityT>
void PropertyProvider<TProperty, EntityT>::value(Atlas::Message::Element& value, const EntityT& entity) const {
	auto* prop = entity.template getPropertyClass<TProperty>(this->m_attribute_name);
	if (!prop) {
		return;
	}

	if (this->m_consumer) {
		this->m_consumer->value(value, *prop);
	} else {
		value = (void*)prop;
	}
}


template<typename EntityT>
class SoftPropertyProvider : public ConsumingNamedAttributeProviderBase<Atlas::Message::Element, EntityT> {
public:
	SoftPropertyProvider(std::shared_ptr<Consumer<Atlas::Message::Element>> consumer, const std::string& attribute_name);

	void value(Atlas::Message::Element& value, const EntityT& entity) const override;
};

class MapProvider : public ConsumingNamedAttributeProviderBase<Atlas::Message::Element, Atlas::Message::Element> {
public:
	MapProvider(std::shared_ptr<Consumer> consumer, const std::string& attribute_name);

	void value(Atlas::Message::Element& value, const Atlas::Message::Element& parent_element) const override;
};


///\brief This class retrieves a pointer to the m_contains property of a given entity
template<typename EntityT>
class ContainsProvider : public Consumer<EntityT> {
public:
	void value(Atlas::Message::Element& value, const EntityT& entity) const override;

	const std::type_info* getType() const override;
};

///\brief This class uses container Consumer to retrieve a container and condition Predicate
///to check whether there exists an entity within the container that matches the given condition
template<typename EntityT>
class ContainsRecursiveFunctionProvider : public Consumer<QueryContext<EntityT>> {
public:
	ContainsRecursiveFunctionProvider(std::shared_ptr<Consumer<QueryContext<EntityT>>> container,
									  std::shared_ptr<Predicate<EntityT>> condition,
									  bool recursive
	);

	void value(Atlas::Message::Element& value,
			   const QueryContext<EntityT>& context) const

	override;

private:
	///\brief Condition used to match entities within the container
	std::shared_ptr<Predicate<EntityT>> m_condition;
	///\brief A Consumer which must return std::set<Ref<EntityT>>* i.e. entity.contains
	std::shared_ptr<Consumer<QueryContext<EntityT>>> m_consumer;

	bool m_recursive;

	bool checkContainer(const std::set<Ref<EntityT>>* container,
						const QueryContext<EntityT>& context) const;

};


template<typename EntityT>
class GetEntityFunctionProvider : public ConsumingProviderBase<EntityT, QueryContext<EntityT>> {
public:
	explicit GetEntityFunctionProvider(std::shared_ptr<Consumer<QueryContext<EntityT>>> entity_provider,
									   std::shared_ptr<Consumer<EntityT>>
									   consumer);

	void value(Atlas::Message::Element& value, const QueryContext<EntityT>& context) const override;

	const std::type_info* getType() const override;

private:
	std::shared_ptr<Consumer<QueryContext<EntityT>>>
	m_entity_provider;
};


}

#endif /* PROVIDERS_H_ */
