#ifndef ERIS_PERSON_H
#define ERIS_PERSON_H

#include <Atlas/Objects/Entity/Account.h>

#include "Types.h"

/// An Out-of-Game Person (found in a Room / Lobby)
namespace Eris
{
	
class Person {	
public:
	/// create a person from sight
	Person(const Atlas::Objects::Entity::Account &acc);

	/// update a person based on sight
	void sight(const Atlas::Objects::Entity::Account &acc);

	std::string getAccount() const { return _id; }
	std::string getName() const {return _name;}
protected:
	const std::string _id;
	std::string _name;
	// other fields ...
};

}

#endif
