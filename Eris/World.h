#ifndef ERIS_WORLD_H
#define ERIS_WORLD_H

#include <sigc++/object.h>
#include <sigc++/basic_signal.h>

#include <map>

// GCC 2.9.x need this manually included
#if defined(__GNUC__) && __GNUC__ < 3
#	include <multimap.h>
#endif

namespace Atlas {
	namespace Objects {
		namespace Entity {
			class RootEntity;
			class GameEntity;
		}
		
		namespace Operation {
			class Move;
			class Set;
			class Sound;
			class Talk;
			class Look;
			class Appearance;
			class Disappearance;
			class Info;
			class Delete;
			class Create;
			class Sight;
		}
	}
}

#include "Types.h"

namespace Eris {

// Forward declarations	
class Connection;
class Player;
class Factory;
class InvisibleEntityCache;
	
// the name is wrong, but I feel 'IDEntityMap' is worse
typedef std::map<std::string, Entity*> EntityIDMap;

class UnknownEntity : public BaseException
{
public:
	UnknownEntity(const std::string &msg, const std::string &id) :
		BaseException(msg), _id(id) {;}
	virtual ~UnknownEntity() throw() { }
	std::string _id;
};

/// the entity database and dispatch point
class World : public SigC::Object
{
public:
	// life-cycle management
	World(Player *p, Connection *c);
	~World();

	/// convert an entity ID to an instance pointer
	EntityPtr lookup(const std::string &id);

	/// obtain a pointer to the root entity for this client (i.e look(""))
	EntityPtr getRootEntity();

	/// Retrieve the Connection object associated with the World
	Connection* getConnection() const
	{ return _con; }
	
	/** update the World state periodically : this runs various house-keeping tasks, such
	as running client side looping events, flushing old entities from memory, and running
	client-side motion prediction and interpolation. Basically, you want to be calling this
	reasonably often. */
	void tick();
	
// focus / observation
	/// specify the origin entity for the world; nearly always the player
	void setFocusedEntity(EntityPtr f);
	/** get the currently focused entity. This value is only valid after World.Entered is emitted;
	prior to that it will return NULL. */
	EntityPtr getFocusedEntity()
	{ return _focused; }

	/** get the id of the focused entity. This should be identical to doing getFocusedEntity()->getID(),
	except that is value is valid before World.Entered is emitted. */
	std::string getFocusedEntityID();
	
// factories
	/** Register an entity factory with the world. Any new entities that the
	Factory accept()s will be passed to the Factory for instantiation. */
	/// @param f The factory instance
	/// @param priority Controls the search order; higher-valued factories are tried first
	void registerFactory(Factory *f, unsigned int priority = 1);
	
	/// Remove an factory from the search set.
	void unregisterFactory(Factory *f);

	/// World is a singleton; this is the accessor
	static World* Instance();
	
// signals
	/// Emitted when an entity is created
	/** Emitted after an entity has been created, added to the world and parented
	for the first time (i.e basic setup is all complete) */
	SigC::Signal1<void, Entity*> EntityCreate;

	/// Emitted when an entity is deleted
	/** Emitted before an entity is deleted. The signal is invoked <i>before</i> the ID
	is unregistered or entity is unparented */
	SigC::Signal1<void, Entity*> EntityDelete;

	/// Entered signal is invoked just once, when the player enters the IG world
	SigC::Signal1<void, Entity*> Entered;
	
	/// Appearance is emitted when an entity becoms visible to the client
	SigC::Signal1<void, Entity*> Appearance;
	
	/// Disappearance indicates the client should hide the entity from the user
	SigC::Signal1<void, Entity*> Disappearance;
	
	/** RootEntity change : emitted when the Top-Level Visible Entity (TLVE) change.
	The TLVE is the entity returned by LOOK("") at any given point. The signals specifies
	the new TLVE; you can obtain the current one by calling getRootEntity, i.e the
	the internal value is updated <i>after</i> the signal is emitted. */
	SigC::Signal1<void, Entity*> RootEntityChanged;
	
	// entity change  / move signals ? excessive duplicaton...
	
protected:
	friend class Entity;
	
	// so the cache can call flush()
	friend class InvisibleEntityCache;
		
	void look(const std::string &id);	
	EntityPtr create(const Atlas::Objects::Entity::GameEntity &ge);

	bool isPendingInitialSight(const std::string &id)
	{ return _pendingInitialSight.count(id); }

	void registerCallbacks();
	
	void setRootEntity(Entity* root);
	
	/** interface for entities to get themselves cleaned up eventually;
	this method will place the entity into an internal LRU cache maintained by
	world (the InvisibleEntityCache) for some duration, before the Entity
	is deleted.*/
	void markInvisible(Entity *e);
	
	/** mark the entity as visible again : this will place the entity back into the
	active list. Note that no other state changes occur. */
	void markVisible(Entity *e);
	
	/** remove the specified entity from the world permanently*/
	void flush(Entity *e);
// callbacks
	void recvInfoCharacter(const Atlas::Objects::Operation::Info &ifo,
		const Atlas::Objects::Entity::GameEntity &character);
	void recvAppear(const Atlas::Objects::Operation::Appearance &ap);	
	void recvDisappear(const Atlas::Objects::Operation::Disappearance &ds);

	// sight ops
	void recvSightObject(const Atlas::Objects::Operation::Sight &sight,
		const Atlas::Objects::Entity::GameEntity &ent);

	void recvSightCreate(const Atlas::Objects::Operation::Create &cr,
		const Atlas::Objects::Entity::GameEntity &ent);
	void recvSightDelete(const Atlas::Objects::Operation::Delete &del);
	void recvSightSet(const Atlas::Objects::Operation::Set &set);
	void recvSightMove(const Atlas::Objects::Operation::Move &mv);

	// sound ops
	void recvSoundTalk(const Atlas::Objects::Operation::Sound &snd,
		const Atlas::Objects::Operation::Talk &tk);

	// error ops
	void recvErrorLook(const Atlas::Objects::Operation::Look &lk);
	
	void lookupTimeout(std::string id);
	
	/// callback when Connection generates a 'Connected' signal (usually a reconnect)
	void netConnect();
	
	/// ID of the playing character (usually the same as the focused entity's ID)
	std::string _characterID;	
	Connection* _con;	///< The underlying connection
	Player* _player;	///< The Player object (future : list)
	bool _initialEntry; ///< Set if World.Entered needs to be emitted

	EntityIDMap _lookup;	///< this map tracks <i>all</i> entities we mirror
	EntityPtr _root,		///< the root entity of the world (manged by us)
		_focused;	///< origin entity for field-of-view and so on

	// factory storage : allows ordering
	typedef std::multimap<unsigned int, Factory*> FactoryMap;

	/// storage of every entity factory registered on the world
	FactoryMap _efactories;
	
	/// Set of entities that are waiting for a SIGHT after a LOOK (caused by an appear / move / set / ...)
	StringSet _pendingInitialSight;
	
	/// cache of invisble entities that might re-appear so we keep them around
	InvisibleEntityCache* _ieCache;
	
	/// static singleton instance
	static World* _theWorld;
};
	
} // of namespace
#endif
