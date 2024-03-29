/*
 Copyright (C) 2020 Erik Ogenvik

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

#ifndef TESTS_NULL_PROPERTY_MANAGER_H
#define TESTS_NULL_PROPERTY_MANAGER_H

#include "common/PropertyManager_impl.h"

template<typename EntityT>
struct NullPropertyManager : PropertyManager<EntityT> {

	virtual std::unique_ptr<PropertyCore<EntityT>> addProperty(const std::string& name) const {
		return {};
	}
};


#endif //TESTS_NULL_PROPERTY_MANAGER_H
