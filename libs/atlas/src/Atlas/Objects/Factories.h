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

#ifndef ATLAS_C_FACTORIES_H
#define ATLAS_C_FACTORIES_H


#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <string>
#include <list>
#include <map>


namespace Atlas::Objects {

template<class T>
static SmartPtr<RootData> factory(const std::string&, int) {
	return SmartPtr<T>();
}

template<class T>
static SmartPtr<RootData> defaultInstance(const std::string&, int) {
	return T::allocator.getDefaultObjectInstance();
}


SmartPtr<RootData> generic_factory(const std::string& name, int no);

SmartPtr<RootData> anonymous_factory(const std::string& name, int no);

typedef Root (* FactoryMethod)(const std::string&, int);

typedef Root (* DefaultInstanceMethod)(const std::string&, int);

/**
 * Holds methods for creating new instances and accessing the default instance.
 */
struct Factory {
public:
	/**
	 * Method for creating a new instance.
	 */
	FactoryMethod factory_method;

	/**
	 * Method for accessing the default instance.
	 */
	DefaultInstanceMethod default_instance_method;

	/**
	 * The class number for the objects created by this factory.
	 */
	int classno;
};

class Factories {
public:

	Factories();

	Factories(const Factories&) = default;

	~Factories();

	bool hasFactory(const std::string& name) const;

	Root createObject(const std::string& name) const;

	/**
	 * Creates a new object.
	 *
	 * If the map that's supplied can be destroyed, look into using "createObject" with move semantics..
	 * @param msg
	 * @return
	 */
	Root createObject(const Atlas::Message::MapType& msg) const;

	/**
	 * Creates a new object by supplying a Map which is moved into the resulting object.
	 * @param msg
	 * @return
	 */
	Root createObject(Atlas::Message::MapType&& msg) const;

	Root getDefaultInstance(const std::string& name) const;

	std::list<std::string> getKeys() const;

	int addFactory(const std::string& name, FactoryMethod method, DefaultInstanceMethod defaultInstanceMethod);

	void installStandardTypes();

	/**
	 * Parses the supplied list into a vector of Root objects.
	 * @param val
	 * @return
	 */
	std::vector<Root> parseListOfObjects(const Atlas::Message::ListType& val) const;

	std::vector<Root> parseListOfObjects(Atlas::Message::ListType&& val) const;

private:

	static int enumMax;

	std::map<const std::string, Factory> m_factories;


	Root instantiateObject(const Atlas::Message::MapType& msg) const;


	/**
	 * Adds a new factory.
	 * @param name The class name attached to the factory.
	 * @param method The method used for creating new objects.
	 * @param defaultInstanceMethod The method used for accessing the
	 *          default instance.
	 * @param classno The class number.
	 */
	void addFactory(const std::string& name, FactoryMethod method, DefaultInstanceMethod defaultInstanceMethod, int classno);

	/**
	 * Adds a new factory using a type.
	 *
	 * This is a utility version of the more extensive addFactory method which
	 * will access T::allocator to get the methods needed.
	 *
	 * @param name The class name attached to the factory.
	 * @param classno The class number.
	 */
	template<typename T>
	void addFactory(const std::string& name, int classno);
};

template<typename T>
void Factories::addFactory(const std::string& name, int classno) {
	addFactory(name, &factory<T>, &defaultInstance<T>, classno);
}

}
// namespace Atlas::Objects

#endif //ATLAS_C_FACTORIES_H
