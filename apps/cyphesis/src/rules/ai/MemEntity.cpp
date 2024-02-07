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


#include "rules/BBoxProperty.h"
#include "rules/ScaleProperty.h"
#include "rules/EntityLocation_impl.h"

#include <utility>
#include "MemEntity.h"
#include "common/TypeNode.h"
#include "common/PropertyManager.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

static const bool debug_flag = false;

MemEntity::MemEntity(RouterId id, const TypeNode<MemEntity>* typeNode) :
		RouterIdentifiable(std::move(id)),
		m_lastSeen(0),
		m_seq(0),
		m_lastUpdated(0),
		m_flags(0),
		m_type(typeNode),
		m_parent(nullptr) {
}

std::unique_ptr<PropertyCore<MemEntity>> MemEntity::createProperty(const std::string& propertyName) const {
//    if (propertyName == ReadPositionProperty::property_name) {
//        return std::make_unique<ReadPositionProperty>(const_cast<WFMath::Point<3>&>(m_transform.pos));
//    }
//    if (propertyName == ReadBboxProperty::property_name) {
//        return std::make_unique<ReadBboxProperty>(const_cast<WFMath::AxisBox<3>&>(m_bbox));
//    }
//    if (propertyName == ReadOrientationProperty::property_name) {
//        return std::make_unique<ReadOrientationProperty>(const_cast<WFMath::Quaternion&>(m_transform.orientation));
//    }
//    if (propertyName == ReadVelocityProperty::property_name) {
//        return std::make_unique<ReadVelocityProperty>(const_cast<MemEntity&>(*this));
//    }
//    if (propertyName == ReadAngularProperty::property_name) {
//        return std::make_unique<ReadAngularProperty>(const_cast<MemEntity&>(*this));
//    }
	return PropertyManager<MemEntity>::instance().addProperty(propertyName);
}


void MemEntity::addChild(MemEntity& childEntity) {
	m_contains.insert(&childEntity);
	childEntity.m_parent = this;
}

void MemEntity::removeChild(MemEntity& childEntity) {
	m_contains.erase(&childEntity);
}

void MemEntity::destroy() {
	m_scriptEntity.reset();

	if (m_parent) {
		m_parent->removeChild(*this);
		m_parent = nullptr;
	}

	clearProperties();

	m_flags.addFlags(mem_entity_destroyed);
	destroyed.emit();

	auto containsCopy = m_contains;
	for (auto& child_ent: containsCopy) {
		child_ent->destroy();
	}

}

void MemEntity::merge(const MapType& ent) {
	for (auto& entry: ent) {
		const std::string& key = entry.first;
		if (key.empty()) {
			continue;
		}
		setAttr(key, entry.second);
	}
}

PropertyCore<MemEntity>* MemEntity::setAttr(const std::string& name, const Atlas::Message::Element& attr) {

	bool propNeedsInstalling = false;
	PropertyCore<MemEntity>* prop;
	// If it is an existing property, just update the value.
	auto I = m_properties.find(name);
	if (I == m_properties.end() || !I->second.property) {
		//Install a new property.
		std::map<std::string, std::unique_ptr<PropertyCore<MemEntity>>>::const_iterator J;
		if (m_type && (J = m_type->defaults().find(name)) != m_type->defaults().end()) {
			prop = J->second->copy();
			assert(prop != nullptr);
			m_properties[name].property.reset(prop);
		} else {
			// This is an entirely new property, not just a modification of
			// one in defaults, so we need to install it to this Entity.
			auto newProp = createProperty(name);
			assert(newProp != nullptr);
			prop = newProp.get();
			m_properties[name].property = std::move(newProp);
			propNeedsInstalling = true;
		}
	} else {
		prop = I->second.property.get();
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


void MemEntity::applyProperty(const std::string& name, PropertyCore<MemEntity>& prop) {
	// Allow the value to take effect.
	prop.apply(*this);
	prop.addFlags(prop_flag_unsent);
	propertyApplied(name, prop);
}

bool MemEntity::hasAttr(const std::string& name) const {
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

int MemEntity::getAttr(const std::string& name,
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

std::optional<Atlas::Message::Element> MemEntity::getAttr(const std::string& name) const {
	//The idea here is to gradually move away from the C style method call to a more C++ based one with std::optional (and even std::optional).
	Atlas::Message::Element attr;
	auto result = getAttr(name, attr);
	if (result == 0) {
		return attr;
	} else {
		return {};
	}
}


int MemEntity::getAttrType(const std::string& name,
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

std::optional<Atlas::Message::Element> MemEntity::getAttrType(const std::string& name, int type) const {
	//The idea here is to gradually move away from the C style method call to a more C++ based one with std::optional (and even std::optional).

	Atlas::Message::Element attr;
	auto result = getAttrType(name, attr, type);
	if (result == 0) {
		return attr;
	} else {
		return {};
	}
}

void MemEntity::clearProperties() {
	if (m_type) {
		for (auto& entry: m_type->defaults()) {
			//Only remove if there's no instance specific property.
			if (m_properties.find(entry.first) == m_properties.end()) {
				entry.second->remove(*this, entry.first);
			}
		}
	}

	for (auto& entry: m_properties) {
		entry.second.property->remove(*this, entry.first);
	}
	m_properties.clear();
}

std::string MemEntity::describeEntity() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}


const PropertyCore<MemEntity>* MemEntity::getProperty(const std::string& name) const {
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

std::ostream& operator<<(std::ostream& s, const MemEntity& d) {
	auto name = d.getAttrType("name", Element::TYPE_STRING);
	s << d.getId();
	if (d.m_type) {
		s << "(" << d.m_type->name();
		if (name) {
			s << ",'" << name->String() << "'";
		}
		s << ")";
	} else {
		if (name) {
			s << "('" << name->String() << "')";
		}
	}
	return s;
}
