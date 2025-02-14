/*
 Copyright (C) 2025 Erik Ogenvik

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

#include "rules/Location.h"
#include "rules/Modifier.h"
#include "modules/Ref.h"
#include "modules/ReferenceCounted.h"
#include "modules/Flags.h"

#include "PropertyBase.h"
#include "common/Router.h"
#include "common/log.h"
#include "common/Visibility.h"
#include "common/PropertyUtil.h"
#include "common/SynchedState.h"

#include <Atlas/Objects/Operation.h>

#include <sigc++/signal.h>

#include <any>

#include <set>
class Domain;

class LocatedEntity;

template<typename>
class Script;

template<typename>
class TypeNode;

template<typename, typename>
class Property;

/**
 * Stores a property. Since all properties are modifiable we need to handle modifications.
 */
struct ModifiableProperty {
	/**
	 * The property.
	 */
	std::unique_ptr<PropertyBase> property;

	/**
	 * An optional base value, in the case of modifiers being applied.
	 * If so, this value is the "base" value set on the property before any modifiers are applied.
	 */
	Atlas::Message::Element baseValue;

	/**
	 * A list of optionally modifiers to apply.
	 * If the list contains modifiers, they must be applied together with the baseValue whenever the property is set.
	 * If the list is empty, the baseValue can be ignored and the value can be fetched or set directly on the property.
	 */
	std::vector<std::pair<Modifier*, LocatedEntity*>> modifiers;
};

struct LocatedEntityState {
	/// Map of properties
	std::map<std::string, ModifiableProperty> m_properties;

	std::map<RouterId, std::set<std::pair<std::string, Modifier*>>> m_activeModifiers;

	/// Class of which this is an instance
	const TypeNode<LocatedEntity>* m_type;
};
