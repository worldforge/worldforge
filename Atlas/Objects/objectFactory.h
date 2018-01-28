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
#include <utility>

namespace Atlas { namespace Objects {

class NoSuchFactoryException : public Atlas::Exception
{
  protected:
    std::string name;
  public:
    explicit NoSuchFactoryException(std::string factoryName) noexcept :
               Atlas::Exception("No factory for Objects type"),
               name(std::move(factoryName)) { }

    ~NoSuchFactoryException() noexcept override = default;
    const std::string & getName() {
        return name;
    }
};

template <class T>
static SmartPtr<RootData> factory(const std::string &, int)
{
    return SmartPtr<T>();
}

template <class T>
static SmartPtr<RootData> defaultInstance(const std::string &, int)
{
    return T::allocator.getDefaultObjectInstance();
}


SmartPtr<RootData> generic_factory(const std::string & name, int no);
SmartPtr<RootData> anonymous_factory(const std::string & name, int no);

typedef Root (*FactoryMethod)(const std::string &, int);
typedef Root (*DefaultInstanceMethod)(const std::string &, int);

/**
 * Holds methods for creating new instances and accessing the default instance.
 */
struct Factory
{
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
typedef std::map<const std::string, Factory > FactoryMap;

class Factories 
{
public:
    friend class AddFactories;

    Factories() = default;
    Factories(const Factories &) = default;

    bool hasFactory(const std::string& name);
    Root createObject(const std::string& name);
    Root createObject(const Atlas::Message::MapType & msg);
    Root getDefaultInstance(const std::string& name);
    std::list<std::string> getKeys();
    int addFactory(const std::string& name, FactoryMethod method, DefaultInstanceMethod defaultInstanceMethod);

    static Factories * instance();
private:
    FactoryMap m_factories;

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
    template <typename T>
    void addFactory(const std::string& name, int classno);
};
    
extern std::map<const std::string, Root> objectDefinitions;


template <typename T>
void Factories::addFactory(const std::string& name, int classno)
{
    addFactory(name, &factory<T>, &defaultInstance<T>, classno);
}

} } // namespace Atlas::Objects

#endif // ATLAS_OBJECTS_OBJECTFACTORY_H
