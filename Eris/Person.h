#ifndef ERIS_PERSON_H
#define ERIS_PERSON_H

#include <Atlas/Objects/Entity/Account.h>

#include "Types.h"

namespace Eris
{
	
/** An Out-of-Game Person (found in a Room / Lobby)	As more person data becomes available,
this class will be extended, for example to return nicknames, location, the choosen 'face' graphic.
*/
class Person {	
public:
	/// create a person from sight
	Person(const Atlas::Objects::Entity::Account &acc);

	/// update a person based on sight
	void sight(const Atlas::Objects::Entity::Account &acc);

	/// access the Atlas account ID for this person
	std::string getAccount() const { return _id; }
	/// access the human-readable name for this person 
	std::string getName() const {return _name;}
protected:
	const std::string _id;	///< the account ID 
	std::string _name;	///< the name, i.e account.GetName()
	// other fields ...
};

}

#endif
