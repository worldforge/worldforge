#ifndef ERIS_FACTORY_H
#define ERIS_FACTORY_H

#include <Atlas/Objects/Entity/GameEntity.h>
#include "Types.h"

namespace Eris {

// forward decls	
class Entity;
class World;

/// Factory is used to allow custom entity creation by client libraries
class Factory
{
public:	
	/// Accept is called by the world to test if this factory can instantiate the specified object
	/** Accept is called when an entity must be constructed; this will be called every time
	an object is created, so avoid lengthy processing if possible. */

	//virtual bool Accept(const Atlas::Message::Object &o) = 0;
	virtual bool accept(const Atlas::Objects::Entity::GameEntity &ge, World *world) = 0;

	/// create whatever entity the client desires
	virtual EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity &ge, World *world) = 0;
	//virtual EntityPtr Instantiate(const Atlas::Message::Object &o) = 0;
};

class StdFactory : public Factory
{
public:
	virtual bool accept(const Atlas::Objects::Entity::GameEntity &ge, World *world);
	virtual EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity &ge, World *world);
};
	

} // of namespace

#endif
