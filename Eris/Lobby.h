#ifndef ERIS_LOBBY_H
#define ERIS_LOBBY_H

#include <sigc++/object.h>
#include <sigc++/basic_signal.h>

#include <Atlas/Objects/Entity/Player.h>
#include <Atlas/Objects/Operation/Info.h>

#include "Types.h"
#include "Connection.h"
#include "Room.h"

namespace Eris
{
	
class Player;
class Person;
	
/** Lobby is the Out-of-Game session object, valid from connection to the server until disconnection.
*/
class Lobby : public Room
{
public:	
	~Lobby();

	/// Join the specified room; throws if not allowed or not found
	Room* join(const std::string &roomID);

	//Room* Create(const string &nm);

	/// obtain a person's info, given their account ID; may return NULL!
	Person* getPerson(const std::string &acc);
		
	/// obtain a Room object, given the id
	Room* getRoom(const std::string &id);

	/// get the player's account ID
	std::string getAccountID();
	
	/// The underlying connection for the lobby
	Connection* getConnection() const { return _con; }

	// callbacks
	/// Emitted when sight of a person is received
	SigC::Signal1<void, Person*> SightPerson;
	
	/// Emitted when sight of a room is recieved
	//SigC::Signal1<void Room*> SightRoom;
	SigC::Signal2<void, const std::string&, const std::string&> PrivateTalk;
	/// Emitted when initial login completes
	SigC::Signal1<void, const Atlas::Objects::Entity::Player&> LoggedIn;
	
	/** Access the global Lobby instance. Do not call this prior to creating a Connection object, or
	InvalidOperation will be thrown; the Lobby needs to be bound to a connection when it is
	initalised, and this method creates the Lobby if an existing instance is not found. */
	static Lobby* instance();
protected:
	friend class Room;
	friend class Player; 
	
	Lobby(/*Player *p,*/ Connection *c);
	
	void look(const std::string &id);
	void expectInfoRefno(long ref);
	
	// callbacks	
	void recvInfoAccount(const Atlas::Objects::Operation::Info &ifo, 
		const Atlas::Objects::Entity::Account &account);
	
	void recvSightPerson(const Atlas::Objects::Entity::Account &ac);
	void recvSightRoom(const Atlas::Objects::Entity::RootEntity &room);
	// FIXME - remove once cyphesis is updated
	void recvSightLobby(const Atlas::Objects::Entity::RootEntity &lobby);
	
	void recvPrivateChat(const Atlas::Objects::Operation::Talk &tk);
	
	/// delayed registration of callbacks (until we have a valid account ID)
	void registerCallbacks();

	// should this try a shutdown, or simply wait and hope for resume?
	void netFailure(const std::string& msg);
	void netConnected();
	void netDisconnected();
	
	std::string _account;
	Connection* _con;
	//Player* _player;
	bool _reconnect;	///< set if reconnecting (for INFO processing)
	long _infoRefno;	///< refno of the INFO op we're looking for
	
	typedef std::map<std::string, Person*> PersonDict;
	PersonDict _peopleDict;
	
	typedef std::map<std::string, Room*> RoomDict;
	RoomDict _roomDict;
	
	static Lobby* _theLobby;
};
	
} // of namespace Eris

#endif
