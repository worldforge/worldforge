#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Person.h"

namespace Eris
{
	
Person::Person(const Atlas::Objects::Entity::Account &acc) :
	_id(acc.GetId()),
	_name(acc.GetName())
{
	;
}
	
void Person::sight(const Atlas::Objects::Entity::Account &acc)
{
	if (acc.GetId() != _id)
		throw IllegalObject(acc, "Person's ID doesn't match account ID: " + _id);
	_name = acc.GetName();
	
	// FIXME - remove this once all clients set a name
	if (_name.empty())
		_name = _id;
}

} // of namespace Eris
