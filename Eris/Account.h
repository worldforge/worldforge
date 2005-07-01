#ifndef ERIS_PLAYER_H
#define ERIS_PLAYER_H

#include <vector>

#include <Eris/Types.h>
#include <Eris/Timeout.h>
#include <sigc++/object.h>
#include <Atlas/Objects/ObjectsFwd.h>

namespace Eris
{
	
class Connection;
class Avatar;
class AccountRouter;

/** Type used to return available characters */
typedef std::map<std::string, Atlas::Objects::Entity::GameEntity> CharacterMap;

/// Encapsulates all the state of an Atlas Account, and methods that operation on that state

/** An Account object represents the encapsulation of a server account, and it's binding to a character in the
game world. Future versions of Eris will support multiple Account objects per Connection, allowing various
configurations of interface, proxies and so forth.
<br>
Account is also the mechanism by which Lobby and Avatars objects are made available to the client,
in response to login / create operations */

class Account : virtual public SigC::Object
{
public:
    /// Create a new Account associated with a Connection object
    /**
    Create a new Account object : currently only one is assumed, but multiple
    Accounts might be supported in the future 
    @param con A valid (but not necessarily connected) Connection instance
    */
    Account(Connection *con);
   
     ~Account();

    /// Login to the server using user-supplied account information
    /** This is the basic way of logging into an existing account. Server-side 
    failures during the login process, such as the account being unknown
    or an incorrect password being supplied, will result in the 'LoginFailure' signal being
    emitted with some vaugely helpful error message, and an error code. The LoginSuccess
    signal will be emitted upon sucessful completion of the login process. 
    
    @param uname The username of the account
    @param pwd The correct password for the account
    */
    Result login(const std::string &uname, const std::string &pwd);

    /// Attempt to create a new account on the server and log into it.
    /* Create a new account on the server, if possible.
    Server-side failures, such as an account already existing with the specified
    username, will cause the 'LoginFailure' signal to be emitted with an error message
    and a code. As for 'login', LoginSuccess wil be emitted if everything goes as plan. 
   
    @param uname The desired username of the account (eg 'ajr')
    @param fullName The real name of the user (e.g 'Al Riddoch')
    @param pwd The plaintext password for the new account
    */

    Result createAccount(const std::string &uname,
        const std::string &fullName,
        const std::string &pwd);
	
    /// Request logout from the server.
    /** Initiate a clean disconnection from the server. The LogoutComplete
    signal will be emitted when the process completes. Calling this on an Account
    which is not logged in will produce an error. */
    Result logout();

    /// Check if the account is logged in.
    /** Many operations will produce errors if the account is not logged in. */
    bool isLoggedIn() const;
	
	/// Returns a container of character types that the client is allowed to create.
	const std::vector< std::string > & getCharacterTypes(void) const;
    
    /// Get the characters owned by this account.
    /**
    Note you should call
    refreshCharacterInfo, and wait for the GotAllCharacters signal, prior to the
    initial call : otherwise, it will return an empty or incomplete list.
    */
    const CharacterMap& getCharacters();

    /**
    Update the charcter list (based on changes to play). The intention here is
    that clients will call this method for some kind of'choose character' interface
    or menu, and wait for the GotAllCharacters signal before displaying the list. 
    Alternatively, you can display the UI immediatley, and add character entries
    based on the GotCharacterInfo signal, which will be emitted once for each character. 
    */
    Result refreshCharacterInfo();

    /// Enter the game using an existing character
    /**
    @param id The id of the game entity to activate; this must be owned by the account.
    @result The Avatar that represents the character. Note ownership of this passes to
        the caller.
    */
    Result takeCharacter(const std::string &id);

    /// enter the game using a new character
    Result createCharacter(const Atlas::Objects::Entity::GameEntity &character);

	/// pop up the game's character creation dialog, if present
	//void createCharacter();

	///  returns true if the game has defined a character creation dialog
	bool canCreateCharacter() {return false;}

    

    /// returns the account ID if logged in
    const std::string& getId() const
    {
        return m_accountId;
    }

    /** Return the username of this account. */
    const std::string& getUsername() const
    { return m_username; }

    /// Access the underlying Connection for this account
    Connection* getConnection() const 
    {
        return m_con;
    }

// signals
    /// emitted when a character has been retrived from the server
    SigC::Signal1<void, const Atlas::Objects::Entity::GameEntity&> GotCharacterInfo;
    
    /// emitted when the entire character list had been updated
    SigC::Signal0<void> GotAllCharacters;
    
    ///  Emitted when a server-side error occurs during account creation / login.
    /**
    The argument is an error message from the server - hopefully this will 
    become something more useful such as an enum code, in the future.
    */
    SigC::Signal1<void, const std::string &> LoginFailure;
    
    /** Emitted when login or character creation is successful. */
    SigC::Signal0<void> LoginSuccess;
    
    /// Emitted when a logout completes
    /** Depending on whether the logout completed with a positive server
    acknowledgement or just timedout, the argument will be either true
    (success, clean logout) or false (failure, timeout or other problem)
    */
    SigC::Signal1<void, bool> LogoutComplete;

    /**
    Emitted when creating a character or taking an existing one
    succeeds.
    */
    SigC::Signal1<void, Avatar*> AvatarSuccess;

    SigC::Signal1<void, const std::string &> AvatarFailure;

protected:
    friend class AccountRouter;
    
    void sightCharacter(const Atlas::Objects::Operation::RootOperation& op);
    
    void loginComplete(const Atlas::Objects::Entity::Account &p);
    void loginError(const Atlas::Objects::Operation::Error& err);

    Result internalLogin(const std::string &unm, const std::string &pwd);
    void internalLogout(bool clean);

	/// Callback for network re-establishment
	void netConnected();
	
	/// help! the plug is being pulled!
	bool netDisconnecting();
	void netFailure(const std::string& msg);

    void loginResponse(const Atlas::Objects::Operation::RootOperation& op);
    void logoutResponse(const Atlas::Objects::Operation::RootOperation& op);
    void avatarResponse(const Atlas::Objects::Operation::RootOperation& op);
    
	void handleLogoutTimeout();
//	void recvRemoteLogout(const Atlas::Objects::Operation::Logout &lo);

    void handleLoginTimeout();
    
    typedef enum
    {
        DISCONNECTED = 0,   ///< Default state, no server account active
        LOGGING_IN,         ///< Login sent, waiting for initial INFO response
        LOGGED_IN,          ///< Fully logged into a server-side account
        LOGGING_OUT,         ///< Sent a logout op, waiting for the INFO response
        
        TAKING_CHAR,        ///< sent a LOOK op for a character, awaiting INFO response
        CREATING_CHAR       ///< send a character CREATE op, awaiting INFO response
    } Status;
        
private:
    Connection* m_con;	///< underlying connection instance
    Status m_status;    ///< what the Player is currently doing
    AccountRouter* m_router;
    
    std::string m_accountId;	///< the account ID
    std::string m_username;	///< The player's username ( != account object's ID)
    std::string m_pass;
    
	std::vector< std::string > m_characterTypes;
    CharacterMap _characters;	///< characters belonging to this player
    StringSet m_characterIds;
    bool m_doingCharacterRefresh; ///< set if we're refreshing character data
    
    std::auto_ptr<Timeout> m_timeout;
};
	
} // of namespace Eris

#endif
