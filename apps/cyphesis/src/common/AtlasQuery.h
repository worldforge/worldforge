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

#ifndef ATLASQUERY_H
#define ATLASQUERY_H

#include <Atlas/Message/Element.h>

#include <string>
#include <functional>


/**
 * Provides utility methods for quickly finding Atlas elements within other Atlas Map elements.
 */
class AtlasQuery {

public:
	template<typename T>
	static void find(const Atlas::Message::Element& element, const std::string& name, const std::function<void(const T&)>& callback);

	template<typename T>
	static void find(const Atlas::Message::MapType& map, const std::string& name, const std::function<void(const T&)>& callback);
};

template<typename T>
 void AtlasQuery::find(const Atlas::Message::Element& element, const std::string& name, const std::function<void(const T&)>& callback) {
	if (element.isMap()) {
		find(element.Map(), name, callback);
	}
}


template<>
inline void AtlasQuery::find(const Atlas::Message::MapType& map, const std::string& name, const std::function<void(const Atlas::Message::MapType&)>& callback) {
	auto I = map.find(name);
	if (I != map.end()) {
		if (I->second.isMap()) {
			callback(I->second.Map());
		}
	}
}

template<>
inline void AtlasQuery::find(const Atlas::Message::MapType& map, const std::string& name, const std::function<void(const Atlas::Message::ListType&)>& callback) {
	auto I = map.find(name);
	if (I != map.end()) {
		if (I->second.isList()) {
			callback(I->second.List());
		}
	}
}

template<>
inline void AtlasQuery::find(const Atlas::Message::MapType& map, const std::string& name, const std::function<void(const Atlas::Message::IntType&)>& callback) {
	auto I = map.find(name);
	if (I != map.end()) {
		if (I->second.isInt()) {
			callback(I->second.Int());
		}
	}
}

template<>
inline void AtlasQuery::find(const Atlas::Message::MapType& map, const std::string& name, const std::function<void(const Atlas::Message::StringType&)>& callback) {
	auto I = map.find(name);
	if (I != map.end()) {
		if (I->second.isString()) {
			callback(I->second.String());
		}
	}
}

template<>
inline void AtlasQuery::find(const Atlas::Message::MapType& map, const std::string& name, const std::function<void(const Atlas::Message::FloatType&)>& callback) {
	auto I = map.find(name);
	if (I != map.end()) {
		if (I->second.isFloat()) {
			callback(I->second.Float());
		}
	}
}


#endif //ATLASQUERY_H
