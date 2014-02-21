#ifndef ERIS_LOBBY_H
#define ERIS_LOBBY_H

#include "Room.h"

namespace Eris
{
	
class Account;
class Person;
class Connection;
class OOGRouter;

/**
Lobby is the Out-of-Game session object, valid from connection to the server
until disconnection. It manages the entire OOG structure, which consists of
rooms and people. People can be in more than one room (this is the big
difference to in-game). Note the entire OOG system remains live and valid
even when an Avatar is active, it's up to the client to present some UI
for accessing the OOG system if it wants.

Of course, nothing requries to create a Lobby at all, Account and the in-game
structure will work perfectly well without one.
*/
class Lobby : public Room
{
public:	
    /** Create a Lobby for the specified account, and retrive the initial
    OOG structure if the Account is logged in and connected. */
    Lobby(Account *acc); 
    
    /** Delete the Lobby, including all it's Rooms and Persons. */
    virtual ~Lobby();

    /** Join the specified room, or return NULL if an error occurs. Note
    the Room will not be valid until it emits the Entered signal. */
    Room* join(const std::string &roomID);

    /// obtain a person's info, given their account ID; may return NULL
    Person* getPerson(const std::string &acc);
		
    /**
    Obtain a Room object, given the rooms' id. This will return NULL if the
    id is invalid.
    */
    Room* getRoom(const std::string &id);

    /// Retrive the Account which this lobbby is bound to
    Account* getAccount() const
    {
        return m_account;
    }
    
    /// Helper method to access the underlying Connection from the Account
    Connection* getConnection() const;

// callbacks
    /// Emitted when sight of a person is received
    sigc::signal<void, Person*> SightPerson;
    
    /**
    Emitted when some person sends a private (one-to-one) chat message
    to the client's account. The first argument is the sender, which will
    always be a valid Person object, and the second is the message text.
    */
    sigc::signal<void, Person*, const std::string&> PrivateTalk;
	
protected:
    friend class Room;
    friend class OOGRouter;
    
    void look(const std::string &id);
		
    void sightPerson(const Atlas::Objects::Entity::Account &ac);
    Router::RouterResult recvTalk(const Atlas::Objects::Operation::Talk& tk);
    void recvInitialSight(const Atlas::Objects::Entity::RootEntity& ent);
        
    void recvAppearance(const Atlas::Objects::Root& obj);
    void recvDisappearance(const Atlas::Objects::Root& obj);
    
    Router::RouterResult recvImaginary(const Atlas::Objects::Operation::Imaginary& im);
    
private:
    void onLoggedIn();
    void onLogout(bool clean);
    
    Account* m_account;    
    IdPersonMap m_people;
	
    typedef std::unordered_map<std::string, Room*> IdRoomMap;
    IdRoomMap m_rooms;

    OOGRouter* m_router;
};
	
} // of namespace Eris

#endif
