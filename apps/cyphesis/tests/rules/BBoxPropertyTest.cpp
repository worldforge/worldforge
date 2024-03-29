// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "../PropertyCoverage.h"

#include "rules/BBoxProperty_impl.h"
#include "rules/ScaleProperty_impl.h"
#include "rules/Location_impl.h"
#include "../../src/rules/simulation/LocatedEntity.h"
using Atlas::Message::ListType;

int main()
{
    BBoxProperty<LocatedEntity> ap;

    PropertyChecker<BBoxProperty<LocatedEntity>> pc(ap);

    pc.testDataAppend(ListType(3, 1.f));
    pc.testDataAppend(ListType(6, 1.f));

    // Coverage is complete, but it wouldn't hurt to add some bad data here.

    pc.basicCoverage();

    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

// stubs

#include "../TestWorld.h"
