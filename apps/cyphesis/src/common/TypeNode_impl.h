// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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
#pragma once


#include "TypeNode.h"

#include "debug.h"
#include "Property_impl.h"
#include "PropertyManager_impl.h"
#include "log.h"

#include <set>
#include "common/PropertyUtil.h"


using Atlas::Message::MapType;

template<typename EntityT>
TypeNode<EntityT>::TypeNode(std::string name)
		: m_name(std::move(name)),
		  m_parent(nullptr) {
}

template<typename EntityT>
TypeNode<EntityT>::TypeNode(std::string name,
							const Atlas::Objects::Root& d)
		: m_name(std::move(name)),
		  m_parent(nullptr) {
	setDescription(d);
}

template<typename EntityT>
TypeNode<EntityT>::~TypeNode() = default;


template<typename EntityT>
void TypeNode<EntityT>::setDescription(const Atlas::Objects::Root& description) {
	//We need to split out any private and protected attributes
	m_privateDescription = description.copy();
	m_protectedDescription = description.copy();
	// Regular clients (protected and public) don't need to see children.
	// We want to allow for some mystery about the different kinds of entities in the world.
	m_protectedDescription->removeAttr("children");
	m_protectedDescription->removeAttr("attributes");
	m_publicDescription = description.copy();
	m_publicDescription->removeAttr("children");
	m_publicDescription->removeAttr("attributes");
	m_privateDescription->setAttr("access", "private");
	m_protectedDescription->setAttr("access", "protected");
	m_publicDescription->setAttr("access", "public");

}

template<typename EntityT>
TypeNode<EntityT>::PropertiesUpdate TypeNode<EntityT>::injectProperty(const std::string& name,
																	  std::unique_ptr<PropertyCore<EntityT>> prop) {
	TypeNode<EntityT>::PropertiesUpdate update;
	if (!PropertyUtil::isValidName(name)) {
		spdlog::warn("Tried to add a property '{}' to type '{}', which has an invalid name.", name, m_name);
		return update;
	}
	if (prop->hasFlags(prop_flag_instance)) {
		spdlog::warn("Tried to add a property '{}' to type '{}', which is forbidden since it's instance only.", name, m_name);
		return update;
	}

	auto* p = prop.get();
	auto existingI = m_defaults.find(name);
	if (existingI != m_defaults.end() && existingI->second.get() != prop.get()) {
		existingI->second = std::move(prop);
		update.changedProps.insert(name);
	} else {
		m_defaults.emplace(name, std::move(prop));
		update.newProps.insert(name);
	}

	auto add_attribute_fn = [&](Atlas::Objects::Root& description) {
		Atlas::Message::Element propertiesElement = Atlas::Message::MapType();
		if (description->hasAttr("properties")) {
			propertiesElement = description->getAttr("properties");
		}
		Atlas::Message::Element propertyElement;
		p->get(propertyElement);
		propertiesElement.Map()[name] = propertyElement;
		description->setAttr("properties", std::move(propertiesElement));
	};

	add_attribute_fn(m_privateDescription);
	if (!p->hasFlags(prop_flag_visibility_private)) {
		add_attribute_fn(m_protectedDescription);
	}
	if (!p->hasFlags(prop_flag_visibility_non_public)) {
		add_attribute_fn(m_publicDescription);
	}

	return update;

}

template<typename EntityT>
void TypeNode<EntityT>::addProperties(const MapType& attributes, const PropertyManager<EntityT>& propertyManager) {
	for (const auto& entry: attributes) {
		if (!PropertyUtil::isValidName(entry.first)) {
			spdlog::warn("Tried to add a property '{}' to type '{}', which has an invalid name.", entry.first, m_name);
			continue;
		}
		auto p = propertyManager.addProperty(entry.first);
		if (p->hasFlags(prop_flag_instance)) {
			spdlog::warn("Tried to add a property '{}' to type '{}', which is forbidden since it's instance only.", entry.first, m_name);
			continue;
		}
		assert(p != nullptr);
		p->set(entry.second);
		p->addFlags(prop_flag_class);
		p->install(*this, entry.first);
		m_defaults[entry.first] = std::move(p);
	}
}

template<typename EntityT>
TypeNode<EntityT>::PropertiesUpdate TypeNode<EntityT>::updateProperties(const MapType& attributes, const PropertyManager<EntityT>& propertyManager) {

	PropertiesUpdate propertiesUpdate;

	//Update the description
	Atlas::Message::MapType propertiesMapPrivate = m_privateDescription->hasAttr("properties") ? m_privateDescription->getAttr("properties").Map() : Atlas::Message::MapType{};
	Atlas::Message::MapType propertiesMapProtected = m_protectedDescription->hasAttr("properties") ? m_protectedDescription->getAttr("properties").Map() : Atlas::Message::MapType{};
	Atlas::Message::MapType propertiesMapPublic = m_publicDescription->hasAttr("properties") ? m_publicDescription->getAttr("properties").Map() : Atlas::Message::MapType{};

	// Discover the default attributes which are no longer
	// present after the update.
	for (auto& entry: m_defaults) {
		//Don't remove ephemeral attributes.
		if (attributes.find(entry.first) == attributes.end() && !entry.second->hasFlags(prop_flag_persistence_ephem)) {
			//cy_debug_print(entry.first << " removed")
			propertiesUpdate.removedProps.insert(entry.first);
			propertiesMapPrivate.erase(entry.first);
			propertiesMapProtected.erase(entry.first);
			propertiesMapPublic.erase(entry.first);
		}
	}

	// Remove the class properties for the default attributes that
	// no longer exist
	for (auto& entry: propertiesUpdate.removedProps) {
		auto M = m_defaults.find(entry);
		m_defaults.erase(M);
	}



	// Update the values of existing class properties, and add new class
	// properties for added default attributes.
	for (auto& entry: attributes) {
		if (!PropertyUtil::isValidName(entry.first)) {
			spdlog::warn("Tried to add a property '{}' to type '{}', which has an invalid name.", entry.first, m_name);
			continue;
		}

		auto I = m_defaults.find(entry.first);
		if (I == m_defaults.end()) {
			auto p = propertyManager.addProperty(entry.first);
			if (p->hasFlags(prop_flag_instance)) {
				spdlog::warn("Tried to add a property '{}' to type '{}', which is forbidden since it's instance only.", entry.first, m_name);
				continue;
			}
			p->addFlags(prop_flag_class);
			p->install(*this, entry.first);
			propertiesUpdate.newProps.emplace(entry.first);
			p->set(entry.second);

			propertiesMapPrivate[entry.first] = entry.second;
			if (!p->hasFlags(prop_flag_visibility_private)) {
				propertiesMapProtected[entry.first] = entry.second;
			}
			if (!p->hasFlags(prop_flag_visibility_non_public)) {
				propertiesMapPublic[entry.first] = entry.second;
			}
			m_defaults[entry.first] = std::move(p);
		} else {
			Atlas::Message::Element oldVal;
			auto& p = I->second;
			p->get(oldVal);
			if (oldVal != entry.second) {
				p->set(entry.second);
				propertiesUpdate.changedProps.emplace(entry.first);
			}
			propertiesMapPrivate[entry.first] = entry.second;
			if (!p->hasFlags(prop_flag_visibility_private)) {
				propertiesMapProtected[entry.first] = entry.second;
			}
			if (!p->hasFlags(prop_flag_visibility_non_public)) {
				propertiesMapPublic[entry.first] = entry.second;
			}
		}
	}

	m_privateDescription->setAttr("properties", std::move(propertiesMapPrivate));
	m_protectedDescription->setAttr("properties", std::move(propertiesMapProtected));
	m_publicDescription->setAttr("properties", std::move(propertiesMapPublic));

	return propertiesUpdate;
}

template<typename EntityT>
bool TypeNode<EntityT>::isTypeOf(const std::string& base_type) const {
	const TypeNode* node = this;
	do {
		if (node->name() == base_type) {
			return true;
		}
		node = node->parent();
	} while (node != nullptr);
	return false;
}

template<typename EntityT>
bool TypeNode<EntityT>::isTypeOf(const TypeNode* base_type) const {
	const TypeNode* node = this;
	do {
		if (node == base_type) {
			return true;
		}
		node = node->parent();
	} while (node != nullptr);
	return false;
}

template<typename EntityT>
Atlas::Objects::Root& TypeNode<EntityT>::description(Visibility visibility) {
	switch (visibility) {
		case Visibility::PROTECTED:
			return m_protectedDescription;
		case Visibility::PRIVATE:
			return m_privateDescription;
		case Visibility::PUBLIC:
		default:
			return m_publicDescription;
	}
}

template<typename EntityT>
const Atlas::Objects::Root& TypeNode<EntityT>::description(Visibility visibility) const {
	switch (visibility) {
		case Visibility::PROTECTED:
			return m_protectedDescription;
		case Visibility::PRIVATE:
			return m_privateDescription;
		case Visibility::PUBLIC:
		default:
			return m_publicDescription;
	}
}


