#ifndef ERIS_ENTITY_H
#define ERIS_ENTITY_H

// system headers
#include <sigc++/object.h>
#include <sigc++/basic_signal.h>

#include <Atlas/Message/Object.h>

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
		}
	}
}

// local headers
#include "Types.h"
#include "SignalDispatcher.h"

namespace Eris {

// Forward Declerations	
class Entity;
class World;	
class Dispatcher;
	
typedef std::vector<Entity*> EntityArray;

/// Entity is a concrete (instanitable) class representing one game entity
/** Entity encapsulates the state and tracking of one game entity; this includes
it's location in the containership tree (graph?), it's name and unique and id,
and so on.

This class may be sub-classed by users (and those sub-classes built via
a Factory), to allow specific functionality. This means there are two
integration strategies; either subclassing and over-riding virtual functions,
or creating peer clases and attaching them to the signals.
*/

class Entity : public SigC::Object
{
public:	
	explicit Entity(const Atlas::Objects::Entity::GameEntity &ge);
	virtual ~Entity();

	/* container interface; not virtualized, should it be? i.e, will anyone want to
	change containership semantics on the client? Assume no until proved
	otherwise */
	Entity* getContainer() const
	{ return _container; }

	unsigned int getNumMembers() const
	{ return _members.size(); }

	Entity* getMember(unsigned int index);

	// property query interface
	/// access a property; thows UnknownProperty if not found
	virtual const Atlas::Message::Object& getProperty(const std::string &p);
	/// test whether the named property exists on this entity
	virtual bool hasProperty(const std::string &p) const;

	// dynamics; these are virtual to allow derived class to implement motion
	// prediction or other effects that can't be guessed here
	virtual Coord getPosition() const;
	// virtual Vector GetOrientation() const; (heading or euler triple?)
	virtual BBox getBBox() const;
	
	// accesors
	/// retrieve the unique entity ID
	std::string getID() const { return _id;}
	std::string getName() const { return _name;}
	
	/// access the current time-stamp of the entity
	float getStamp() const
	{ return _stamp; }

	/// Get a set of the parent objects of the entity
	StringSet getInherits() const
	{ return _parents; }
	
// Signals
	SigC::Signal1<void, Entity*> AddedMember;
	SigC::Signal1<void, Entity*> RemovedMember;
	
	/// Signal that the entity's container changed
	/** Emitted when the entity's container changes; note if the entity has
	just been created, or is being deleted, then one of the arguments may be NULL
	(but not both)
	*/
	SigC::Signal2<void, Entity*, Entity*> Recontainered;

	/** Emitted when a macro change occurs */
	SigC::Signal1<void, Entity*> Changed;

	/** Emitted when then entity's position or orientation have changed; i.e the
	displayed model/sprite/etc needs to be updated. The argument is the new position */
	SigC::Signal2<void, Entity*, Coord> Moved;

	/** Emitted with this entity speaks. In the future langauge may be specified */
	SigC::Signal2<void, Entity*, string> Say;
	
	// signal accessors (dynamically created signals)
	//void ConnectPropertySlot(const string &p, PropertySlot &slot);
	
	/** Emitted when this entity originates the specified class of operation;
	note the derived operations will also invoke the signal */
	template <class T>
	void ConnectOpToSlot(const std::string &op, SigC::Slot1<void, T> &slot)
	{ innerOpToSlot(new SignalDispatcher<T>(op, slot)); }
	
	/** Emitted when this entity receives a specified class of operations; again
	derived operations will also trigger the signal to be invoked */
	template <class T>
	void ConnectOpFromSlot(const std::string &op, SigC::Slot1<void, T> &slot)
	{  innerOpFromSlot(new SignalDispatcher<T>(op, slot)); }
	
protected:	
	/// constructor for use by derived classes only!
	explicit Entity(const std::string &id);	

	// the World is a friend, so that it may set properties and containership of
	// entities.
	friend class World;

	virtual void handleMove();
	virtual void handleTalk(const string &msg);
	virtual void handleChanged();
	
	/// set the property value; this protected so only Entity / World may use it
	virtual void setProperty(const std::string &p, const Atlas::Message::Object &v);	

	/// update the container of this entity (may be NULL)
	virtual void setContainer(Entity *pr);
	
	/// add a contained entity to this object (sets container)
	virtual void addMember(Entity *e);
	
	/// remove an contained entity
	/** remove a contained entity; throws InvalidOperation if not found. Note
	that the container of e is <i>not<i/> reset */
	virtual void rmvMember(Entity *e);
	
	void setContainerById(const std::string &id);
	
	const std::string _id;	///< the Atlas object ID
	std::string _name;		///< a human readable name
	float _stamp;		///< last modification time (in seconds) 
	std::string _description;// surely this should be retrieved dynamically from the server?
	StringSet _parents;
	
	// container-ship / entity heirarchy
	Entity* _container;	///< The container entity, NULL for the root-entity, or if un-parented
	EntityArray _members;

	BBox _bbox;
	Coord _position;

// attribute synchronisation
	/// Mark the attribute set as needing a resync operation before any Get()s
	/// @param attr The attribute which is no longer in sync
	void setUnsync(const std::string &attr)
	{ _unsynched.insert(attr); }
	
	/// Resynchronize the native and Atlas values of the  named attributes
	/// @param attrs The set of attributes to be synchronised
	virtual void resync(StringSet &attrs);

	/// Predicate, valid if every attribute is synchronised between native and Atlas
	bool isFullySynched() const
	{ return _unsynched.empty(); }

	Atlas::Message::Object::MapType _properties;
	StringSet _unsynched;	///< unsynchronised attributes

private:
	//friend class World;	// World has to be a friend so it can call these
		
	// these are private and final so sub-classing people don't get confused with the
	// respective handleXXXX methods. If you want to change how the raw Atlas
	// operation is processed, setup a custom dispatcher.
	void recvSight(const Atlas::Objects::Entity::GameEntity &ge);	
	void recvMove(const Atlas::Objects::Operation::Move &mv);
	void recvSet(const Atlas::Objects::Operation::Set &st);
	
	void recvSound(const Atlas::Objects::Operation::Sound &snd);
	void recvTalk(const Atlas::Objects::Operation::Talk &tk);

	void innerOpFromSlot(Dispatcher *s);
	void innerOpToSlot(Dispatcher *s);

	// disptchers that have been bound to this entity (so we know to delete them)
	StringList _localDispatchers;
};


// motion predicted entity that moves a lot.
class Moveable : public Entity
{
public:
	Moveable(const string &id);
	virtual ~Moveable();

	void getPosition(bool predicted);

protected:
	Coord _velocity,
		_delta;
};

class UnknownProperty : public InvalidOperation
{
public:	
	UnknownProperty(const std::string &p, const std::string &m) :
		InvalidOperation(m), prop(p)
	{;}
	
	const std::string prop;
};

} // of namespace

#endif
