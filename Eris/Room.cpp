#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/signal_system.h>

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Imaginary.h>

#include "Room.h"
#include "Lobby.h"
#include "Connection.h"
#include "Utils.h"
#include "Person.h"

#include "OpDispatcher.h"
#include "SignalDispatcher.h"
#include "ClassDispatcher.h"
#include "EncapDispatcher.h"
#include "IdDispatcher.h"
#include "ArgumentDispatcher.h"

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
	if (!_parted)
		leave();
	
	Connection *con = _lobby->getConnection();
	std::string rid = "room_" + _id;
	
	// delete *everything* below our node
	con->removeDispatcherByPath("op:oog:sound", rid);
	//con->removeDispatcherByPath("op:oog:sight", rid);
	con->removeDispatcherByPath("op:oog:imaginary", rid);
	con->removeDispatcherByPath("op:oog:appearance", rid);
	con->removeDispatcherByPath("op:oog:disappearance", rid);
}

void Room::setup()
{
	assert(!_id.empty());
	std::string rid("room_" + _id);
	// setup the dispatchers
	Connection *con = _lobby->getConnection();
	
	Dispatcher *sound = con->getDispatcherByPath("op:oog:sound");
	sound = sound->addSubdispatch(new ArgumentDispatcher("room_" + _id, "loc", _id));
	
// talk 
	sound->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Talk>("foo",
		SigC::slot(this, &Room::recvSoundTalk)
	));

// visual stuff (sights)
	Dispatcher *img = con->getDispatcherByPath("op:oog:sight:op");
	assert(img);
	img = img->addSubdispatch(new ArgumentDispatcher(rid, "loc", _id));
	img = img->addSubdispatch(ClassDispatcher::newAnonymous());
	
	// emotes
	Dispatcher *em = img->addSubdispatch(new IdDispatcher("emote", "emote"), "imaginary");
	em->addSubdispatch(new SignalDispatcher2<Atlas::Objects::Operation::Imaginary,
		Atlas::Objects::Root>("emote",
		SigC::slot(this, &Room::recvSightEmote)
	));

// appearance
	Dispatcher *apd = con->getDispatcherByPath("op:oog:appearance");
	apd = apd->addSubdispatch(new ArgumentDispatcher(rid, "loc", _id));
	apd->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Appearance>("foo",
		SigC::slot(this, &Room::recvAppear)
	));
	
// disappearance
	Dispatcher *disd = con->getDispatcherByPath("op:oog:disappearance");
	disd = disd->addSubdispatch(new ArgumentDispatcher(rid, "loc", _id));
	disd->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Disappearance>("foo",
		SigC::slot(this, &Room::recvDisappear)
	));
	
// initial look	
	if (_lobby != this) // not actually necessary, but avoids a duplicate initial look
		_lobby->look(_id);
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
	speech["loc"] = _id;
	
	t.SetArgs(Atlas::Message::Object::ListType(1, speech));
	t.SetTo(_id);
	t.SetFrom(_lobby->getAccountID());
	t.SetSerialno(getNewSerialno());
	
	_lobby->getConnection()->send(t);
}

void Room::emote(const std::string &em)
{
	if (!_lobby->getConnection()->isConnected())
		// FIXME - provide some feed-back here
		return;
	
	Atlas::Objects::Operation::Imaginary im = 
		Atlas::Objects::Operation::Imaginary::Instantiate();
	
	Atlas::Message::Object::MapType emote;
	emote["id"] = "emote";
	emote["description"] = em;
	emote["loc"] = _id;
	
	im.SetArgs(Atlas::Message::Object::ListType(1, emote));
	im.SetTo(_id);
	im.SetFrom(_lobby->getAccountID());
	im.SetSerialno(getNewSerialno());
	
	_lobby->getConnection()->send(im);
}

void Room::leave()
{
	Connection *c = _lobby->getConnection();
	if (!c->isConnected())
		throw InvalidOperation("Not connected to server");
		
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
	Log(LOG_NOTICE, "Got sight of room %s", _id.c_str());
	_initialGet = true;
		
	_name = room.GetName();
	//_creator = room.GetAttr("creator").AsString();

	// extract the current people list
	if (room.HasAttr("people")) {
		Message::Object::ListType people = room.GetAttr("people").AsList();
			
		for (Message::Object::ListType::const_iterator i=people.begin(); i!=people.end(); ++i) {
	
			std::string account = i->AsString();
			_people.insert(account);
		
			if (_lobby->getPerson(account) == NULL) {
				// mark as pending
				_pending.insert(account);
			}
		}
	}
	
	if (_pending.empty()) {
		Log(LOG_NOTICE, "Doing immediate entry to room %s", _id.c_str());
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
	Talk.emit(this, p->getAccount(), say);
}

void Room::recvSightEmote(const Atlas::Objects::Operation::Imaginary &imag,
		const Atlas::Objects::Root &emote)
{
	std::string msg = emote.GetAttr("description").AsString();
	
	Person *p = _lobby->getPerson(imag.GetFrom());
	assert(p);
	Emote.emit(this, p->getAccount(), msg);
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
