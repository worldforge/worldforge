#ifndef ERIS_PLAYER_H
#define ERIS_PLAYER_H

#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Entity/Player.h>

#include "Types.h"
#include "Timeout.h"

namespace Eris
{
	
// Forward Declerations
class Lobby;
class Connection;
class World;
class Avatar;

/** Enumeration of various server-side things that can go wrong when trying
to create an account or login.*/
typedef enum {
	LOGIN_INVALID = 0,
	LOGIN_DUPLICATE_ACCOUNT,	///< The requested account already exists (when creating)
	LOGIN_BAD_ACCOUNT,		///< The account data was invalid
	LOGIN_UNKNOWN_ACCOUNT,	///< The account was not found (logging in)
	LOGIN_BAD_PASSWORD,		///< The supplied password do not match
	LOGIN_DUPLICATE_CONNECT	///< The account is already active (not always an error)
} LoginFailureType;

/** List type used to return character info to the client. */
typedef std::list<Atlas::Objects::Entity::GameEntity> CharacterList;

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
	/** create a new Player object : currently only one is assumed, but multiple might be supported
	in the future */
	Player(Connection *con);
	~Player();

	/// Login to the server using the existing account specified
	/// @param uname The username of the account
	/// @param pwd The correct password for the account
	
	/** Server-side failures during the login process, such as the account being unknown
	or an incorrect password being supplied, will result in the 'LoginFailure' signal being
	emitted with some vaugely helpful error message, and an error code. The LoginSuccess
	signal will be emitted upon sucessful completion of the login process. */

	void login(const std::string &uname,
		const std::string &pwd);

	/** Attempt to create a new account on the server and log into it.
	Server-side failures, such as an account already existing with the specified
	username, will cause the 'LoginFailure' signal to be emitted with an error message
	and a code. As for 'login', LoginSuccess wil be emitted if everything goes as planne. */

	/// @param uname The desired username of the account (eg 'bryceh')
	/// @param name The real name of the user (e.g 'Bryce Harrington')
	/// @param pwd The password for the new account

	void createAccount(const std::string &uname,
		const std::string &name,
		const std::string &pwd);
	
	/** Initiate a clean disconnection from the server. The LogoutComplete
	signal will be emitted when the process completes */
	void logout();

	/** access the characters currently owned by the player  : note you should call
	refreshCharacterInfo, and wait for 'GotCharacters' signal, prior to the
	initial call : otherwise, it may return an empty or incomplete list. */
	CharacterList getCharacters();

	/** update the charcter list (based on changes to play). The intention here is
	that clients will call this method when the user invokes the 'choose character' command,
	and wait for the 'GotCharacters signal before displaying. Alternatively, you can
	display the UI immediatley, and add character entries based on the 'GotCharacterInfo'
	signal, which will be emitted once for each character. */
	void refreshCharacterInfo();

	/// enter the game using an existing character
	/// @param id The Atlas-ID of the game entity to take-over; this must be owned by the player's account
	Avatar* takeCharacter(const std::string &id);

	/// enter the game using a new character
	Avatar* createCharacter(const Atlas::Objects::Entity::GameEntity &character);

	/// returns the account ID if logged in, or throws and exception
	std::string getAccountID() const;

// signals
	/// emitted when a character has been retrived from the server
	SigC::Signal1<void, const Atlas::Objects::Entity::GameEntity&> GotCharacterInfo;
	
	/// emitted when the entire character list had been updated
	SigC::Signal0<void> GotAllCharacters;
	
	/// emitted when a server-side error occurs during account creation / login
	SigC::Signal2<void, LoginFailureType, const std::string &> LoginFailure;
	
	SigC::Signal0<void> LoginSuccess;
	
	/** emitted when a LOGOUT operation completes; either cleanly (argument = true),
	indicating the LOGOUT was acknowledged by the server, or due to a timeout
	or other error (argument = false) */
	SigC::Signal1<void, bool> LogoutComplete;
protected:
	void recvOpError(const Atlas::Objects::Operation::Error &err);	
	void recvSightCharacter(const Atlas::Objects::Entity::GameEntity &ge);
	void loginComplete(const Atlas::Objects::Entity::Player &p);

	void internalLogin(const std::string &unm, const std::string &pwd);

	/// Callback for network re-establishment
	void netConnected();
	
	/// help! the plug is being pulled!
	bool netDisconnecting();
	void netFailure(const std::string& msg);

	void recvLogoutInfo(const Atlas::Objects::Operation::Logout &lo);
	void handleLogoutTimeout();

	Connection* _con;	///< underlying connection instance
	std::string _account;	///< account ID (the username, at present)

	CharacterList _characters;	///< charatcers belonging to this player
	StringList _charIds;

	std::string _username,	///< The player's username ( != account object's ID)
		_pass;		///< The password; FIXME - clear text.

	/// current action tracking (for error processing)
	std::string _currentAction;
	long _currentSerial;	///< serial no of the Atlas operation

	Timeout* _logoutTimeout;

	Lobby* _lobby;
};
	
} // of namespace Eris

#endif
