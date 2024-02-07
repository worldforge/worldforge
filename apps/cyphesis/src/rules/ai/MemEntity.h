// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifndef RULESETS_MEM_ENTITY_H
#define RULESETS_MEM_ENTITY_H

#include "modules/ReferenceCounted.h"
#include "modules/Ref.h"
#include "common/Router.h"
#include "common/PropertyManager.h"
#include "common/log.h"
#include "common/PropertyUtil.h"
#include "rules/EntityLocation.h"

#include <sigc++/signal.h>

#include <chrono>
#include <any>
#include <set>


/// \brief Flag indicating entity has been destroyed
/// \ingroup EntityFlags
static const std::uint32_t mem_entity_destroyed = 1u << 4u;

/// \brief Flag indicating entity is asleep
/// \ingroup EntityFlags
/// Currently only used on BaseMind
static const std::uint32_t mem_entity_asleep = 1u << 8u;

/// \brief This class is used to represent entities inside MemMap used
/// by the mind of an AI.
///
/// It adds a flag to indicate if this entity is currently visible, and
/// a means of tracking when it was last seen, so garbage entities can
/// be cleaned up.
class MemEntity : public RouterIdentifiable, public ReferenceCounted {
public:
	struct PropertyEntry {
		/**
		 * The property.
		 */
		std::unique_ptr<PropertyCore<MemEntity>> property;

	};
protected:


	std::chrono::milliseconds m_lastSeen;

	std::unique_ptr<PropertyCore<MemEntity>> createProperty(const std::string& propertyName) const;


	std::map<std::string, PropertyEntry> m_properties;

	/// Sequence number
	int m_seq;

	void clearProperties();


public:
	explicit MemEntity(RouterId id, const TypeNode<MemEntity>* typeNode);

	~MemEntity() override = default;

	std::chrono::milliseconds lastSeen() const {
		return m_lastSeen;
	}

	void update(std::chrono::milliseconds d) {
		if (d >= m_lastSeen) {
			m_lastSeen = d;
		}
	}

	void destroy();

	void merge(const Atlas::Message::MapType& ent);


	/// \brief Adds a child to this entity.
	void addChild(MemEntity& childEntity);

	/// \brief Removes a child from this entity.
	void removeChild(MemEntity& childEntity);

	/// \brief Accessor for entity type property
	const TypeNode<MemEntity>* getType() const { return m_type; }

	std::chrono::milliseconds m_lastUpdated;

	const std::map<std::string, PropertyEntry>& getProperties() const { return m_properties; }

	PropertyCore<MemEntity>* setAttr(const std::string& name, const Atlas::Message::Element& modifier);

	void applyProperty(const std::string& name, PropertyCore<MemEntity>& prop);

	/// @brief Signal emitted whenever a property update is applied.
	///
	/// The first parameter is the name of the property, the second is the updated property.
	sigc::signal<void(const std::string&, const PropertyCore<MemEntity>&)> propertyApplied;

	/// \brief Signal emitted when this entity is removed from the server
	///
	/// Note that this is usually well before the object is actually deleted
	/// and marks the conceptual destruction of the concept this entity
	/// represents, not the destruction of this object.
	sigc::signal<void()> destroyed;

	/// \brief Check if this entity has a property with the given name
	///
	/// @param name Name of attribute to be checked
	/// @return true if this entity has an attribute with the name given
	/// false otherwise
	bool hasAttr(const std::string& name) const;

	/// \brief Get the value of an attribute
	///
	/// @param name Name of attribute to be retrieved
	/// @param attr Reference used to store value
	/// @return zero if this entity has an attribute with the name given
	/// nonzero otherwise
	int getAttr(const std::string& name,
				Atlas::Message::Element&) const;

	/// \brief Get the value of an attribute
	///
	/// @param name Name of attribute to be retrieved
	/// @return An optional containing the value.
	std::optional<Atlas::Message::Element> getAttr(const std::string& name) const;

	/// \brief Get the value of an attribute if it is the right type
	///
	/// @param name Name of attribute to be retrieved
	/// @param attr Reference used to store value
	/// @return zero if this entity has an attribute with the name given
	/// nonzero otherwise
	int getAttrType(const std::string& name,
					Atlas::Message::Element&,
					int type) const;

	/// \brief Get the value of an attribute if it is the right type
	///
	/// @param name Name of attribute to be retrieved
	/// @return An optional containing the value.
	std::optional<Atlas::Message::Element> getAttrType(const std::string& name, int type) const;

	std::string describeEntity() const;

	const PropertyCore<MemEntity>* getProperty(const std::string& name) const;

	void setType(TypeNode<MemEntity>* type) {
		m_type = type;
	}

/// \brief Get a property that is required to of a given type.
	template<class PropertyT>
	const PropertyT* getPropertyClass(const std::string& name) const {
		const auto* p = getProperty(name);
		if (p != nullptr) {
			return dynamic_cast<const PropertyT*>(p);
		}
		return nullptr;
	}

	/// \brief Get a property that is required to of a given type.
	///
	/// The specified class must present the "property_name" trait.
	template<class PropertyT>
	const PropertyT* getPropertyClassFixed() const {
		return this->getPropertyClass<PropertyT>(PropertyT::property_name);
	}

	/// \brief Get a property that is a generic property of a given type
	template<typename T>
	const Property<T, MemEntity>* getPropertyType(const std::string& name) const {
		const auto* p = getProperty(name);
		if (p != nullptr) {
			return dynamic_cast<const Property<T, MemEntity>*>(p);
		}
		return nullptr;
	}

/// \brief Require that a property of a given type is set.
	///
	/// If the property is not set on the Entity instance, but has a class
	/// default, the default is copied to the instance, and a pointer is
	/// returned if it is a property of the right type. If it does not
	/// exist, or is not of the right type, a new property is created of
	/// the right type, and installed on the Entity instance.
	template<class PropertyT>
	PropertyT& requirePropertyClass(const std::string& name,
									const Atlas::Message::Element& def_val
									= Atlas::Message::Element()) {
		PropertyCore<MemEntity>* p = nullptr;
		auto I = m_properties.find(name);
		if (I != m_properties.end()) {
			p = I->second.property.get();
		}
		PropertyT* sp = nullptr;
		if (p != nullptr) {
			sp = dynamic_cast<PropertyT*>(p);
		}
		if (sp == nullptr) {
			// If it is not of the right type, delete it and a new
			// one of the right type will be inserted.
			sp = new PropertyT;
			sp->flags().addFlags(PropertyUtil::flagsForPropertyName(name));
			m_properties[name].property.reset(sp);
			sp->install(*this, name);
			if (p != nullptr) {
				spdlog::warn("Property {} on entity with id {} "
							 "reinstalled with new class."
							 "This might cause instability. Make sure that all "
							 "properties are properly installed in "
							 "CorePropertyManager.", name, getId());
				Atlas::Message::Element val;
				if (p->get(val)) {
					sp->set(val);
				}
			} else if (!def_val.isNone()) {
				sp->set(def_val);
			}
			sp->apply(*this);
		}
		return *sp;
	}

	/// \brief Require that a property of a given type is set, relying on the "property_name" trait.
	///
	/// If the property is not set on the Entity instance, but has a class
	/// default, the default is copied to the instance, and a pointer is
	/// returned if it is a property of the right type. If it does not
	/// exist, or is not of the right type, a new property is created of
	/// the right type, and installed on the Entity instance.
	///
	/// The specified class must present the "property_name" trait.
	template<class PropertyT>
	PropertyT& requirePropertyClassFixed(const Atlas::Message::Element& def_val = Atlas::Message::Element()) {
		return this->requirePropertyClass<PropertyT>(PropertyT::property_name, def_val);
	}

	/// \brief Accessor for sequence number
	int getSeq() const { return m_seq; }

	void increaseSequenceNumber() {
		m_seq++;
	}

	/**
	 * This is needed for the Filter system to work. Not sure if we want to implement it for MemEntities though.
	 * Perhaps there's a need?
	 * Or should we make the Filter system more modular, so it's only available for LocatedEntity?
	 * Or we could implement a more generic "distance to" system which can be used?
	 * @param entityLocation
	 * @param extraReach
	 * @return
	 */
	inline bool canReach(const EntityLocation<MemEntity>& entityLocation, float extraReach = 0) const {
		return false;
	}

	/// Flags indicating entity behaviour
	Flags m_flags;

	const TypeNode<MemEntity>* m_type;


	MemEntity* m_parent;
	/// List of entities which use this as ref
	std::set<Ref<MemEntity>> m_contains;

	inline const std::set<Ref<MemEntity>>* containsAsPtr() const {
		return &m_contains;
	}

	inline std::set<Ref<MemEntity>>* containsAsPtr() {
		return &m_contains;
	}

	// A representation of this instance used by the scripting system. This is opaque to this class.
	std::any m_scriptEntity;
};

std::ostream& operator<<(std::ostream& s, const MemEntity& d);


#endif // RULESETS_MEM_ENTITY_H
