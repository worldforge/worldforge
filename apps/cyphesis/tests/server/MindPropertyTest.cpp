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

#include "server/ExternalMindsManager.h"
#include "server/PossessionAuthenticator.h"

#include "../PropertyCoverage.h"

#include "server/MindProperty.h"

int main() {
	PossessionAuthenticator possessionAuthenticator;
	ExternalMindsManager mindsManager(possessionAuthenticator);
	MindProperty ap;

	PropertyChecker<MindProperty> pc(ap);

	// Coverage is complete, but it wouldn't hurt to add some bad data here.

	pc.basicCoverage();

	// The is no code in operations.cpp to execute, but we need coverage.
	return 0;
}

// stubs

#include "../TestWorld.h"
#include "rules/ai/BaseMind.h"
#include "rules/python/PythonScriptFactory_impl.h"






#include "rules/ai/python/CyPy_BaseMind.h"

template<>
PythonScriptFactory<MemEntity, BaseMind>::PythonScriptFactory(const std::string& package,
																  const std::string& type) :
		PythonClass(package,
					type) {
}


int GetScriptDetails(const Atlas::Message::MapType& script,
					 const std::string& class_name,
					 const std::string& context,
					 std::string& script_package,
					 std::string& script_class) {
	return 0;
}
