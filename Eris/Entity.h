#ifndef ERIS_ENTITY_H
#define ERIS_ENTITY_H

// system headers
#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

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

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>

// local headers
#include <Eris/Types.h>
#include <Eris/SignalDispatcher.h>


namespace Eris {

// Forward Declerations	
class Entity;
class World;	
class Dispatcher;
class Property;
class TypeInfo;
    
typedef std::vector<Entity*> EntityArray;

typedef std::map<std::string, Property*> PropertyMap;

/// Entity is a concrete (instanitable) class representing one game entity
/** Entity encapsulates the state and tracking of one game entity; this includes
it's location in the containership tree (graph?), it's name and unique and id,
and so on.

This class may be sub-classed by users (and those sub-classes built via
a Factory), to allow specific functionality. This means there are two
integration strategies; either subclassing and over-riding virtual functions,
or creating peer clases and attaching them to the signals.
*/

class Entity : virtual public SigC::Object
{
public:	
	explicit Entity(const Atlas::Objects::Entity::GameEntity &ge, World *world);
	virtual ~Entity();

	/* container interface; not virtualized, should it be? i.e, will anyone want to
	change containership semantics on the client? Assume no until proved
	otherwise */
	Entity* getContainer() const
	{ return _container; }

	unsigned int getNumMembers() const
	{ return _members.size(); }

        bool hasBBox() const
        { return _hasBBox; }

	Entity* getMember(unsigned int index);

	// property query interface
	/// access a property; thows UnknownProperty if not found
	virtual const Atlas::Message::Object& getProperty(const std::string &p);
	/// test whether the named property exists on this entity
	virtual bool hasProperty(const std::string &p) const;

	// dynamics; these are virtual to allow derived class to implement motion
	// prediction or other effects that can't be guessed here
	virtual WFMath::Point<3> getPosition() const;
	virtual WFMath::Vector<3> getVelocity() const;
	
	/** retrieve the orientation as a quaternion */
	virtual WFMath::Quaternion getOrientation() const;
	
	virtual WFMath::AxisBox<3> getBBox() const;
	
	// accesors
	/// retrieve the unique entity ID
	const std::string& getID() const { return _id;}
	const std::string& getName() const { return _name;}
	
	/// access the current time-stamp of the entity
	float getStamp() const
	{ return _stamp; }

	/// Get a set of the parent objects of the entity
	StringSet getInherits() const
	{ return _parents; }
	
	TypeInfo* getType() const;

	World* getWorld() const
	{ return _world; }
	
	/** Query the visiblity of this entity; this is controlled by Appearance/Disappearance ops
	from the server */
	bool isVisible() const
	{ return _visible; }
	
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
	SigC::Signal1<void, const StringSet&> Changed;

	/** Emitted when then entity's position or orientation have changed; i.e the
	displayed model/sprite/etc needs to be updated. The argument is the new position */
	SigC::Signal1<void, const WFMath::Point<3>&> Moved;

	/** Emitted with this entity speaks. In the future langauge may be specified */
	SigC::Signal1<void, const std::string&> Say;
	
	/** Emitted when this entity originates the specified class of operation;
	note the derived operations will also invoke the signal */
	template <class T>
	void connectOpToSlot(const std::string &op, const SigC::Slot1<void, const T&> &slot)
	{ innerOpToSlot(new SignalDispatcher<T>(op, slot)); }
	
	/** Emitted when this entity receives a specified class of operations; again
	derived operations will also trigger the signal to be invoked */
	template <class T>
	void connectOpFromSlot(const std::string &op, SigC::Slot1<void, const T&> &slot)
	{  innerOpFromSlot(new SignalDispatcher<T>(op, slot)); }
	
	void observeProperty(const std::string &nm, 
	    const SigC::Slot1<void, const Atlas::Message::Object&> slot);
	
protected:	
	/// constructor for use by derived classes only!
	explicit Entity(const std::string &id, World *world);	

	// the World is a friend, so that it may set properties and containership of
	// entities.
	friend class World;

	virtual void handleMove();
	virtual void handleTalk(const std::string &msg);
	
	/// set the property value; this protected so only Entity / World may use it
	virtual void setProperty(const std::string &p, const Atlas::Message::Object &v);	

	virtual void setPosition(const WFMath::Point<3>& pt);
	/// update the container of this entity (may be NULL)
	virtual void setContainer(Entity *pr);
	
	virtual void setContents(const Atlas::Message::Object::ListType &contents);
	
	/// add a contained entity to this object (sets container)
	virtual void addMember(Entity *e);
	
	/// remove an contained entity
	/** remove a contained entity; throws InvalidOperation if not found. Note
	that the container of e is <i>not<i/> reset */
	virtual void rmvMember(Entity *e);
	
	/** called by World in response to Appearance/Disappearance messages : note that
	after a disappearance (vis = false), the server will not send any futher messages to the
	entity. At some point, invisible entities get flushed by Eris using an LRU scheme. */
	virtual void setVisible(bool vis);
	
	void setContainerById(const std::string &id);
	
	const std::string _id;	///< the Atlas object ID
	std::string _name;		///< a human readable name
	float _stamp;		///< last modification time (in seconds) 
	std::string _description;// surely this should be retrieved dynamically from the server?
	StringSet _parents;
	bool _visible;
	
	// container-ship / entity heirarchy
	Entity* _container;	///< The container entity, NULL for the root-entity, or if un-parented
	EntityArray _members;

	WFMath::AxisBox<3> _bbox;
	WFMath::Point<3> _position;
	WFMath::Vector<3> _velocity;
	WFMath::Quaternion _orientation;
	
// properties
	void beginUpdate();
	void endUpdate();
	
	PropertyMap _properties;
	
	/** This flag is set if a property update is in progress. This supresses emission
	of the Changed signal until endUpdate is called, so that a number of
	attributes may be updated en-masse, generating just one signal. */
	bool _inUpdate;

        /** This flag should be set when the server notifies that this entity
        has a bounding box. If this flag is not true, the contents of the
        BBox attribute are undefined.  */
        bool _hasBBox;
	
	/** When a batched property update is in progress, the set tracks the names
	of each modified property. This set is passed as a parameter of the Changed
	callback when endUpdate is called, to allow clients to determine what
	was changed. */
	StringSet _modified;

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

	World *_world; // the World that created the Entity
};


// motion predicted entity that moves a lot.
class Moveable : public Entity
{
	typedef Entity Inherited;		
public:
	Moveable(const std::string &id);
	virtual ~Moveable();

	virtual WFMath::Point<3> getPosition() const	{return Inherited::getPosition();} 
	void getPosition(bool predicted);

protected:
	WFMath::Vector<3> _velocity,
		_delta;
};

class UnknownProperty : public InvalidOperation
{
public:	
	UnknownProperty(const std::string &p, const std::string &m) :
		InvalidOperation(m), prop(p)
	{;}
        virtual ~UnknownProperty() throw () { }
	
	const std::string prop;
};

} // of namespace

#endif
