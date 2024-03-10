// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../../PropertyExerciser.h"

#include "rules/simulation/AreaProperty.h"
#include "rules/simulation/AtlasProperties.h"
#include "rules/simulation/CalendarProperty.h"
#include "rules/simulation/EntityProperty.h"
#include "rules/simulation/LineProperty.h"
#include "rules/simulation/TerrainProperty.h"

#include "rules/simulation/BaseWorld.h"
#include "common/log.h"
#include "common/Property_impl.h"
#include "common/TypeNode_impl.h"

#include "rules/Location_impl.h"

#include <Mercator/Terrain.h>

#include <cassert>
#include <Atlas/Objects/Factories.h>

using Atlas::Message::Element;

static const Element::Type TYPE_INT = Element::TYPE_INT;
static const Element::Type TYPE_FLOAT = Element::TYPE_FLOAT;
static const Element::Type TYPE_STRING = Element::TYPE_STRING;
static const Element::Type TYPE_PTR = Element::TYPE_PTR;
static const Element::Type TYPE_LIST = Element::TYPE_LIST;
static const Element::Type TYPE_MAP = Element::TYPE_MAP;

int main() {
	PropertyExerciser exerciser;

	{
		Property<int, LocatedEntity> test_property(0);
		assert(exerciser.exerciseProperty(test_property, TYPE_INT) == 0);
	}

	{
		Property<long, LocatedEntity> test_property(0);
		assert(exerciser.exerciseProperty(test_property, TYPE_INT) == 0);
	}

	{
		Property<float, LocatedEntity> test_property(0);
		assert(exerciser.exerciseProperty(test_property, TYPE_FLOAT) == 0);
	}

	{
		Property<double, LocatedEntity> test_property(0);
		assert(exerciser.exerciseProperty(test_property, TYPE_FLOAT) == 0);
	}

	{
		Property<std::string, LocatedEntity> test_property(0);
		assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
	}

	{
		AreaProperty test_property;
		assert(exerciser.exerciseProperty(test_property, TYPE_MAP) == 0);
	}

	{
		RouterId test_string_val(1);
		IdProperty test_property(test_string_val);
		assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
	}

	{
		NameProperty test_property(0);
		assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
	}

	{
		NameProperty test_property(0);
		assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
	}

	{
		LocatedEntitySet test_entityset_val;
		ContainsProperty test_property(test_entityset_val);
		assert(exerciser.exerciseProperty(test_property, TYPE_LIST) == 0);
	}

	{
		CalendarProperty test_property;
		assert(exerciser.exerciseProperty(test_property, TYPE_MAP) == 0);
	}

#if 0
	// FIXME This currently segfaults, as we give it pointers that are not
	// pointers to entities. This highlights the fact that we need to
	// protect property code from getting given pointers from outside.
	// Atlas-C++ must not allow pointers to come in from the network.
	{
		WeakEntityRef<LocatedEntity> test_entityref_data;
		EntityProperty test_property(test_entityref_data);
		assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
	}
#endif

	// FIXME Yay this throws!!! I found a bug with my foolish tests
	{
		LineProperty test_property;
		assert(exerciser.exerciseProperty(test_property, TYPE_LIST) == 0);
	}


	{
		TerrainProperty test_property;
		assert(exerciser.exerciseProperty(test_property, TYPE_LIST) == 0);
	}

	return 0;
}


#include "common/TypeNode_impl.h"
