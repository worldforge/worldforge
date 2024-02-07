#include <memory>

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

#include "SimpleTypeStore.h"
#include "common/log.h"
#include "common/TypeNode_impl.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Factories.h>
#include <common/Property.h>
#include "common/custom_impl.h"
#include "common/AtlasFactories.h"

const TypeNode<MemEntity>* SimpleTypeStore::getType(const std::string& parent) const {
	auto I = m_types.find(parent);
	if (I == m_types.end()) {
		return nullptr;
	}
	return I->second.get();
}


TypeNode<MemEntity>* SimpleTypeStore::addChild(const Atlas::Objects::Root& obj) {
	assert(obj.isValid());
	TypeNode<MemEntity>* parentNode = nullptr;

	const std::string& child = obj->getId();
	std::string parent;
	if (!obj->isDefaultParent()) {
		parent = obj->getParent();
	}

	if (!parent.empty()) {
		auto I = m_types.find(parent);
		if (I == m_types.end()) {
			spdlog::error("Installing {} \"{}\" "
						  "which has unknown parent \"{}\".",
						  obj->getObjtype(), child, parent);
			return nullptr;
		}
		parentNode = I->second.get();
	}

	auto I = m_types.find(child);
	if (I != m_types.end()) {

		auto existingParent = I->second->parent();
		spdlog::error("Installing {} \"{}\"(parent \"{}\") "
					  "which was already installed as a {} with parent \"{}\"",
					  obj->getObjtype(), child, parent,
					  I->second->description(Visibility::PRIVATE)->getObjtype(),
					  existingParent ? existingParent->name() : "NON");
		return nullptr;
	}
	auto type = std::make_unique<TypeNode<MemEntity>>(child, obj);

	//First add all properties from parent type.
	if (parentNode) {
		for (auto& entry: parentNode->defaults()) {
			type->injectProperty(entry.first, std::unique_ptr<PropertyCore<MemEntity>>(entry.second->copy()));
		}
	}

	readAttributesIntoType(*type, obj);

	if (parentNode) {

		Atlas::Message::Element children(Atlas::Message::ListType(1, child));

		auto description = parentNode->description(Visibility::PRIVATE);

		if (description->copyAttr("children", children) == 0) {
			assert(children.isList());
			children.asList().emplace_back(child);
		}
		description->setAttr("children", children);
		parentNode->setDescription(description);

		type->setParent(parentNode);
	}


	auto result = m_types.emplace(child, std::move(type));
	return result.first->second.get();
}


void SimpleTypeStore::readAttributesIntoType(TypeNode<MemEntity>& type, const Atlas::Objects::Root& obj) {
	// Store the default attribute for entities create by this rule.
	if (obj->hasAttr("properties")) {
		auto propertiesElement = obj->getAttr("properties");
		if (propertiesElement.isMap()) {
			type.addProperties(propertiesElement.Map(), m_propertyManager);
		}
	}

}


size_t SimpleTypeStore::getTypeCount() const {
	return m_types.size();
}


SimpleTypeStore::SimpleTypeStore(const PropertyManager<MemEntity>& propertyManager)
		: m_propertyManager(propertyManager) {

	Atlas::Objects::Entity::Anonymous root_desc;

	root_desc->setObjtype("meta");
	root_desc->setId(Atlas::Objects::RootData::default_parent);

	auto root = std::make_unique<TypeNode<MemEntity>>(root_desc->getId(), root_desc);

	m_types.emplace("root", std::move(root));

	installStandardObjects(*this);
	installCustomOperations(*this);

}


Atlas::Objects::Factories& SimpleTypeStore::getFactories() {
	return AtlasFactories::factories;
}


const Atlas::Objects::Factories& SimpleTypeStore::getFactories() const {
	return AtlasFactories::factories;
}
