// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Aloril
// Copyright (C) 2005 Al Riddoch

// $Id$

#ifndef ATLAS_OBJECTS_OBJECTFACTORY_H
#define ATLAS_OBJECTS_OBJECTFACTORY_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

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
    virtual ~NoSuchFactoryException() throw ();
    const std::string & getName() {
        return name;
    }
};

template <class T>
static SmartPtr<RootData> factory(const std::string &, int)
{
    SmartPtr<T> obj;
    return obj;
}

SmartPtr<RootData> generic_factory(const std::string & name, int no);
SmartPtr<RootData> anonymous_factory(const std::string & name, int no);

typedef Root (*FactoryMethod)(const std::string &, int);
typedef std::map<const std::string, std::pair<FactoryMethod, int> > FactoryMap;

class Factories 
{
public:
    friend class AddFactories;

    Factories();
    explicit Factories(const Factories &);

    bool hasFactory(const std::string& name);
    Root createObject(const std::string& name);
    Root createObject(const Atlas::Message::MapType & msg);
    std::list<std::string> getKeys();
    int addFactory(const std::string& name, FactoryMethod method);

    static Factories * instance();
private:
    FactoryMap m_factories;

    void addFactory(const std::string& name, FactoryMethod method, int classno);
};
    
extern std::map<const std::string, Root> objectDefinitions;

} } // namespace Atlas::Objects

#endif // ATLAS_OBJECTS_OBJECTFACTORY_H
