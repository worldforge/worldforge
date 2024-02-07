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


#include "common/type_utils_impl.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;

using Atlas::Objects::Root;

Root atlasOpDefinition(const std::string& name, const std::string& parent) {
	Atlas::Objects::Entity::Anonymous r;

	r->setParent(parent);
	r->setObjtype("op_definition");
	r->setId(name);

	return r;
}

Root atlasClass(const std::string& name, const std::string& parent) {
	Atlas::Objects::Entity::Anonymous r;

	r->setParent(parent);
	r->setObjtype("class");
	r->setId(name);

	return r;
}

Root atlasType(const std::string& name,
			   const std::string& parent,
			   bool abstract) {
	Atlas::Objects::Entity::Anonymous r;

	r->setParent(parent);
	r->setObjtype(abstract ? "data_type" : "type");
	r->setId(name);

	return r;
}

void idListasObject(const IdList& l, Atlas::Message::ListType& ol) {
	ol.clear();
	for (auto& entry: l) {
		ol.emplace_back(entry);
	}
}

int idListFromAtlas(const Atlas::Message::ListType& l, IdList& ol) {
	ol.clear();
	for (auto& entry: l) {
		if (!entry.isString()) {
			return -1;
		}
		ol.push_back(entry.asString());
	}
	return 0;
}
