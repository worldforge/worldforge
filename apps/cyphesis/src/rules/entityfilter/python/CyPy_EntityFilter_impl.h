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

#pragma once

#include "CyPy_EntityFilter.h"
#include "rules/entityfilter/ProviderFactory_impl.h"
#include "rules/entityfilter/ParserDefinitions.h"
#include "pycxx/CXX/Objects.hxx"


template<typename EntityT>
CyPy_Filter<EntityT>::CyPy_Filter(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
		: WrapperBase<std::shared_ptr<EntityFilter::Filter<EntityT>>, CyPy_Filter<EntityT>>(self, args, kwds) {
	args.verify_length(1);
	auto filterString = verifyString(args.front());
	EntityFilter::ProviderFactory<EntityT> factory;
	try {
		this->m_value.reset(new EntityFilter::Filter<EntityT>(filterString, factory));
	} catch (const std::exception& e) {
		throw Py::TypeError(fmt::format("Error when parsing query: {}", e.what()));
	}
}

template<typename EntityT>
void CyPy_Filter<EntityT>::init_type() {
	CyPy_Filter<EntityT>::behaviors().name("Filter");
	CyPy_Filter<EntityT>::behaviors().doc("");

	CyPy_Filter<EntityT>::behaviors().supportStr();

	CyPy_Filter<EntityT>::behaviors().readyType();
}

template<typename EntityT>
CyPy_Filter<EntityT>::CyPy_Filter(Py::PythonClassInstance* self, std::shared_ptr<EntityFilter::Filter<EntityT>> value)
		: WrapperBase<std::shared_ptr<EntityFilter::Filter<EntityT>>, CyPy_Filter<EntityT>>(self, std::move(value)) {

}

template<typename EntityT>
Py::Object CyPy_Filter<EntityT>::str() {
	return Py::String(fmt::format("Entity filter: {}", this->m_value->getDeclaration()));
}

template<typename EntityT>
CyPy_EntityFilter<EntityT>::CyPy_EntityFilter()
		: Py::ExtensionModule<CyPy_EntityFilter<EntityT>>("entity_filter") {

	CyPy_Filter<EntityT>::init_type();


	CyPy_EntityFilter<EntityT>::initialize("Entity filtering");

	Py::Dict d(CyPy_EntityFilter<EntityT>::moduleDictionary());
	d["Filter"] = CyPy_Filter<EntityT>::type();
}

template<typename EntityT>
std::string CyPy_EntityFilter<EntityT>::init() {
	PyImport_AppendInittab("entity_filter", []() {
		static auto module = new CyPy_EntityFilter();
		return module->module().ptr();
	});
	return "entity_filter";
}
