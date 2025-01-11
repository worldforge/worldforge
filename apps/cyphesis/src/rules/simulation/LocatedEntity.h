// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2007 Alistair Riddoch
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


#ifndef RULESETS_LOCATED_ENTITY_H
#define RULESETS_LOCATED_ENTITY_H


#include "rules/Location.h"
#include "rules/Modifier.h"
#include "modules/Ref.h"
#include "modules/ReferenceCounted.h"
#include "modules/Flags.h"

#include "PropertyBase.h"
#include "common/Router.h"
#include "common/log.h"
#include "common/Visibility.h"
#include "common/PropertyUtil.h"
#include "common/SynchedState.h"

#include <Atlas/Objects/Operation.h>

#include <sigc++/signal.h>

#include <any>

#include <set>

class Domain;

class LocatedEntity;

template<typename>
class Script;

template<typename>
class TypeNode;

template<typename, typename>
class Property;

/**
 * Stores a property. Since all properties are modifiable we need to handle modifications.
 */
struct ModifiableProperty {
	/**
	 * The property.
	 */
	std::unique_ptr<PropertyBase> property;

	/**
	 * An optional base value, in the case of modifiers being applied.
	 * If so, this value is the "base" value set on the property before any modifiers are applied.
	 */
	Atlas::Message::Element baseValue;

	/**
	 * A list of optionally modifiers to apply.
	 * If the list contains modifiers, they must be applied together with the baseValue whenever the property is set.
	 * If the list is empty, the baseValue can be ignored and the value can be fetched or set directly on the property.
	 */
	std::vector<std::pair<Modifier*, LocatedEntity*>> modifiers;
};

typedef std::set<Ref<LocatedEntity>> LocatedEntitySet;

/// \brief Flag indicating entity has been written to permanent store
/// \ingroup EntityFlags
static constexpr std::uint32_t entity_clean = 1u << 0u;
/// \brief Flag indicating entity POS has been written to permanent store
/// \ingroup EntityFlags
static constexpr std::uint32_t entity_pos_clean = 1u << 1u;
/// \brief Flag indicating entity ORIENT has been written to permanent store
/// \ingroup EntityFlags
static constexpr std::uint32_t entity_orient_clean = 1u << 2u;

static constexpr std::uint32_t entity_clean_mask = entity_clean |
											   entity_pos_clean |
											   entity_orient_clean;

/// \brief Flag indicating entity is perceptive
/// \ingroup EntityFlags
static constexpr std::uint32_t entity_perceptive = 1u << 3u;
/// \brief Flag indicating entity has been destroyed
/// \ingroup EntityFlags
static constexpr std::uint32_t entity_destroyed = 1u << 4u;
/// \brief Flag indicating entity has been queued for storage update
/// \ingroup EntityFlags
static constexpr std::uint32_t entity_queued = 1u << 5u;
/// \brief Flag indicaiting entity is ephemeral
/// \ingroup EntityFlags
static constexpr std::uint32_t entity_ephem = 1u << 6u;
/// \brief Flag indicating entity is visible
/// \ingroup EntityFlags
/// Currently only used on MemEntity
static constexpr std::uint32_t entity_visible = 1u << 7u;
/// \brief Flag indicating entity is asleep
/// \ingroup EntityFlags
/// Currently only used on BaseMind
static constexpr std::uint32_t entity_asleep = 1u << 8u;
/// \brief Flag indicating entity contains a Domain, used for movement and sights
/// \ingroup EntityFlags
static constexpr std::uint32_t entity_domain = 1u << 9u;

/// \brief Flag indicating entity has thoughts that needs to be persisted.
/// \ingroup EntityFlags
static constexpr std::uint32_t entity_dirty_thoughts = 1u << 10u;

/// \brief Flag indicating entity has a location which has been changed since last sent to clients,
/// and new location data should be sent on the next Update.
/// \ingroup EntityFlags
static constexpr std::uint32_t entity_dirty_location = 1u << 11u;

/**
 * The entity is an admin character, and has additional capabilities.
 */
static constexpr std::uint32_t entity_admin = 1u << 12u;

/**
 * The entity is stackable.
 */
static constexpr std::uint32_t entity_stacked = 1u << 13u;

/**
 * The entity should be visible when contained, even if it normally wouldn't be.
 * This is of use for things like fires or effects.
 * Note that any entity_visibility_protected or entity_visibility_private flags still should apply.
 */
static constexpr std::uint32_t entity_contained_visible = 1u << 14u;

/**
 * The entity won't allow any kind of modifiers to affect it.
 */
static constexpr std::uint32_t entity_modifiers_not_allowed = 1u << 15u;

/**
 * The entity can only be seen by its parent entity.
 * This is useful for adding entities to a character entity, which only the character entity should see.
 */
static constexpr std::uint32_t entity_visibility_protected = 1u << 16u;

/**
 * The entity can only be seen by the simulation, and admin entities.
 */
static constexpr std::uint32_t entity_visibility_private = 1u << 17u;

/**
 * The entity has a pending Update broadcast op queued, and thus doesn't need any more.
 */
static constexpr std::uint32_t entity_update_broadcast_queued = 1u << 18u;

struct EntityState {
	/// Map of properties
	std::map<std::string, ModifiableProperty> m_properties;

	std::map<RouterId, std::set<std::pair<std::string, Modifier*>>> m_activeModifiers;

	/// Class of which this is an instance
	const TypeNode<LocatedEntity>* m_type;
};

/// \brief This is the base class from which in-game and in-memory objects
/// inherit.
///
/// This class should not normally be instantiated directly.
/// This class provides hard-coded attributes that are common to all
/// in-game objects, and objects in the memory on an NPC.
/// It provides a physical location for the entity, and a contains list
/// which lists other entities which specify their location with reference to
/// this one. It also provides the script interface for handling operations
/// in scripts rather than in the C++ code.
class LocatedEntity : public Router, public ReferenceCounted, public AtlasDescribable, public ExternalRouter {
private:
	static std::set<std::string> s_immutable;

	static const std::set<std::string>& immutables();

protected:
	/// Map of properties
	std::map<std::string, ModifiableProperty> m_properties;

	std::map<RouterId, std::set<std::pair<std::string, Modifier*>>> m_activeModifiers;

	/// Sequence number
	int m_seq;

	/// Class of which this is an instance
	const TypeNode<LocatedEntity>* m_type;


    /// Map of delegate properties.
    std::multimap<int, std::string> m_delegates;

    /// A static map tracking the number of existing entities per type.
    /// A monitor by the name of "entity_count{type=*}" will be created
    /// per type.
    static SynchedState<std::map<const TypeNode<LocatedEntity>*, int>> s_monitorsMap;

    std::unique_ptr<Domain> m_domain;

	//"virtual" for testing, see if we can remove it
    virtual std::unique_ptr<PropertyBase> createProperty(const std::string& propertyName) const;

    void updateProperties(const Operation& op, OpVector& res);

    bool lookAtEntity(const Operation& op,
                      OpVector& res,
                      const LocatedEntity& watcher) const;

    void generateSightOp(const LocatedEntity& observingEntity,
                         const Operation& originalLookOp,
                         OpVector& res) const;

    void moveToNewLocation(const Ref<LocatedEntity>& new_loc,
                           OpVector& res,
                           const Domain* existingDomain,
                           const Point3D& newPos,
                           const Quaternion& newOrientation,
                           const Vector3D& newImpulseVelocity);

    void moveOurselves(const Operation& op, const Atlas::Objects::Entity::RootEntity& ent, OpVector& res);

    void moveOtherEntity(const Operation& op, const Atlas::Objects::Entity::RootEntity& ent, OpVector& res) const;
	/**
	 * Collects all observers of the child, i.e. all entities that are currently observing it.
	 * This method will walk upwards the entity chain.
	 * @param child The child entity that's being observed.
	 */
	void collectObserversForChild(const LocatedEntity& child, std::set<const LocatedEntity*>& receivers) const;

public:

	/// Flags indicating entity behaviour
	Flags m_flags;

	/// Scripts that are associated with this entity.
	std::vector<std::unique_ptr<Script<LocatedEntity>>> m_scripts;
	// Parent entity. This is a pointer instead of a ref to avoid circular references.
	// It's expected that the owner entity should synchronize both addition of a child and
	// setting the parent.
	LocatedEntity* m_parent;
	/// List of entities which use this as ref
	std::unique_ptr<LocatedEntitySet> m_contains;

    const std::set<Ref<LocatedEntity>>* containsAsPtr() const {
		return m_contains.get();
	}

	// A representation of this instance used by the scripting system. This is opaque to this class.
	std::any m_scriptEntity;

	std::vector<OperationsListener<LocatedEntity>*> m_listeners;

	void addToEntity(const Atlas::Objects::Entity::RootEntity&) const override;

	virtual void DeleteOperation(const Operation&, OpVector&);

	virtual void ImaginaryOperation(const Operation&, OpVector&) const;

	virtual void LookOperation(const Operation&, OpVector&) const;

	virtual void MoveOperation(const Operation&, OpVector&);

	virtual void SetOperation(const Operation&, OpVector&);

	virtual void TalkOperation(const Operation&, OpVector&) const;

	virtual void UpdateOperation(const Operation&, OpVector&);

	virtual void CreateOperation(const Operation& op, OpVector& res) const;

	void externalOperation(const Operation& op, Link&) override;

	void operation(const Operation&, OpVector&) override;

	HandlerResult callDelegate(const std::string&,
							   const Operation&,
							   OpVector&);

	/// \brief Find and call the handler for an operation
	///
	/// @param op The operation to be processed.
	/// @param res The result of the operation is returned here.
	void callOperation(const Operation&, OpVector&);

	Ref<LocatedEntity> createNewEntity(const Atlas::Objects::Entity::RootEntity& entity) const;

	Ref<LocatedEntity> createNewEntity(const Operation& op, OpVector& res) const;
	explicit LocatedEntity(RouterId id);

	~LocatedEntity() override;

	void clearProperties();

	/// \brief Check if this entity is flagged as perceptive
	bool isPerceptive() const { return m_flags.hasFlags(entity_perceptive); }

	/// \brief Check if this entity is flagged as destroyed
	bool isDestroyed() const { return m_flags.hasFlags(entity_destroyed); }

	bool isVisible() const { return m_flags.hasFlags(entity_visible); }

	/// \brief Accessor for flags
	const Flags& flags() const { return m_flags; }

	Flags& flags() { return m_flags; }

	void addFlags(std::uint32_t flags) {
		m_flags.addFlags(flags);
	}

	void removeFlags(std::uint32_t flags) {
		m_flags.removeFlags(flags);
	}

	bool hasFlags(std::uint32_t flags) const {
		return m_flags.hasFlags(flags);
	}

	/// \brief Accessor for sequence number
	int getSeq() const { return m_seq; }

	void increaseSequenceNumber() {
		m_seq++;
	}

	/// \brief Accessor for entity type property
	const TypeNode<LocatedEntity>* getType() const { return m_type; }

	/// \brief Accessor for properties
    const std::map<std::string, ModifiableProperty>& getProperties() const { return m_properties; }
    std::map<std::string, ModifiableProperty>& getProperties() { return m_properties; }

	const std::map<RouterId, std::set<std::pair<std::string, Modifier*>>>& getActiveModifiers() const {
		return m_activeModifiers;
	}

	/// \brief Set the value of the entity type property
	virtual void setType(const TypeNode<LocatedEntity>* t);


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
				Atlas::Message::Element& attr) const;

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
					Atlas::Message::Element& attr,
					int type) const;

	/// \brief Get the value of an attribute if it is the right type
	///
	/// @param name Name of attribute to be retrieved
	/// @return An optional containing the value.
	std::optional<Atlas::Message::Element> getAttrType(const std::string& name, int type) const;

	/// \brief Set the value of an attribute
	///
	/// @param name Name of attribute to be changed
	/// @param attr Value to be stored
	PropertyBase* setAttrValue(const std::string& name,
							   Atlas::Message::Element attr);

	PropertyBase* setAttr(const std::string& name,
						  const Modifier* modifier);

	const PropertyBase* getProperty(const std::string& name) const;

	PropertyBase* modProperty(const std::string& name, const Atlas::Message::Element& def_val = Atlas::Message::Element());

	/// \brief Set the property object for a given attribute
	///
	/// @param name name of the attribute for which the property is given
	/// @param prop the property object to be used
	PropertyBase* setProperty(const std::string& name, std::unique_ptr<PropertyBase> prop);

	 void installDelegate(int, const std::string&);

	 void removeDelegate(int, const std::string&);

	 void onContainered(const Ref<LocatedEntity>& oldLocation);

	 void onUpdated();

	 //"virtual" for testing, see if we can remove it
	 virtual void destroy();

	 Domain* getDomain();

	 const Domain* getDomain() const;

	 void setDomain(std::unique_ptr<Domain> domain);


	/// \brief Send an operation to the world for dispatch.
	///
	/// sendWorld() bypasses serialno assignment, so you must ensure
	/// that serialno is sorted. This allows client serialnos to get
	/// in, so that client gets correct useful refnos back.
	//"virtual" for testing, see if we can remove it
	virtual void sendWorld(Operation op);


	/// \brief Sends operations to the world for dispatch.
	///
	/// sendWorld() bypasses serialno assignment, so you must ensure
	/// that serialno is sorted. This allows client serialnos to get
	/// in, so that client gets correct useful refnos back.
	void sendWorld(OpVector& res);

	 void setScript(std::unique_ptr<Script<LocatedEntity>> script);

	void makeContainer();

	void changeContainer(const Ref<LocatedEntity>&);

	void merge(const Atlas::Message::MapType&);

	/// \brief Adds a child to this entity.
	 void addChild(LocatedEntity& childEntity);

	/// \brief Removes a child from this entity.
	 void removeChild(LocatedEntity& childEntity);

	 void addListener(OperationsListener<LocatedEntity>* listener);

	 void removeListener(const OperationsListener<LocatedEntity>* listener);

	/**
	 * Applies the property and set flags on both the property and the entity to mark them as unclean.
	 * @param name
	 * @param prop
	 */
	void applyProperty(const std::string& name, PropertyBase& prop);

	/**
	 * Applies a property where the property name is fixed (through the "property_name" type trait).
	 * @tparam T
	 * @param prop
	 */
	template<typename T>
	void applyProperty(T& prop) {
		applyProperty(T::property_name, prop);
	}

	/**
	 * Collects all entities that are observing this entity.
	 * @param observers A set which will be filled with observing entities.
	 */
	void collectObservers(std::set<const LocatedEntity*>& observers) const;

	void collectObserved(std::set<const LocatedEntity*>& observed) const;

	/**
	 * Broadcasts an op.
	 *
	 * If this entity has a domain, the op is broadcast to all observers in the domain.
	 * In addition, broadcastFromChild will be called on any parent entity, to make it broadcast to entities in the same domain, or in domains above.
	 * @param op
	 * @param res
	 */
	void broadcast(const Atlas::Objects::Operation::RootOperation& op, OpVector& res, Visibility visibility) const;

	/**
	 * Processes appearance and disappearance of this entity for other observing entities. This is done by matching the supplied list of entities that previously
	 * observed the entity. When called, a list of entities that are currently observing it will be created, and the two lists will be compared.
	 *
	 * Any new entity gets a "Appearance" op, and any old entity which wasn't present in the new list will get a "Disappearance" op.
	 * @param previousObserving
	 * @param res
	 */
	void processAppearDisappear(std::set<const LocatedEntity*> previousObserving, OpVector& res) const;

	/**
	* @brief Determines if this entity is visible to another entity.
	*
	* @param observer The other entity observing this entity, for which we want to determine visibility.
	* @return True if this entity is visible to another entity.
	*/
	bool isVisibleForOtherEntity(const LocatedEntity& observer) const;

	/**
	* @brief Determines if this entity can be reached by another entity (i.e. physically interacted with).
	*
	* @param entityLocation Where we want to reach.
	* @return True if this entity is reachable to another entity.
	*/
	bool canReach(const EntityLocation<LocatedEntity>& entityLocation, float extraReach = 0) const;

	void addModifier(const std::string& propertyName, Modifier* modifier, LocatedEntity* affectingEntity);

	void removeModifier(const std::string& propertyName, Modifier* modifier);

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
	const Property<T, LocatedEntity>* getPropertyType(const std::string& name) const {
		const auto* p = getProperty(name);
		if (p != nullptr) {
			return dynamic_cast<const Property<T, LocatedEntity>*>(p);
		}
		return nullptr;
	}

	/// \brief Get a property that is required to of a given type.
	template<class PropertyT>
	PropertyT* modPropertyClass(const std::string& name) {
		auto* p = modProperty(name);
		if (p != nullptr) {
			return dynamic_cast<PropertyT*>(p);
		}
		return nullptr;
	}

	/// \brief Get a property that is required to of a given type.
	///
	/// The specified class must present the "property_name" trait.
	template<class PropertyT>
	PropertyT* modPropertyClassFixed() {
		return this->modPropertyClass<PropertyT>(PropertyT::property_name);
	}

	/// \brief Get a modifiable property that is a generic property of a type
	///
	/// If the property is not set on the Entity instance, but has a class
	/// default, the default is copied to the instance, and a pointer is
	/// returned if it is a property of the right type.
	template<typename T>
	Property<T, LocatedEntity>* modPropertyType(const std::string& name) {
		auto* p = modProperty(name);
		if (p != nullptr) {
			return dynamic_cast<Property<T, LocatedEntity>*>(p);
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
		if (!PropertyUtil::isValidName(name)) {
			spdlog::warn("Tried to add property '{}' to entity '{}', which has an invalid name.", name, describeEntity());
			throw std::runtime_error(fmt::format("Tried to add property '{}' to entity '{}', which has an invalid name.", name, describeEntity()));
		}

		auto* p = modProperty(name, def_val);
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
							 "CorePropertyManager.", name, getIdAsString());
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

	/**
	 * Generates a string describing this entity, used for debugging.
	 *
	 * @return A string describing the entity in more detail.
	 */
	std::string describeEntity() const;

	/**
	 * Enqueues a new Update op if there's none already queued.
	 * @param res
	 */
	void enqueueUpdateOp(OpVector& res);

	/**
	 * Enqueues a new Update op if there's none already queued.
	 * This version sends it directly to the world.
	 */
	void enqueueUpdateOp();

	/// Signal indicating that this entity has been changed
	sigc::signal<void()> updated;

	/// Signal indicating that this entity has changed its LOC.
	/// First parameter is the old location.
	sigc::signal<void(const Ref<LocatedEntity>&)> containered;

	/// \brief Signal emitted when this entity is removed from the server
	///
	/// Note that this is usually well before the object is actually deleted
	/// and marks the conceptual destruction of the concept this entity
	/// represents, not the destruction of this object.
	sigc::signal<void()> destroyed;

	/// @brief Signal emitted whenever a property update is applied.
	///
	/// The first parameter is the name of the property, the second is the updated property.
	sigc::signal<void(const std::string&, const PropertyBase&)> propertyApplied;

	friend std::ostream& operator<<(std::ostream& s, const LocatedEntity& d);

};

std::ostream& operator<<(std::ostream& s, const LocatedEntity& d);


#endif // RULESETS_LOCATED_ENTITY_H
