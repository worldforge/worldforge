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

#ifndef TESTS_NULL_ENTITY_CREATOR_H
#define TESTS_NULL_ENTITY_CREATOR_H

#include <rules/simulation/EntityCreator.h>
#include <rules/simulation/LocatedEntity.h>
#include <string>

struct NullEntityCreator : public EntityCreator {
	Ref<LocatedEntity> newEntity(RouterId id, const std::string& type, const Atlas::Objects::Entity::RootEntity& attrs) const override {
		return {};
	}
};

#endif //TESTS_NULL_ENTITY_CREATOR_H
