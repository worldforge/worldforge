#ifndef ERIS_PERSON_H
#define ERIS_PERSON_H

#include <Atlas/Objects/Entity/Account.h>

#include "Types.h"

namespace Eris
{

class Lobby;    
    
/** An Out-of-Game Person (found in a Room / Lobby)	As more person data becomes available,
this class will be extended, for example to return nicknames, location, the choosen 'face' graphic.
*/
class Person {	
public:
    /// create a person from sight
    Person(Lobby *l, const Atlas::Objects::Entity::Account &acc);

    /// update a person based on sight
    void sight(const Atlas::Objects::Entity::Account &acc);

    /** send a private message to a person */
    void msg(const std::string &msg);

    /// access the Atlas account ID for this person
    const std::string& getAccount() const { return _id; }
    /// access the human-readable name for this person 
    const std::string& getName() const {return _name;}
protected:
    const std::string _id;	///< the account ID 
    std::string _name;	///< the name, i.e account.GetName()
	// other fields ...

    Lobby* _lobby;	///< the lobby owning this Person
};

}

#endif
