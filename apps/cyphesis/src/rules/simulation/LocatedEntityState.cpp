/*
 Copyright (C) 2025 Erik Ogenvik

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

#include "LocatedEntityState.h"


#include "Domain.h"

#include "rules/Script.h"
#include "AtlasProperties.h"

#include "common/TypeNode.h"
#include "common/operations/Update.h"
#include "common/SynchedState.h"
#include "common/debug.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Anonymous.h>
#include <wfmath/atlasconv.h>

#include <memory>
#include <sstream>
#include <common/Link.h>
#include <common/Monitors.h>
#include "common/Variable.h"
#include <common/operations/Thought.h>
#include <rules/PhysicalProperties.h>
#include "common/SynchedState_impl.h"

#include "BaseWorld.h"
#include "ModeDataProperty.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

/// \brief Copy attributes into an Atlas entity
///
/// @param ent Atlas entity this entity should be copied into
void LocatedEntityState::addToEntity(const RootEntity& ent) const {
	// We need to have a list of keys to pull from attributes.
	for (auto& entry: m_properties) {
		entry.second.property->add(entry.first, ent);
	}

	ent->setStamp(m_seq);
	if (m_type != nullptr) {
		ent->setParent(m_type->name());
	}
	ent->setObjtype("obj");
}
HandlerResult LocatedEntityState::callDelegate(const std::string& name,
										  const Operation& op,
										  OpVector& res) {
	PropertyBase* p = nullptr;
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		p = I->second.property.get();
	} else if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			p = J->second.get();
		}
	}
	if (p != nullptr) {
		return p->operation(*this, op, res);
	}
	return OPERATION_IGNORED;
}

void LocatedEntityState::clearProperties() {
	if (m_type) {
		for (auto& entry: m_type->defaults()) {
			//Only remove if there's no instance specific property.
			if (!m_properties.contains(entry.first)) {
				entry.second->remove(*this, entry.first);
			}
		}
	}

	for (auto& entry: m_properties) {
		entry.second.property->remove(*this, entry.first);
	}
}

bool LocatedEntityState::hasAttr(const std::string& name) const {
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		return true;
	}
	if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			return true;
		}
	}
	return false;
}

int LocatedEntityState::getAttr(const std::string& name,
						   Element& attr) const {
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		return I->second.property->get(attr);
	}
	if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			return J->second->get(attr);
		}
	}
	return -1;
}

std::optional<Atlas::Message::Element> LocatedEntityState::getAttr(const std::string& name) const {
	//The idea here is to gradually move away from the C style method call to a more C++ based one with std::optional (and even std::optional).
	Atlas::Message::Element attr;
	auto result = getAttr(name, attr);
	if (result == 0) {
		return attr;
	} else {
		return {};
	}
}


int LocatedEntityState::getAttrType(const std::string& name,
							   Element& attr,
							   int type) const {
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		return I->second.property->get(attr) || (attr.getType() == type ? 0 : 1);
	}
	if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			return J->second->get(attr) || (attr.getType() == type ? 0 : 1);
		}
	}
	return -1;
}

std::optional<Atlas::Message::Element> LocatedEntityState::getAttrType(const std::string& name, int type) const {
	//The idea here is to gradually move away from the C style method call to a more C++ based one with std::optional (and even std::optional).

	Atlas::Message::Element attr;
	auto result = getAttrType(name, attr, type);
	if (result == 0) {
		return attr;
	} else {
		return {};
	}
}

PropertyBase* LocatedEntityState::setAttrValue(const std::string& name, Element attr) {
	auto parsedPropertyName = PropertyUtil::parsePropertyModification(name);
	return setAttr(parsedPropertyName.second, Modifier::createModifier(parsedPropertyName.first, std::move(attr)).get());
}

PropertyBase* LocatedEntityState::setAttr(const std::string& name, const Modifier* modifier) {
	if (!PropertyUtil::isValidName(name)) {
		spdlog::warn("Tried to add a property '{}' to entity '{}', which has an invalid name.", name, describeEntity());
		return nullptr;
	}

	bool propNeedsInstalling = false;
	PropertyBase* prop;
	Atlas::Message::Element attr;
	// If it is an existing property, just update the value.
	auto I = m_properties.find(name);
	if (I == m_properties.end() || !I->second.property) {
		//Install a new property.
		std::map<std::string, std::unique_ptr<PropertyBase>>::const_iterator J;
		if (m_type && (J = m_type->defaults().find(name)) != m_type->defaults().end()) {
			prop = J->second->copy();
			m_properties[name].property.reset(prop);
		} else {
			// This is an entirely new property, not just a modification of
			// one in defaults, so we need to install it to this Entity.
			auto newProp = createProperty(name);
			prop = newProp.get();
			m_properties[name].property = std::move(newProp);
			propNeedsInstalling = true;
		}
	} else {
		prop = I->second.property.get();
	}

	if (I != m_properties.end() && !I->second.modifiers.empty()) {
		ModifiableProperty& modifiableProperty = I->second;
		//Should we apply any modifiers?
		//If the new value is default we can just apply it directly.
		if (modifier) {
			if (modifier->getType() == ModifierType::Default) {
				modifiableProperty.baseValue = modifier->mValue;
				attr = modifier->mValue;
			} else {
				attr = modifiableProperty.baseValue;
				modifier->process(attr, modifiableProperty.baseValue);
			}
		}
		for (auto& modifierEntry: modifiableProperty.modifiers) {
			modifierEntry.first->process(attr, modifiableProperty.baseValue);
		}
	} else {
		if (modifier) {
			if (modifier->getType() == ModifierType::Default) {
				attr = modifier->mValue;
			} else {
				prop->get(attr);
				modifier->process(attr, attr);
			}
		}
	}
	//By now we should always have a new prop.
	assert(prop != nullptr);
	prop->removeFlags(prop_flag_persistence_clean);
	prop->set(attr);

	//We deferred any installation until after the prop has had its value set.
	if (propNeedsInstalling) {
		prop->install(*this, name);
	}

	// Allow the value to take effect.
	applyProperty(name, *prop);
	return prop;
}

void LocatedEntityState::addModifier(const std::string& propertyName, Modifier* modifier, LocatedEntity* affectingEntity) {
	if (hasFlags(entity_modifiers_not_allowed)) {
		return;
	}
	if (!PropertyUtil::isValidName(propertyName)) {
		spdlog::warn("Tried to add a modifier for property '{}' to entity '{}', which has an invalid name.", propertyName, describeEntity());
		return;
	}

	m_activeModifiers[affectingEntity->m_id].emplace(propertyName, modifier);

	auto I = m_properties.find(propertyName);
	if (I != m_properties.end()) {
		if (I->second.property && I->second.property->hasFlags(prop_flag_modifiers_not_allowed)) {
			return;
		}
		//We had a local value, check if there are any modifiers.
		if (I->second.modifiers.empty()) {
			//Set the base value from the current
			I->second.property->get(I->second.baseValue);
		}
		I->second.modifiers.emplace_back(modifier, affectingEntity);
		DefaultModifier defaultModifier(I->second.baseValue);
		setAttr(propertyName, &defaultModifier);
	} else {

		//Check if there's also a property in the type, and if so create a copy.
		bool had_property_in_type = false;
		if (m_type) {
			auto typeI = m_type->defaults().find(propertyName);
			if (typeI != m_type->defaults().end()) {
				if (typeI->second->hasFlags(entity_modifiers_not_allowed)) {
					return;
				}
				//We need to create a new modifier entry.
				auto& modifiableProperty = m_properties[propertyName];
				modifiableProperty.modifiers.emplace_back(modifier, affectingEntity);
				//Copy the default value.
				typeI->second->get(modifiableProperty.baseValue);
				DefaultModifier defaultModifier(modifiableProperty.baseValue);
				//Apply the new value
				setAttr(propertyName, &defaultModifier);
				had_property_in_type = true;
			}
		}
		if (!had_property_in_type) {
			//We need to create a new modifier entry with a new property.
			auto& modifiableProperty = m_properties[propertyName];
			modifiableProperty.modifiers.emplace_back(modifier, affectingEntity);
			//Apply the new value
			setAttr(propertyName, nullptr);
		}
	}
	enqueueUpdateOp();
}

void LocatedEntityState::removeModifier(const std::string& propertyName, Modifier* modifier) {
	for (auto& entry: m_activeModifiers) {
		auto result = entry.second.erase(std::make_pair(propertyName, modifier));
		if (result > 0) {
			break;
		}
	}

	auto propertyI = m_properties.find(propertyName);
	if (propertyI == m_properties.end()) {
		spdlog::warn("Tried to remove modifier from property {} which couldn't be found.", propertyName);
		return;
	}

	auto& modifiers = propertyI->second.modifiers;
	for (auto I = modifiers.begin(); I != modifiers.end(); ++I) {
		if (modifier == I->first) {
			modifiers.erase(I);
			//FIXME: If there's no base value we should remove the property, but there's no support in the storage manager for that yet.
			setAttrValue(propertyName, propertyI->second.baseValue);

			enqueueUpdateOp();

			return;
		}
	}
}

/// \brief Get the property object for a given attribute
///
/// @param name name of the attribute for which the property is required.
/// @return a pointer to the property, or zero if the attributes does
/// not exist, or is not stored using a property object.
const PropertyBase* LocatedEntityState::getProperty(const std::string& name) const {
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		return I->second.property.get();
	}
	if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			return J->second.get();
		}
	}
	return nullptr;
}

PropertyBase* LocatedEntityState::modProperty(const std::string& name, const Atlas::Message::Element& def_val) {
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		return I->second.property.get();
	}
	if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			// We have a default for this property. Create a new instance
			// property with the same value.
			PropertyBase* new_prop = J->second->copy();
			if (!def_val.isNone()) {
				new_prop->set(def_val);
			}
			J->second->remove(*this, name);
			new_prop->removeFlags(prop_flag_class);
			m_properties[name].property.reset(new_prop);
			new_prop->install(*this, name);
			applyProperty(name, *new_prop);
			return new_prop;
		}
	}
	return nullptr;
}

PropertyBase* LocatedEntityState::setProperty(const std::string& name,
										 std::unique_ptr<PropertyBase> prop) {
	auto& installedProp = m_properties[name];
	installedProp.property = std::move(prop);;
	installedProp.property->install(*this, name);
	return installedProp.property.get();
}