#ifndef ERIS_ROOM_H
#define ERIS_ROOM_H

#include <sigc++/object.h>
#include <sigc++/basic_signal.h>

#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

#include "Types.h"

namespace Eris
{
	
// forward decls
class Person;
class Lobby;
	
class Room : public SigC::Object
{
public:	
	Room(Lobby *l, const std::string &id);
	virtual ~Room();

	/// Send a piece of text to this room
	void say(const std::string &tk);

	/// Leave the room
	void leave();

	/// Obtain the human-readable name  of this room
	std::string getName() const
	{ return _name; }
	
	/// Obtain a list of the account IDs of each person in this room
	StringList getPeople() const
	{ return StringList(_people.begin(), _people.end()); }
	
	/// Obtain a list of rooms within this room
	StringList getRooms() const
	{ return StringList(_subrooms.begin(), _subrooms.end()); }
	
	/// Get the Atlas object ID
	std::string getID() const {return _id;}
	
	/// Called by the lobby when sight of us arrives	
	void sight(const Atlas::Objects::Entity::RootEntity &room);	
// signals
	SigC::Signal1<void, Room*> Entered;
	SigC::Signal3<void, Room*, std::string, std::string> Talk;
	
	/// Emitted when a person enters the room; argument is the account ID
	SigC::Signal2<void, Room*, std::string> Appearance;
	SigC::Signal2<void, Room*, std::string> Disappearance;
	
protected:
	/// base-construcutore for derived classes (lobby)
	explicit Room(Lobby *l);	

	///  delayable initialization
	void setup();

	// Callbacks
	void notifyPersonSight(Person *p);
	
	void recvSoundTalk(const Atlas::Objects::Operation::Talk &tk);
	void recvAppear(const Atlas::Objects::Operation::Appearance &ap);
	void recvDisappear(const Atlas::Objects::Operation::Disappearance &dis);
	
	std::string _id;	///< ID of the room entity
	Lobby* _lobby;		///< the Lobby object
	bool _parted;		///< Set if 'left'

	std::string _name,	///< displayed named of the room
		_creator;	///< ID of the account that created the room
	bool _initialGet;

	StringSet _subrooms;
	StringSet _people,
		_pending;
};
	
}

#endif
