#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Person.h"
#include "Lobby.h"
#include "Utils.h"

namespace Eris
{
	
Person::Person(Lobby *l, const Atlas::Objects::Entity::Account &acc) :
	_id(acc.GetId()),
	_name(acc.GetName()),
	_lobby(l)
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

void Person::msg(const std::string &msg)
{
    if (!_lobby->getConnection()->isConnected())
	// FIXME - provide some feed-back here
	return;
	
    Atlas::Objects::Operation::Talk t = 
	Atlas::Objects::Operation::Talk::Instantiate();
	
    Atlas::Message::Object::MapType speech;
    speech["say"] = msg;
    //speech["loc"] =   no location for private chat
	
    t.SetArgs(Atlas::Message::Object::ListType(1, speech));
    t.SetTo(_id);
    t.SetFrom(_lobby->getAccountID());
    t.SetSerialno(getNewSerialno());
	
    _lobby->getConnection()->send(t);
}

} // of namespace Eris
