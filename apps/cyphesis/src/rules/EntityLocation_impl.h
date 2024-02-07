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
#pragma once

#include "EntityLocation.h"

template<typename EntityT>
EntityLocation<EntityT>::EntityLocation() = default;


template<typename EntityT>
EntityLocation<EntityT>::EntityLocation(Ref<EntityT> loc)
		: m_parent(std::move(loc)) {
}

template<typename EntityT>
EntityLocation<EntityT>::EntityLocation(Ref<EntityT> loc, const WFMath::Point<3>& pos)
		: m_parent(std::move(loc)), m_pos(pos) {
}

template<typename EntityT>
const WFMath::Point<3>& EntityLocation<EntityT>::pos() const {
	return m_pos;
}

template<typename EntityT>
bool EntityLocation<EntityT>::isValid() const {
	return (m_parent && m_pos.isValid());
}
