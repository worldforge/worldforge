#ifndef ERIS_LOBBY_H
#define ERIS_LOBBY_H

#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

#include <Atlas/Objects/Entity/Player.h>
#include <Atlas/Objects/Operation/Info.h>

#include "Types.h"
#include "Connection.h"
#include "Room.h"

// forward decleration of test cases
class TestLobby;

namespace Eris
{
	
class Player;
class Person;
	
/** Lobby is the Out-of-Game session object, valid from connection to the server until disconnection.
*/
class Lobby : public Room
{
public:	
	Lobby(Connection *c); 
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
	
	// emitted when someone does a private (direct) chat
	SigC::Signal2<void, const std::string&, const std::string&> PrivateTalk;
	
	/// Emitted when initial login completes
	SigC::Signal1<void, const Atlas::Objects::Entity::Player&> LoggedIn;
	
	/** Access the global Lobby instance. Do not call this prior to creating a Connection object, or
	InvalidOperation will be thrown; the Lobby needs to be bound to a connection when it is
	initalised, and this method creates the Lobby if an existing instance is not found. */
	static Lobby* getPrimary() {return Connection::getPrimary()->getLobby();}
	/// deprecated
	static Lobby* instance() {return getPrimary();}
protected:
	friend class Room;
	friend class Player; 
	
	friend class TestLobby;	///< allows tests to set things up
	
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
	
	void recvSightCreate(const Atlas::Objects::Operation::Create &cr,
	    const Atlas::Objects::Entity::RootEntity &ent);

    /// internal helper when a room creation is observed
    void processRoomCreate(const Atlas::Objects::Operation::Create &cr,
	const Atlas::Objects::Entity::RootEntity &ent);

    /// delayed registration of callbacks (until we have a valid account ID)
    void registerCallbacks();

    /** register that the lobby should expect to observer the creation of a room with the
    given refno, and it should pass the data on the the given Room object. This is an internal
    helpfer for Room::createChild */
    void addPendingCreate(Room *r, int serialno);

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
	
    typedef std::map<int, Room*> PendingCreateMap;
    /// map from the serialno of a CREATE operation to the corresponding Room instance it applies to
    PendingCreateMap _pendingCreate;
};
	
} // of namespace Eris

#endif
