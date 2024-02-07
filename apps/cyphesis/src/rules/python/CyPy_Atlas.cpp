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

#include "CyPy_Atlas.h"
#include "CyPy_Element.h"
#include "CyPy_Operation.h"
#include "CyPy_Oplist.h"
#include "CyPy_RootEntity.h"
#include "CyPy_Location.h"
#include "CyPy_Root.h"
#include "rules/simulation/ScriptUtils.h"

CyPy_Atlas::CyPy_Atlas() : ExtensionModule("atlas") {

	CyPy_ElementMap::init_type();
	CyPy_ElementList::init_type();
	CyPy_Root::init_type();
	CyPy_Operation::init_type();
	CyPy_Oplist::init_type();
	CyPy_RootEntity::init_type();


	initialize("Atlas bindings, allowing the Atlas library to be used from Python.");

	Py::Dict d(moduleDictionary());

	d["Operation"] = CyPy_Operation::type();
	d["Root"] = CyPy_Root::type();
	d["Oplist"] = CyPy_Oplist::type();
	d["Entity"] = CyPy_RootEntity::type();
	d["ElementList"] = CyPy_ElementList::type();
	d["ElementMap"] = CyPy_ElementMap::type();
}

std::string CyPy_Atlas::init() {
	PyImport_AppendInittab("atlas", []() {
		static auto module = new CyPy_Atlas();
		return module->module().ptr();
	});
	return "atlas";
}


HandlerResult
ScriptUtils::processScriptResult(const std::string& scriptName, const Py::Object& ret, OpVector& res, const std::string& attachedEntityId, std::function<std::string()> entityDescribeFn) {
	HandlerResult result = OPERATION_IGNORED;

	auto processPythonResultFn = [&](const Py::Object& pythonResult) {
		if (pythonResult.isLong()) {
			auto numRet = Py::Long(pythonResult).as_long();
			if (numRet == 0) {
				result = OPERATION_IGNORED;
			} else if (numRet == 1) {
				result = OPERATION_HANDLED;
			} else if (numRet == 2) {
				result = OPERATION_BLOCKED;
			} else {
				spdlog::error("Unrecognized return code {} for script '{}' attached to entity '{}'", numRet, scriptName, entityDescribeFn());
			}

		} else if (CyPy_Operation::check(pythonResult)) {
			auto operation = CyPy_Operation::value(pythonResult);
			assert(operation);
			//If nothing is set the operation is from the entity containing the usages.
			if (operation->isDefaultFrom()) {
				operation->setFrom(attachedEntityId);
			}
			res.push_back(std::move(operation));
		} else if (CyPy_Oplist::check(pythonResult)) {
			auto& o = CyPy_Oplist::value(pythonResult);
			for (auto& opRes: o) {
				//If nothing is set the operation is from the entity containing the usages.
				if (opRes->isDefaultFrom()) {
					opRes->setFrom(attachedEntityId);
				}
				res.push_back(opRes);
			}
		} else {
			spdlog::error("Python script \"{}\" returned an invalid "
						  "result.", scriptName);
		}
	};

	if (ret.isNone()) {
		cy_debug_print("Returned none")
	} else {
		//Check if it's a tuple and process it.
		if (ret.isTuple()) {
			for (auto item: Py::Tuple(ret)) {
				processPythonResultFn(item);
			}
		} else {
			processPythonResultFn(ret);
		}
	}

	return result;
}