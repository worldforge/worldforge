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

#pragma once


#include "PropertyFactory.h"
#include "Property_impl.h"

template<typename T, typename EntityT>
std::unique_ptr<PropertyCore<EntityT>> PropertyFactory<T, EntityT>::newProperty() {
	auto prop = std::make_unique<T>();
	prop->addFlags(m_flags);
	return prop;
}

template<typename T, typename EntityT>
std::unique_ptr<PropertyKit<EntityT>> PropertyFactory<T, EntityT>::duplicateFactory() const {
	auto copy = std::make_unique<PropertyFactory<T, EntityT>>();
	copy->m_flags = m_flags;
	return copy;
}

