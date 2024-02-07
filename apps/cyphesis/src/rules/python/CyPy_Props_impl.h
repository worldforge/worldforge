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

#include "CyPy_Props.h"
#include "CyPy_Element.h"

template<typename EntityT>
CyPy_Props<EntityT>::CyPy_Props(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
		:  WrapperBase<Ref<EntityT>, CyPy_Props<EntityT>>(self, args, kwds) {

}


template<typename EntityT>
CyPy_Props<EntityT>::CyPy_Props(Py::PythonClassInstance* self, Ref<EntityT> value)
		:  WrapperBase<Ref<EntityT>, CyPy_Props<EntityT>>(self, std::move(value)) {

}


template<typename EntityT>
void CyPy_Props<EntityT>::init_type() {
	CyPy_Props<EntityT>::behaviors().name("Properties");
	CyPy_Props<EntityT>::behaviors().doc("");

	CyPy_Props<EntityT>::behaviors().supportMappingType(Py::PythonType::support_mapping_subscript);
	CyPy_Props<EntityT>::behaviors().readyType();
}

template<typename EntityT>
Py::Object CyPy_Props<EntityT>::getattro(const Py::String& name) {
	auto nameStr = name.as_string();

	auto prop = this->m_value->getProperty(nameStr);
	if (prop) {
		Atlas::Message::Element element;
		// If this property is not set with a value, return none.
		if (prop->get(element) == 0) {
			if (element.isNone()) {
				return Py::None();
			} else {
				return CyPy_Element::wrap(element);
			}
		}
	}
	return Py::None();
}

template<typename EntityT>
int CyPy_Props<EntityT>::setattro(const Py::String& name, const Py::Object& attr) {
//	Atlas::Message::Element obj = CyPy_Element::asElement(attr);
//	this->m_value->setAttrValue(name.as_string(), std::move(obj));
	return 0;
}

template<typename EntityT>
Py::Object CyPy_Props<EntityT>::mapping_subscript(const Py::Object& key) {
	if (key.isString()) {
		return getattro(Py::String(key));
	}
	return Py::None();
}
