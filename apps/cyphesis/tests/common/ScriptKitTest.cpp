// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#include "common/ScriptKit.h"
#include "rules/Script_impl.h"
#include "rules/simulation/python/CyPy_LocatedEntity_impl.h"

class LocatedEntity;

class TestScriptKit : public ScriptKit<LocatedEntity, CyPy_LocatedEntity> {
public:
	std::string m_package;

	const std::string& package() const override { return m_package; }

	std::unique_ptr<Script<LocatedEntity>> createScriptWrapper(CyPy_LocatedEntity& entity) const override { return nullptr; }

	int refreshClass() override { return 0; }
};

int main() {
	// The is no code in ScriptKit.cpp to execute, but we need coverage.
	{
		auto* sk = new TestScriptKit;

		delete sk;
	}
	return 0;
}
