#ifndef ERIS_PLAYER_H
#define ERIS_PLAYER_H

#include <Eris/Types.h>
#include <Eris/Timeout.h>
#include <sigc++/object.h>
#include <Atlas/Objects/ObjectsFwd.h>

namespace Eris
{
	
class Connection;
class Avatar;

/** Type used to return available characters */
typedef std::map<std::string, Atlas::Objects::Entity::GameEntity> CharacterMap;

/// Per-player instance, for entire lifetime; abstracted to permit (in theorey) multiple players per client

/** A Player object represents the encapsulation of a server account, and it's binding to a character in the
game world. Future versions of Eris will support mutliple Player objects per Connection, allowing various
configurations of interface, proxies and so forth.
<br>
Player is also the mechanism by which Lobby and World objects are made available to the client,
in response to login / create operations */

class Player : virtual public SigC::Object
{
public:
    /** create a new Player object : currently only one is assumed, but multiple
    might be supported in the future */
    Player(Connection *con);
    ~Player();

    /// Login to the server using the existing account specified
    /// @param uname The username of the account
    /// @param pwd The correct password for the account
    
    /** Server-side failures during the login process, such as the account being unknown
    or an incorrect password being supplied, will result in the 'LoginFailure' signal being
    emitted with some vaugely helpful error message, and an error code. The LoginSuccess
    signal will be emitted upon sucessful completion of the login process. */

    void login(const std::string &uname, const std::string &pwd);

    /** Attempt to create a new account on the server and log into it.
    Server-side failures, such as an account already existing with the specified
    username, will cause the 'LoginFailure' signal to be emitted with an error message
    and a code. As for 'login', LoginSuccess wil be emitted if everything goes as planne. */

    /// @param uname The desired username of the account (eg 'ajr')
    /// @param fullName The real name of the user (e.g 'Al Riddoch')
    /// @param pwd The password for the new account

    void createAccount(const std::string &uname,
        const std::string &fullName,
        const std::string &pwd);
	
    /** Initiate a clean disconnection from the server. The LogoutComplete
    signal will be emitted when the process completes */
    void logout();

    /** check if this account is logged in sucessfully. Many operations
    will produce errors if the account is not logged in. */
    bool isLoggedIn() const
    {
        return m_status == LOGGED_IN;
    }

    /** access the characters currently owned by the player  : note you should call
    refreshCharacterInfo, and wait for 'GotCharacters' signal, prior to the
    initial call : otherwise, it may return an empty or incomplete list. */
    const CharacterMap& getCharacters();

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

	/// pop up the game's character creation dialog, if present
	//void createCharacter();

	/////  returns true if the game has defined a character creation dialog
	bool canCreateCharacter() {return false;}

    /// returns the account ID if logged in
    const std::string& getID() const
    {
        return m_accountId;
    }

    Connection* getConnection() const 
    {
        return m_con;
    }

// signals
    /// emitted when a character has been retrived from the server
    SigC::Signal1<void, const Atlas::Objects::Entity::GameEntity&> GotCharacterInfo;
    
    /// emitted when the entire character list had been updated
    SigC::Signal0<void> GotAllCharacters;
    
    /// emitted when a server-side error occurs during account creation / login
    SigC::Signal1<void, const std::string &> LoginFailure;
    
    SigC::Signal0<void> LoginSuccess;
    
    /** emitted when a LOGOUT operation completes; either cleanly (argument = true),
    indicating the LOGOUT was acknowledged by the server, or due to a timeout
    or other error (argument = false) */
    SigC::Signal1<void, bool> LogoutComplete;

    /// emitted when a character is created by the zero-argument createCharacter()
    SigC::Signal1<void, Avatar*> NewCharacter;
protected:
    friend class AccountRouter;
    
    void sightCharacter(const Atlas::Objects::Entity::GameEntity &ge);
    
    void loginComplete(const Atlas::Objects::Entity::Player &p);
    void loginError(const Atlas::Objects::Operation::Error& err);

    void internalLogin(const std::string &unm, const std::string &pwd);
    void internalLogout(bool clean);

	/// Callback for network re-establishment
	void netConnected();
	
	/// help! the plug is being pulled!
	bool netDisconnecting();
	void netFailure(const std::string& msg);

	void recvLogoutInfo(const Atlas::Objects::Operation::Logout &lo);
	void handleLogoutTimeout();
	void recvRemoteLogout(const Atlas::Objects::Operation::Logout &lo);

	void createCharacterHandler(long serialno);

	Timeout* _logoutTimeout;
    
    typedef enum
    {
        DISCONNECTED = 0,   ///< default state
        LOGGING_IN,
        LOGGED_IN,
        LOGGING_OUT
    } Status;
        
private:
    Connection* m_con;	///< underlying connection instance
    Status m_status;    ///< what the Player is currently doing
    AccountRouter* m_router;
    
    std::string m_accountId;	///< the account ID
    std::string m_username;	///< The player's username ( != account object's ID)
    std::string m_pass;
    
    CharacterMap _characters;	///< charatcers belonging to this player
    StringSet m_characterIds;
    bool m_doingCharacterRefresh; ///< set if we're refreshing character data
};
	
} // of namespace Eris

#endif
