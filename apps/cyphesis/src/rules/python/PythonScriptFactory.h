// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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


#ifndef RULESETS_PYTHON_SCRIPT_FACTORY_H
#define RULESETS_PYTHON_SCRIPT_FACTORY_H

#include "pycxx/CXX/Objects.hxx"
#include "pythonbase/PythonClass.h"

#include "common/ScriptKit.h"
#include "PythonWrapper.h"

/// \brief Factory implementation for creating python script objects to attach
/// to in game objects.
template<typename EntityT, typename ScriptObjectT>
class PythonScriptFactory : public ScriptKit<EntityT, ScriptObjectT>, private PythonClass {
public:
	PythonScriptFactory(const std::string& package, const std::string& type);

	~PythonScriptFactory() override = default;

	int setup() override;

	const std::string& package() const override;

	int refreshClass() override;

	Py::Object createScript(ScriptObjectT& entity) const;

	std::unique_ptr<Script<EntityT>> createScriptWrapper(ScriptObjectT& entity) const override;


};

#endif // RULESETS_PYTHON_SCRIPT_FACTORY_H
