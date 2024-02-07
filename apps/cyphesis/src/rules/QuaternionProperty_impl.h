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
#include "QuaternionProperty.h"
#include "common/Property_impl.h"

#include <wfmath/atlasconv.h>

template <typename EntityT>
QuaternionProperty<EntityT>::QuaternionProperty(const QuaternionProperty& rhs) = default;

template <typename EntityT>
int QuaternionProperty<EntityT>::get(Atlas::Message::Element& val) const {
	if (m_data.isValid()) {
		val = m_data.toAtlas();
		return 0;
	}
	return 1;

}

template <typename EntityT>
void QuaternionProperty<EntityT>::set(const Atlas::Message::Element& val) {
	if (val.isList()) {
		m_data.fromAtlas(val.List());
	}
}

template <typename EntityT>
QuaternionProperty<EntityT>* QuaternionProperty<EntityT>::copy() const {
	return new QuaternionProperty(*this);
}

template <typename EntityT>
void QuaternionProperty<EntityT>::add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const {
	if (m_data.isValid()) {
		PropertyCore<EntityT>::add(key, ent);
	}
}

