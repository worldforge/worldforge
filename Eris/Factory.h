#ifndef ERIS_FACTORY_H
#define ERIS_FACTORY_H

#include <Atlas/Objects/ObjectsFwd.h>

namespace Eris {

// forward decls	
class Entity;
class View;
class TypeInfo;

/// Factory is used to allow custom entity creation by client libraries
class Factory
{
public:	
    virtual ~Factory();
    /// Accept is called by the world to test if this factory can instantiate the specified object
    /** Accept is called when an entity must be constructed; this will be called every time
    an object is created, so avoid lengthy processing if possible. */

    virtual bool accept(const Atlas::Objects::Entity::RootEntity &ge, TypeInfo* type) = 0;

    /// create whatever entity the client desires
    virtual Entity* instantiate(const Atlas::Objects::Entity::RootEntity &ge, TypeInfo* type, View* v) = 0;
    
    /** retrieve this factory's priority level; higher priority factories
    get first chance to process a received Atlas entity. The default implementation
    returns one. */
    virtual int priority();
};
	
} // of namespace Eris

#endif
