#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Person.h>
#include <Eris/Lobby.h>
#include <Eris/Utils.h>

#include <Atlas/Objects/Entity/Account.h>
#include <Atlas/Objects/Operation/Talk.h>

namespace Eris
{
	
Person::Person(Lobby *l, const Atlas::Objects::Entity::Account &acc) :
	_id(acc.getId()),
	_name(acc.getName()),
	_lobby(l)
{
	;
}
	
void Person::sight(const Atlas::Objects::Entity::Account &acc)
{
	if (acc.getId() != _id)
		throw IllegalObject(acc, "Person's ID doesn't match account ID: " + _id);
	_name = acc.getName();
	
	// FIXME - remove this once all clients set a name
	if (_name.empty())
		_name = _id;
}

void Person::msg(const std::string &msg)
{
    if (!_lobby->getConnection()->isConnected())
	// FIXME - provide some feed-back here
	return;
	
    Atlas::Objects::Operation::Talk t;
    Atlas::Message::Element::MapType speech;
    speech["say"] = msg;
    //speech["loc"] =   no location for private chat
	
    t.setArgs(Atlas::Message::Element::ListType(1, speech));
    t.setTo(_id);
    t.setFrom(_lobby->getAccountID());
    t.setSerialno(getNewSerialno());
	
    _lobby->getConnection()->send(t);
}

} // of namespace Eris
