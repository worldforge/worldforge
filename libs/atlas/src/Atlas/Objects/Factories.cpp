/*
 Copyright 2000-2001 Aloril.
 Copyright 2001-2005 Alistair Riddoch.
 Copyright (C) 2019 Erik Ogenvik

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

#include "Factories.h"

#include "Atlas/Objects/Anonymous.h"
#include "Atlas/Objects/Generic.h"


namespace Atlas::Objects {

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

SmartPtr<RootData> generic_factory(const std::string& name, int no) {
	Operation::Generic obj;
	obj->setType(name, no);
	return obj;
}

SmartPtr<RootData> anonymous_factory(const std::string& name, int no) {
	Entity::Anonymous obj;
	obj->setType(name, no);
	return obj;
}

Factories::Factories() {
	installStandardTypes();
}

Factories::~Factories() = default;

bool Factories::hasFactory(const std::string& name) const {
	auto I = m_factories.find(name);
	return I != m_factories.end();
}

Root Factories::createObject(const std::string& name) const {
	auto I = m_factories.find(name);
	if (I == m_factories.end()) {
		return {nullptr};
	} else {
		return (*I).second.factory_method(name, (*I).second.classno);
	}
}

Root Factories::createObject(const MapType& msg_map) const {
	Root obj = instantiateObject(msg_map);

	for (auto& entry: msg_map) {
		obj->setAttr(entry.first, entry.second, this);
	}
	return obj;
}

Root Factories::createObject(Atlas::Message::MapType&& msg_map) const {
	Root obj = instantiateObject(msg_map);

	for (auto& entry: msg_map) {
		obj->setAttr(entry.first, std::move(entry.second), this);
	}
	return obj;
}


Root Factories::instantiateObject(const Atlas::Message::MapType& msg_map) const {
	// is this instance of entity or operation?
	auto I = msg_map.find(Atlas::Objects::OBJTYPE_ATTR);
	auto Iend = msg_map.end();
	if (I != Iend && I->second.isString()) {
		auto& objtype = I->second.String();
		if (objtype == "op" || objtype == "obj" || objtype == "object") {
			// get parent
			I = msg_map.find(Atlas::Objects::PARENT_ATTR);
			if (I != Iend && I->second.isString()) {
				auto& parent = I->second.String();
				// objtype and parent ok, try to create it:
				auto J = m_factories.find(parent);
				if (J != m_factories.end()) {
					return J->second.factory_method(parent, J->second.classno);
				} else {
					if (objtype == "op") {
						return Atlas::Objects::Operation::Generic();
					} else {
						return Atlas::Objects::Entity::Anonymous();
					}
				}
				// FIXME We might want to do something different here.
			} // has parent attr?
		} // has known objtype
	} // has objtype attr
	// Should we really use factory? Why not just instantiate by hand?
	return Atlas::Objects::Entity::Anonymous();
}


Root Factories::getDefaultInstance(const std::string& name) const {
	auto I = m_factories.find(name);
	if (I == m_factories.end()) {
		//perhaps throw something instead?
		return {nullptr};
	} else {
		return (*I).second.default_instance_method(name, (*I).second.classno);
	}
}

std::list<std::string> Factories::getKeys() const {
	std::list<std::string> keys;
	for (const auto& factory: m_factories) {
		keys.push_back(factory.first);
	}
	return keys;
}

void Factories::addFactory(const std::string& name, FactoryMethod method, DefaultInstanceMethod defaultInstanceMethod, int classno) {
	Factory factory{};
	factory.classno = classno;
	factory.default_instance_method = defaultInstanceMethod;
	factory.factory_method = method;
	m_factories[name] = factory;
}

int Factories::addFactory(const std::string& name, FactoryMethod method, DefaultInstanceMethod defaultInstanceMethod) {
	int classno = ++enumMax;
	Factory factory{};
	factory.classno = classno;
	factory.default_instance_method = defaultInstanceMethod;
	factory.factory_method = method;
	m_factories[name] = factory;
	return classno;
}

std::vector<Root> Factories::parseListOfObjects(const Atlas::Message::ListType& val) const {
	std::vector<Root> objects;
	objects.reserve(val.size());
	for (const auto& entry: val) {
		if (entry.isMap()) {
			objects.emplace_back(createObject(entry.Map()));
		}
	}
	return objects;
}

std::vector<Root> Factories::parseListOfObjects(Atlas::Message::ListType&& val) const {
	std::vector<Root> objects;
	objects.reserve(val.size());
	for (auto& entry: val) {
		if (entry.isMap()) {
			objects.emplace_back(createObject(entry.moveMap()));
		}
	}
	return objects;
}

}
// namespace Atlas::Objects
