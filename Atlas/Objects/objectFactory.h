// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Aloril

#ifndef ATLAS_OBJECTS_OBJECTFACTORY_H
#define ATLAS_OBJECTS_OBJECTFACTORY_H

#include <string>
#include <list>
#include <map>

#include "Root.h"

namespace Atlas {

class NoSuchFactoryException
{
public:
    NoSuchFactoryException(const std::string& name) : name(name) {}
    std::string name;
};

typedef Root (*FactoryMethod)();
typedef map<const std::string, FactoryMethod> FactoryMap;

class Factories 
{
public:
    bool hasFactory(const std::string& name);
    Root createObject(const std::string& name);
    list<std::string> getKeys();
    void addFactory(const std::string& name, FactoryMethod method);
private:
    FactoryMap m_factories;
};
    
extern map<const std::string, Root> objectDefinitions;
extern Factories objectFactory;
//extern Factories objectInstanceFactory;

Root messageObject2ClassObject(const Atlas::Object& mobj);

} // namespace Atlas

#endif
