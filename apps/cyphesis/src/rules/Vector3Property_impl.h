// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2016 Erik Ogenvik
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

#include "Vector3Property.h"
#include "common/Property_impl.h"

#include <wfmath/atlasconv.h>
#include "common/log.h"

template <typename EntityT>
Vector3Property<EntityT>::Vector3Property(const Vector3Property& rhs) = default;

template <typename EntityT>
int Vector3Property<EntityT>::get(Atlas::Message::Element& val) const {
	if (m_data.isValid()) {
		val = m_data.toAtlas();
		return 0;
	}
	return 1;

}

template <typename EntityT>
void Vector3Property<EntityT>::set(const Atlas::Message::Element& val) {
	if (val.isList()) {
		try {
			m_data.fromAtlas(val.List());
		} catch (...) {
			m_data = {};
			spdlog::error("Vector3Property::set: Data was not in format which could be parsed into 3d vector.");
		}
	} else if (val.isNone()) {
		m_data = {};
	} else {
		m_data = {};
		spdlog::error("Vector3Property::set: Data was not in format which could be parsed into 3d vector.");
	}
}

template <typename EntityT>
Vector3Property<EntityT>* Vector3Property<EntityT>::copy() const {
	return new Vector3Property(*this);
}

template <typename EntityT>
void Vector3Property<EntityT>::add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const {
	if (m_data.isValid()) {
		PropertyCore<EntityT>::add(key, ent);
	}
}
