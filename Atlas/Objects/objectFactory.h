// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Aloril

#ifndef ATLAS_OBJECTS_OBJECTFACTORY_H
#define ATLAS_OBJECTS_OBJECTFACTORY_H

#include <Atlas/Objects/Root.h>

#include <string>
#include <list>
#include <map>

namespace Atlas { namespace Objects {

class NoSuchFactoryException : public Atlas::Exception
{
  protected:
    std::string name;
  public:
    NoSuchFactoryException(const std::string& name) :
               Atlas::Exception("No factory for Objects type"), name(name) { }
    virtual ~NoSuchFactoryException() throw () { }
    const std::string & getName() {
        return name;
    }
};

typedef Root (*FactoryMethod)();
typedef std::map<const std::string, FactoryMethod> FactoryMap;

class Factories 
{
public:
    bool hasFactory(const std::string& name);
    Root createObject(const std::string& name);
    std::list<std::string> getKeys();
    int addFactory(const std::string& name, FactoryMethod method);
private:
    FactoryMap m_factories;
};
    
extern std::map<const std::string, Root> objectDefinitions;
extern Factories objectFactory;
//extern Factories objectInstanceFactory;

Root messageElement2ClassObject(const Atlas::Message::Element::MapType & mobj);

} } // namespace Atlas::Objects

#endif // ATLAS_OBJECTS_OBJECTFACTORY_H
