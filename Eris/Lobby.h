#ifndef ERIS_LOBBY_H
#define ERIS_LOBBY_H

#include <Eris/Room.h>

namespace Eris
{
	
class Player;
class Person;
class Connection;

/** Lobby is the Out-of-Game session object, valid from connection to the server until disconnection.
*/
class Lobby : public Room
{
public:	
    Lobby(Player *pl); 
    virtual ~Lobby();

    /// Join the specified room; throws if not allowed or not found
    Room* join(const std::string &roomID);

    /// obtain a person's info, given their account ID; may return NULL
    Person* getPerson(const std::string &acc);
		
    /// obtain a Room object, given the id; may return NULL
    Room* getRoom(const std::string &id);

    Player* getAccount() const
    {
        return m_account;
    }
    
    Connection* getConnection() const;

    // callbacks
    /// Emitted when sight of a person is received
    SigC::Signal1<void, Person*> SightPerson;
    
    // emitted when someone does a private (direct) chat
    SigC::Signal2<void, Person*, const std::string&> PrivateTalk;
	
protected:
    friend class Room;
    friend class OOGRouter;
    
    void look(const std::string &id);
		
    void sightPerson(const Atlas::Objects::Entity::Account &ac);
    Router::RouterResult recvTalk(const Atlas::Objects::Operation::Talk& tk);
    void recvInitialSight(const Atlas::Objects::Entity::RootEntity& ent);
        
    void recvAppearance(const Atlas::Objects::Root& obj);
    void recvDisappearance(const Atlas::Objects::Root& obj);
        
private:
    void onLoggedIn();
    void onLogout(bool clean);
    
    Player* m_account;    
    IdPersonMap m_people;
	
    typedef std::map<std::string, Room*> IdRoomMap;
    IdRoomMap m_rooms;

    OOGRouter* m_router;
};
	
} // of namespace Eris

#endif
