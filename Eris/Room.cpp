#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/signal_system.h>

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>

#include "Room.h"
#include "Lobby.h"
#include "Connection.h"
#include "Utils.h"
#include "Person.h"

#include "OpDispatcher.h"
#include "SignalDispatcher.h"
#include "ClassDispatcher.h"
#include "EncapDispatcher.h"

using namespace Atlas;

namespace Eris
{
	
Room::Room(Lobby *l, const std::string &id) :
	_id(id), 
	_lobby(l),
	_parted(false)
{
	assert(l);
	setup();
}

Room::Room(Lobby *l) :
	_id(""), _lobby(l), _parted(false)
{
	;
}

Room::~Room()
{
	//cerr << "deleting room " << _name << endl;
	
	if (!_parted)
		leave();
	
	Connection *con = _lobby->getConnection();
	std::string rid = "room_" + _id;
	
	con->removeDispatcherByPath("op:oog:sound", rid);
	con->removeDispatcherByPath("op:oog:appear", rid);
	con->removeDispatcherByPath("op:oog:disappear", rid);
}

void Room::setup()
{
	assert(!_id.empty());
	
	// setup the dispatchers
	Connection *con = _lobby->getConnection();
	
	Dispatcher *d = con->getDispatcherByPath("op:oog");
	assert(d);
	Dispatcher *room = d->addSubdispatch(new OpFromDispatcher("room_" + _id, _id));
	
	// talk 
	Dispatcher *sndd = room->addSubdispatch(new EncapDispatcher("sound", "sound"));
	sndd->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Talk>("foo",
		SigC::slot(this, &Room::recvSoundTalk)
	));
	
	// appearance
	Dispatcher *apd  = room->addSubdispatch(new ClassDispatcher("appear", "appearance"));
	apd->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Appearance>("foo",
		SigC::slot(this, &Room::recvAppear)
	));
	
	// disappearance
	Dispatcher *disd = room->addSubdispatch(new ClassDispatcher("disappear", "disappearance"));
	disd->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Disappearance>("foo",
		SigC::slot(this, &Room::recvDisappear)
	));
	
// initial look
	/*
	Atlas::Objects::Operation::Look lk = 
		Atlas::Objects::Operation::Look::Instantiate();
	
	lk.SetFrom(_lobby->GetAccountID());
	lk.SetTo(_id);
	*/
	
	if (_lobby != this) // not actually necessary, but avoids a duplicate initial look
		_lobby->look(_id);
	//con->Send(lk);
}

void Room::say(const std::string &tk)
{
	if (!_lobby->getConnection()->isConnected())
		// FIXME - provide some feed-back here
		return;
	
	Atlas::Objects::Operation::Talk t = 
		Atlas::Objects::Operation::Talk::Instantiate();
	
	//Object::ListType args;
	Atlas::Message::Object::MapType speech;
	speech["say"] = tk;
	
	t.SetArgs(Atlas::Message::Object::ListType(1, speech));
	t.SetTo(_id);
	t.SetFrom(_lobby->getAccountID());
	t.SetSerialno(getNewSerialno());
	
	_lobby->getConnection()->send(t);
}

void Room::leave()
{
	Connection *c = _lobby->getConnection();
	if (!c->isConnected())
		throw InvalidOperation("Not connected to server");
	
	//cerr << "requesting join of room " << roomID << endl;
	
	Atlas::Objects::Operation::Move part = 
		Atlas::Objects::Operation::Move::Instantiate();
	part.SetFrom(_lobby->getAccountID());
	part.SetSerialno(getNewSerialno());
	
	Message::Object::MapType args;
	args["loc"] = _id;
	args["mode"] = "part";
	part.SetArgs(Message::Object::ListType(1, args));
	
	c->send(part);
	// FIXME - confirm the part somehow?
	_parted = true;
}

void Room::sight(const Atlas::Objects::Entity::RootEntity &room)
{
	if (!checkInherits(room, "room"))
		throw IllegalObject(room, "room object doesn't claim to be a room");
	
	_initialGet = true;
		
	_name = room.GetName();
	//_creator = room.GetAttr("creator").AsString();

	// extract the current people list
	if (room.HasAttr("people")) {
		Message::Object::ListType people = room.GetAttr("people").AsList();
			
		for (Message::Object::ListType::const_iterator i=people.begin(); i!=people.end(); ++i) {
	
			string account = i->AsString();
			_people.insert(account);
		
			if (_lobby->getPerson(account) == NULL) {
				// mark as pending
				_pending.insert(account);
			}
		}
	} else {
		// FIXME  - this code will cause OOG entry, even if the server
		// doesn't really support it (just to an empty lobby). The option
		// is not to emit the 'entered' signal at all. I don't know which
		// makes more sense.
		Entered.emit(this);
		_initialGet = false;
	}

	// wire up the signal for initial get
	_lobby->SightPerson.connect(SigC::slot(this, &Room::notifyPersonSight));
	
	if (room.HasAttr("rooms")) {
		Message::Object::ListType rooms = room.GetAttr("rooms").AsList();
		// FIXME - handle sub-rooms
		//cerr << "got the sub-room list " << endl;
	}
}

void Room::notifyPersonSight(Person *p)
{
	assert(p);
	_pending.erase(p->getAccount());
	
	if (_pending.empty()) {
		if (_initialGet) {
			Entered.emit(this);
			_initialGet = false;
		} else {
			// we were waiting to show an appearance, we assume
			Appearance.emit(this, p->getAccount());
		}
	}
}

void Room::recvSoundTalk(const Atlas::Objects::Operation::Talk &tk)
{
	const Atlas::Message::Object &obj = getArg(tk, 0);
	Message::Object::MapType::const_iterator m = obj.AsMap().find("say");
	if (m == obj.AsMap().end())
		throw IllegalObject(tk, "No sound object in arg 0");
	std::string say = m->second.AsString();
	
	// quick sanity check
	assert (_people.find(tk.GetFrom()) != _people.end());
	if (_pending.find(tk.GetFrom()) != _pending.end()) {
		// supress this talk until we have the name
		// FIXME - buffer these and spool back?
		return;
	}
	
	// get the player name and emit the signal already
	Person *p = _lobby->getPerson(tk.GetFrom());
	assert(p);
	Talk.emit(this, p->getName(), say);
}

void Room::recvAppear(const Atlas::Objects::Operation::Appearance &ap)
{
	std::string account = getArg(ap, "id").AsString();
	_people.insert(account);
	
	if (_lobby->getPerson(account)) {
		// player is already known, can emit right now
		Appearance.emit(this, account);
	} else {
		// need to wait on the lookup
		if (_pending.empty())
			_lobby->SightPerson.connect(SigC::slot(this, &Room::notifyPersonSight));
		
		_pending.insert(account);
	}
}

void Room::recvDisappear(const Atlas::Objects::Operation::Disappearance &dis)
{
	std::string account = getArg(dis, "id").AsString();
 	if (_people.find(account) == _people.end())
		throw IllegalObject(dis, "room disappearance for unknown person");
	
	_people.erase(account);
	Disappearance.emit(this, account);
}

};
