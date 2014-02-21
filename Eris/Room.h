#ifndef ERIS_ROOM_H
#define ERIS_ROOM_H

#include "Router.h"

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <vector>
#include <unordered_map>
#include <string>

namespace Eris
{
	
// forward decls
class Person;
class Lobby;

/** The out-of-game (OOG) heirarchy is composed of Rooms, containing Persons and other
Rooms. Generally rooms corespond to chanels in IRC, and the interface and commands should
be clear if you are familiar with that medium. 
*/
class Room : public sigc::trackable, public Router
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
    {
        return m_name;
    }
	
    std::string getTopic() const
    {
        return m_topic;
    }
    
    /// obtain an array of pointers to everyone in this room
    std::vector<Person*> getPeople() const;
    	
    /// Obtain a list of rooms within this room
    std::vector<Room*> getRooms() const
    {
        return m_subrooms;
    }
	
    /** Get the Atlas object ID of the Room; note that this may return an
    empty value if called prior to entering the Lobby */
    std::string getId() const
    {
        return m_roomId;
    }
    
    Person* getPersonByUID(const std::string& uid);
		
// signals
    /** Emitted when entry into the room (after a Join) is complete, i.e the user list has been
    transferred and resolved. */
    sigc::signal<void, Room*> Entered;
    
    /** The primary talk callback. The arguments are the source room, the person
    talking, and the message itself */
    sigc::signal<void, Room*, Person*, const std::string&> Speech;
    
    /** Emote (/me) callback. The arguments are identical to those for Talk above */
    sigc::signal<void, Room*, Person*, const std::string&> Emote;
	
    /** Emitted when a person enters the room; argument is the account ID. Note that
    Appearance is not generated for the local player when entering/leaving; use the
    Entered signal instead. */
    sigc::signal<void, Room*, Person*> Appearance;
    
    /// Similarly, emitted when the specifed person leaves the room
    sigc::signal<void, Room*, Person*> Disappearance;
	
	
protected:
    friend class Lobby;

    typedef std::unordered_map<std::string, Person*> IdPersonMap;
    
    /** standard constructor. Issues a LOOK against the specified ID, and sets up
    the necessary signals to drive the Room if id arg is provided */
    explicit Room(Lobby *l, const std::string& id);
	
    virtual RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation& op);
    void handleSoundTalk(Person* p, const std::string& speech);
    void handleEmote(Person* p, const std::string& desc);
        
        
    std::string m_roomId;
private:
    /// helper to see if all the people in the room are valid, and if so, do entry
    void checkEntry();

    void sight(const Atlas::Objects::Entity::RootEntity &room);

    void appearance(const std::string& personId);
    void disappearance(const std::string& personId);
    
    // callback slot when Lobby recives SIGHT(person)
    void notifyPersonSight(Person *p);
        
    std::string m_name;
    std::string m_topic;
    bool m_entered;     ///< set once we enter the room, i.e have info on all the members
    Lobby* m_lobby;
    
    IdPersonMap m_members;
    
    std::vector<Room*> m_subrooms;
};
	
}

#endif
