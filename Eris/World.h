#ifndef ERIS_WORLD_H
#define ERIS_WORLD_H

#include <sigc++/object.h>
#include <sigc++/basic_signal.h>

#include <map>
#include <multimap.h>

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

// forward declerations	
class Connection;
class Player;
class Factory;
	
// the name is wrong, but I feel 'IDEntityMap' is worse
typedef std::map<std::string, Entity*> EntityIDMap;

class UnknownEntity : public BaseException
{
public:
	UnknownEntity(const string &msg, const string &id) :
		BaseException(msg), _id(id) {;}
	string _id;
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
	
	/// access the encoder bridge for the underlying connection
	//Atlas::Bridge* GetEncoder() const;
	
// focus / observation
	/// specify the origin entity for the world; nearly always the player
	void setFocusedEntity(EntityPtr f);
	/// get the currently focused entity
	EntityPtr getFocusedEntity()
	{ return _focused; }

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
	
	// entity change  / move signals ? excessive duplicaton...
	
protected:
	friend class Entity;
		
	void look(const std::string &id);	
	EntityPtr create(const Atlas::Objects::Entity::GameEntity &ge);

	bool isPendingInitialSight(const std::string &id)
	{ return _pendingInitialSight.count(id); }

	void registerCallbacks();
	
	void setRootEntity(Entity* root);
	
// callbacks
	void recvInfoOp(const Atlas::Objects::Operation::Info &ifo);
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
	
	/// callback when Connection generates a 'Connected' signal (usually a reconnect)
	void netConnect();
	
	std::string _characterID;	
	Connection* _con;	///< The underlying connection
	Player* _player;	///< The Player object (future : list)
	bool _initialEntry;

	EntityIDMap _lookup;	///< this map tracks <i>all</i> entities we mirror
	EntityPtr _root,		///< the root entity of the world (manged by us)
		_focused;	///< origin entity for field-of-view and so on

	// factory storage : allows ordering
	typedef std::multimap<unsigned int, Factory*> FactoryMap;

	FactoryMap _efactories;
	
	/// Set of entities that are waiting for a SIGHT after a LOOK (caused by an appear / move / set / ...)
	StringSet _pendingInitialSight;
	
	// static singleton instance
	static World* _theWorld;
};
	
}; // of namespace
#endif
