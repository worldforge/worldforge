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

#include "ClientPropertyManager.h"

#include "rules/PhysicalProperties_impl.h"
#include "rules/ScaleProperty_impl.h"
//#include "rules/SolidProperty_impl.h"
#include "rules/BBoxProperty_impl.h"
#include "common/PropertyFactory_impl.h"
#include "common/PropertyManager_impl.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;

ClientPropertyManager::ClientPropertyManager() {
	installFactory(PositionProperty<MemEntity>::property_name, std::make_unique<PropertyFactory<PositionProperty<MemEntity>, MemEntity>>());
	installFactory(VelocityProperty<MemEntity>::property_name, std::make_unique<PropertyFactory<VelocityProperty<MemEntity>, MemEntity>>());
	installFactory(BBoxProperty<MemEntity>::property_name, std::make_unique<PropertyFactory<BBoxProperty<MemEntity>, MemEntity>>());
	installFactory(ScaleProperty<MemEntity>::property_name, std::make_unique<PropertyFactory<ScaleProperty<MemEntity>, MemEntity>>());
	installFactory(SolidProperty<MemEntity>::property_name, std::make_unique<PropertyFactory<SolidProperty<MemEntity>, MemEntity>>());
	installFactory(AngularVelocityProperty<MemEntity>::property_name, std::make_unique<PropertyFactory<AngularVelocityProperty<MemEntity>, MemEntity>>());
	installFactory(OrientationProperty<MemEntity>::property_name, std::make_unique<PropertyFactory<OrientationProperty<MemEntity>, MemEntity>>());
	installFactory("_propel", std::make_unique<PropertyFactory<Vector3Property<MemEntity>, MemEntity>>());
}

std::unique_ptr<PropertyCore<MemEntity>> ClientPropertyManager::addProperty(const std::string& name) const {
	auto I = m_propertyFactories.find(name);
	if (I == m_propertyFactories.end()) {
		return std::make_unique<SoftProperty<MemEntity>>();
	} else {
		return I->second->newProperty();
	}
}
