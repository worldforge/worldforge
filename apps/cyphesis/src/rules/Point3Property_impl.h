/*
 Copyright (C) 2021 Erik Ogenvik

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
#pragma once

#include "Point3Property.h"
#include "common/Property_impl.h"
#include <wfmath/atlasconv.h>
#include "common/log.h"


template <typename EntityT>
Point3Property<EntityT>::Point3Property(const Point3Property& rhs) = default;

template <typename EntityT>
int Point3Property<EntityT>::get(Atlas::Message::Element& val) const {
	if (m_data.isValid()) {
		val = m_data.toAtlas();
		return 0;
	}
	return 1;

}

template <typename EntityT>
void Point3Property<EntityT>::set(const Atlas::Message::Element& val) {
	if (val.isList()) {
		try {
			m_data.fromAtlas(val.List());
		} catch (...) {
			m_data = {};
			spdlog::error("Point3Property::set: Data was not in format which could be parsed into 3d point.");
		}
	} else if (val.isNone()) {
		m_data = {};
	} else {
		m_data = {};
		spdlog::error("Point3Property::set: Data was not in format which could be parsed into 3d point.");
	}
}

template <typename EntityT>
Point3Property<EntityT>* Point3Property<EntityT>::copy() const {
	return new Point3Property(*this);
}

template <typename EntityT>
void Point3Property<EntityT>::add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const {
	if (m_data.isValid()) {
		PropertyCore<EntityT>::add(key, ent);
	}
}
