// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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


#ifndef TESTS_TEST_PROPERTY_MANAGER_H
#define TESTS_TEST_PROPERTY_MANAGER_H

#include "common/PropertyManager_impl.h"
#include "common/Property_impl.h"
#include "common/PropertyFactory_impl.h"

#include <map>

template<typename>
class PropertyKit;

template<typename EntityT>
class TestPropertyManager : public PropertyManager<EntityT> {
public:
	TestPropertyManager();

	~TestPropertyManager() override;

	std::unique_ptr<PropertyCore<EntityT>> addProperty(const std::string& name) const override;

	void installPropertyFactory(const std::string&, std::unique_ptr<PropertyKit<EntityT>>);
};


template<typename EntityT>
inline TestPropertyManager<EntityT>::TestPropertyManager() = default;

template<typename EntityT>
inline TestPropertyManager<EntityT>::~TestPropertyManager() = default;

template<typename EntityT>
inline void TestPropertyManager<EntityT>::installPropertyFactory(const std::string& name,
																 std::unique_ptr<PropertyKit<EntityT>> factory) {
	this->m_propertyFactories.emplace(name, std::move(factory));
}

template<typename EntityT>
inline std::unique_ptr<PropertyCore<EntityT>> TestPropertyManager<EntityT>::addProperty(const std::string& name) const {
	auto I = this->m_propertyFactories.find(name);
	if (I == this->m_propertyFactories.end()) {
		return std::make_unique<SoftProperty<EntityT>>();
	} else {
		return I->second->newProperty();
	}
}


#endif // TESTS_TEST_PROPERTY_MANAGER_H
