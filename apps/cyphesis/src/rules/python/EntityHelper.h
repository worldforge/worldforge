/*
 Copyright (C) 2024 Erik Ogenvik

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

#include "pythonbase/WrapperBase.h"
#include "CyPy_Element.h"
#include <Atlas/Message/Element.h>

template<typename EntityT>
Py::Object wrapEntity(Ref<EntityT> le);


template<typename TValue>
Py::Object getPropertyFromFirstArg(TValue& value, const Py::Tuple& args, const std::function<Py::Object(const Atlas::Message::Element&, Py::Object)>& checkFn) {
	args.verify_length(1, 2);
	Py::Object defaultValue = Py::None();
	if (args.length() > 1) {
		defaultValue = args.getItem(1);
	}
	auto* prop = value.getProperty(verifyString(args.front()));
	if (prop) {
		Atlas::Message::Element element;
		prop->get(element);

		return checkFn(element, defaultValue);
	}

	return defaultValue;
}

template<typename TValue>
Py::Object hasPropertyFromFirstArg(TValue& value, const Py::Tuple& args, const std::function<bool(const Atlas::Message::Element&)>& checkFn) {

	args.verify_length(1);
	auto* prop = value.getProperty(verifyString(args.front()));
	if (prop) {
		Atlas::Message::Element element;
		prop->get(element);
		return Py::Boolean(checkFn(element));
	}

	return Py::False();
}


template<typename TValue>
Py::Object get_prop_num(TValue& value, const Py::Tuple& args) {
	return getPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
		if (element.isInt()) {
			return Py::Long(element.Int());
		} else if (element.isFloat()) {
			return Py::Float(element.Float());
		} else {
			return defaultValue;
		}
	});
}

template<typename TValue>
Py::Object has_prop_num(TValue& value, const Py::Tuple& args) {
	return hasPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element) {
		return element.isNum();
	});
}


template<typename TValue>
Py::Object get_prop_float(TValue& value, const Py::Tuple& args) {
	return getPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
		if (element.isFloat()) {
			return Py::Float(element.Float());
		} else {
			return defaultValue;
		}
	});
}

template<typename TValue>
Py::Object has_prop_float(TValue& value, const Py::Tuple& args) {
	return hasPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element) {
		return element.isFloat();
	});
}


template<typename TValue>
Py::Object get_prop_int(TValue& value, const Py::Tuple& args) {
	return getPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
		if (element.isInt()) {
			return Py::Long(element.Int());
		} else {
			return defaultValue;
		}
	});
}

template<typename TValue>
Py::Object has_prop_int(TValue& value, const Py::Tuple& args) {
	return hasPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element) {
		return element.isInt();
	});
}


template<typename TValue>
Py::Object get_prop_string(TValue& value, const Py::Tuple& args) {
	return getPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
		if (element.isString()) {
			return Py::String(element.String());
		} else {
			return defaultValue;
		}
	});
}

template<typename TValue>
Py::Object has_prop_string(TValue& value, const Py::Tuple& args) {
	return hasPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element) {
		return element.isString();
	});
}


template<typename TValue>
Py::Object get_prop_bool(TValue& value, const Py::Tuple& args) {
	return getPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
		if (element.isInt()) {
			return Py::Boolean(element.Int() != 0);
		} else {
			return defaultValue;
		}
	});
}

template<typename TValue>
Py::Object has_prop_bool(TValue& value, const Py::Tuple& args) {
	return has_prop_int(value, args);
}


template<typename TValue>
Py::Object get_prop_map(TValue& value, const Py::Tuple& args) {
	return getPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
		if (element.isMap()) {
			return CyPy_ElementMap::wrap(element.Map());
//            return Py::Dict(element.Map());
		} else {
			return defaultValue;
		}
	});
}

template<typename TValue>
Py::Object has_prop_map(TValue& value, const Py::Tuple& args) {
	return hasPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element) {
		return element.isMap();
	});
}


template<typename TValue>
Py::Object get_prop_list(TValue& value, const Py::Tuple& args) {
	return getPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element, Py::Object defaultValue) -> Py::Object {
		if (element.isList()) {
			return CyPy_ElementList::wrap(element.List());
//            return Py::Dict(element.Map());
		} else {
			return defaultValue;
		}
	});
}

template<typename TValue>
Py::Object has_prop_list(TValue& value, const Py::Tuple& args) {
	return hasPropertyFromFirstArg(value, args, [](const Atlas::Message::Element& element) {
		return element.isList();
	});
}
