#ifndef ERIS_PLAYER_H
#define ERIS_PLAYER_H

#include <sigc++/object.h>
#include <sigc++/basic_signal.h>

#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Entity/Player.h>

#include "Types.h"

namespace Eris
{
	
// Forward Declerations
class Lobby;
class Connection;
class World;

/// Per-player instance, for entire lifetime; abstracted to permit (in theorey) multiple players per client

/** A Player object represents the encapsulation of a server account, and it's binding to a character in the
game world. Future versions of Eris will support mutliple Player objects per Connection, allowing various
configurations of interface, proxies and so forth.
<br>
Player is also the mechanism by which Lobby and World objects are made available to the client,
in response to login / create operations */

class Player : public SigC::Object
{
public:
	/// create a new Player
	Player();
	~Player();

	/// Login to the server using the existing account specified
	/// @param uname The username of the account
	/// @param pwd The correct password for the account
	
	/** Server-side failures during the login process, such as the account being unknown
	or an incorrect password being supplied, will result in an exception being thrown */

	Lobby* login(Connection *con, 
		const std::string &uname,
		const std::string &pwd);

	/** Attempt to create a new account on the server and log into it.
	Server-side failures, such as an account already existing with the specified
	username, will throw an exception. state which ones! */

	/// @param con The connection object to the server
	/// @param uname The desired username of the account (eg 'bryceh')
	/// @param name The real name of the user (e.g 'Bryce Harrington')
	/// @param pwd The password for the new account

	Lobby* createAccount(Connection *con,
		const std::string &uname,
		const std::string &name,
		const std::string &pwd);
	
	/// Initiate a clean disconnection from the server
	void logout();

	/// access the characters currently owned by the player
	StringList getCharacters();

	/// request the entity corresponding to a character
	/** Used to request the entity corresponding to a character owned by the player; the response will be 
	a BrowseCharatcer signal containing the entity data. This allows clients to display a character browser
	with the name, race, location and so on of characters, prior to entering the game. */
	void requestCharacter(const std::string &id);

	/// enter the game using an existing character
	/// @param id The Atlas-ID of the game entity to take-over; this must be owned by the player's account
	World* takeCharacter(const std::string &id);

	/// enter the game using a new character
	World* createCharacter(const Atlas::Objects::Entity::GameEntity &character);

	/// returns the account ID if logged in, or throws and exception
	std::string getAccountID() const;

	/// emitted when a character has been retrived from the server
	SigC::Signal1<void, const Atlas::Objects::Entity::GameEntity&> BrowseCharacter;
protected:
	void recvOpError(const Atlas::Objects::Operation::Error &err);	
	void recvSightCharacter(const Atlas::Objects::Entity::GameEntity &ge);
	void loginComplete(const Atlas::Objects::Entity::Player &p);

	void internalLogin(const string &unm, const string &pwd);

	/// Callback for network re-establishment
	void netConnected();
	
	/// help! the plug is being pulled!
	bool netDisconnecting();

	Connection* _con;	///< underlying connection instance
	std::string _account;	///< account ID (the username, at present)

	StringList _characters;	///< entity IDs of charatcers belonging to this player

	std::string _username,	///< The player's username ( != account object's ID)
		_pass;		///< The password; FIXME - clear text.

	// current action tracing (for error processing)
	std::string _currentAction;
	long _currentSerial;	///< serial no of the Atlas operation

	Lobby* _lobby;
	World* _world;
};
	
} // of namespace Eris

#endif
