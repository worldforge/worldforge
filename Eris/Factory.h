#ifndef ERIS_FACTORY_H
#define ERIS_FACTORY_H

#include <Atlas/Objects/ObjectsFwd.h>

namespace Eris {

// forward decls	
class Entity;

/// Factory is used to allow custom entity creation by client libraries
class Factory
{
public:	
    /// Accept is called by the world to test if this factory can instantiate the specified object
    /** Accept is called when an entity must be constructed; this will be called every time
    an object is created, so avoid lengthy processing if possible. */

    virtual bool accept(const Atlas::Objects::Entity::GameEntity &ge) = 0;

    /// create whatever entity the client desires
    virtual Entity* instantiate(const Atlas::Objects::Entity::GameEntity &ge) = 0;
    
    static Entity* createEntity(const Atlas::Objects::Entity::GameEntity &ge);
    static void registerFactory(Factory* f);
};

class DefaultFactory : public Factory
{
public:
    virtual bool accept(const Atlas::Objects::Entity::GameEntity &ge);
    virtual Entity* instantiate(const Atlas::Objects::Entity::GameEntity &ge);
};
	
} // of namespace Eris

#endif
