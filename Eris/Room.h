#ifndef ERIS_ROOM_H
#define ERIS_ROOM_H

#include <sigc++/object.h>
#include <sigc++/object_slot.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

namespace Atlas { namespace Objects {

	namespace Operation {
		class Imaginary;
	}
	
}}

#include "Types.h"

namespace Eris
{
	
// forward decls
class Person;
class Lobby;

/** The out-of-game (OOG) heirarchy is composed of Rooms, containing Persons and other
Rooms. Generally rooms corespond to chanels in IRC, and the interface and commands should
be clear if you are familiar with that medium. 
*/
class Room : public SigC::Object
{
public:	
	virtual ~Room();

	/// Send a piece of text to this room
	void say(const std::string &tk);

	/** Send an emote ( /me ) to the room. This is transmitted as an IMAGINARY op
	in Atlas, with args[0]["id"] = "emote". */
	void emote(const std::string &em);

	/** Leave the room - no more signals will be emitted for this room again
	(validity of Room pointer after this call?) */
	void leave();

    /** create a child room of this one, with the specified name. Note that most attributes,
    <em>including the ID</em> will not be valid until the new room emits the 'Entered' signal.
    If you need a unique, referenceable indentifier earlier than that point, use the pointer
    value. */
    Room* createRoom(const std::string &name);

	/// Obtain the human-readable name  of this room
	std::string getName() const
	{ return _name; }
	
	/// Obtain a list of the account IDs of each person in this room
	StringList getPeople() const
	{ return StringList(_people.begin(), _people.end()); }
	
	/// Obtain a list of rooms within this room
	StringList getRooms() const
	{ return StringList(_subrooms.begin(), _subrooms.end()); }
	
	/** Get the Atlas object ID of the Room; note this can fail prior to the Entered signal
	 * being emitted; in that case it throws an exception to avoid returning an invalid
	 * ID */
	std::string getID() const;
	
	/// Called by the lobby when sight of us arrives	
	void sight(const Atlas::Objects::Entity::RootEntity &room);	
// signals
	/** Emitted when entry into the room (after a Join) is complete, i.e the user list has been
	transferred and resolved. */
	SigC::Signal1<void, Room*> Entered;
	
	/** The primary talk callback. The arguments are the source room, the person
	talking, and the message itself */
	SigC::Signal3<void, Room*, const std::string&, const std::string&> Talk;
	
	/** Emote (/me) callback. The arguments are identical to those for Talk above */
	SigC::Signal3<void, Room*, const std::string&, const std::string&> Emote;
	
	/** Emitted when a person enters the room; argument is the account ID. Note that
	Appearance is not generated for the local player when entering/leaving; use the
	Entered signal instead. */
	SigC::Signal2<void, Room*, const std::string&> Appearance;
	/// Similarly, emitted when the specifed person leaves the room
	SigC::Signal2<void, Room*, const std::string&> Disappearance;
	
	/** Emitted when a room change occurs, such as name, topic, or the list of subrooms */
	SigC::Signal1<void, const StringSet&> Changed;
	
protected:
	friend class Lobby;	// so Lobby can call the constructor
		
	/** standard constructor. Issues a LOOK against the specified ID, and sets up
	the necessary signals to drive the Room */
	Room(Lobby *l, const std::string &id);

	/// base-constructor for derived classes (lobby)
	explicit Room(Lobby *l);	

	/**  delayable initialization - allows the Lobby to defer signal binding and so on until
	the account INFO has been recieved. */
	void setup();

	/// routed from the Lobby, which maintains the actual dispatcher
	void notifyPersonSight(Person *p);

	// Callbacks
	void recvSoundTalk(const Atlas::Objects::Operation::Talk &tk);
	void recvAppear(const Atlas::Objects::Operation::Appearance &ap);
	void recvDisappear(const Atlas::Objects::Operation::Disappearance &dis);
	void recvSightEmote(const Atlas::Objects::Operation::Imaginary &imag,
		const Atlas::Objects::Root &emote);	

	std::string _id;	///< ID of the room entity
	Lobby* _lobby;		///< the Lobby object
	bool _parted;		///< Set if 'left'

	std::string _name,	///< displayed named of the room
		_creator;	///< ID of the account that created the room
	bool _initialGet;

	StringSet _subrooms;	///< the IDs of any sub-rooms
	StringSet _people,		///< the account IDs of each person in the room
		_pending;	///< persons for which appear/disappear has been received, but not (yet) SIGHT 
};
	
}

#endif
