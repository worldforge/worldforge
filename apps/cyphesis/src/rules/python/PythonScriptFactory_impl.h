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


#ifndef RULESETS_PYTHON_SCRIPT_FACTORY_IMPL_H
#define RULESETS_PYTHON_SCRIPT_FACTORY_IMPL_H

#include "PythonScriptFactory.h"

#include "PythonWrapper_impl.h"
#include "pythonbase/Python_Script_Utils.h"
#include "pythonbase/Python_API.h"



template<typename EntityT, typename ScriptObjectT>
PythonScriptFactory<EntityT, ScriptObjectT>::PythonScriptFactory(const std::string& p,
											const std::string& t) :
		PythonClass(p, t) {
}


template<typename EntityT, typename ScriptObjectT>
int PythonScriptFactory<EntityT, ScriptObjectT>::setup() {
	return load();
}

template<typename EntityT, typename ScriptObjectT>
const std::string& PythonScriptFactory<EntityT, ScriptObjectT>::package() const {
	return m_package;
}

template<typename EntityT, typename ScriptObjectT>
Py::Object PythonScriptFactory<EntityT, ScriptObjectT>::createScript(ScriptObjectT& entity) const {
	if (!this->m_class || this->m_class->isNull()) {
		return Py::Null();
	}
	auto wrapper = wrapPython(&entity);
	if (wrapper.isNull()) {
		return Py::Null();
	}
	try {
		PythonLogGuard logGuard([wrapper]() {
			return fmt::format("{}: ", wrapper.as_string());
		});

		return this->m_class->apply(Py::TupleN(wrapper));
	} catch (...) {
		spdlog::error("Error when creating script '{}.{}'.", this->m_package, this->m_type);
		if (PyErr_Occurred() != nullptr) {
			PyErr_Print();
		}
		return Py::None();
	}
}

template<typename EntityT, typename ScriptObjectT>
std::unique_ptr<Script<EntityT>> PythonScriptFactory<EntityT, ScriptObjectT>::createScriptWrapper(ScriptObjectT& entity) const {
	auto script = createScript(entity);
	if (!script.isNone() && !script.isNull()) {
		return std::make_unique<PythonWrapper<EntityT>>(script);
	} else {
		return {};
	}
}

template<typename EntityT, typename ScriptObjectT>
int PythonScriptFactory<EntityT, ScriptObjectT>::refreshClass() {
	return refresh();
}

#endif // RULESETS_PYTHON_SCRIPT_FACTORY_IMPL_H
