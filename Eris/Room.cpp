#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Room.h>
#include <Eris/Lobby.h>
#include <Eris/Connection.h>
#include <Eris/Utils.h>
#include <Eris/Person.h>
#include <Eris/Log.h>

#include <Eris/SignalDispatcher.h>
#include <Eris/ClassDispatcher.h>
#include <Eris/ArgumentDispatcher.h>

#include <Eris/Exceptions.h>

#include <Atlas/Objects/Entity/RootEntity.h>

#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Imaginary.h>
#include <Atlas/Objects/Operation/Talk.h>

#include <sigc++/object_slot.h>

#include <cassert>

using namespace Atlas;

typedef Atlas::Message::Element::ListType AtlasListType;
typedef Atlas::Message::Element::MapType AtlasMapType;

namespace Eris
{
	
Room::Room(Lobby *l, const std::string &id) :
    _id(id), 
    _lobby(l),
    _parted(false)
{
    if (_id.empty()) return;
    assert(l);
    setup();
}

Room::~Room()
{
	/* avoid exceptions from further down if the Lobby never recieved the OOG entry. This
	happens if a Lobby object is created but the connection never goes live, for example. */
    if (_id.empty()) return;
    
	if (!_parted) // brutal tear down
		leave();
	
	Connection *con = _lobby->getConnection();
	std::string rid = "room_" + _id;
	
	// delete *everything* below our node
	con->removeDispatcherByPath("op:oog:sound", rid);
	con->removeDispatcherByPath("op:oog:sight:op", rid);
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
		SigC::slot(*this, &Room::recvSoundTalk)
	));

// visual stuff (sights)
    
    Dispatcher *sightOp = con->getDispatcherByPath("op:oog:sight:op");

    Dispatcher *cd = sightOp->addSubdispatch(ClassDispatcher::newAnonymous(con));
    Dispatcher *location = cd->addSubdispatch(new ArgumentDispatcher(rid, "loc", _id), "imaginary");
    
    location->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Imaginary>("imag",
		SigC::slot(*this, &Room::recvSightImaginary)
    ));

// appearance
	Dispatcher *apd = con->getDispatcherByPath("op:oog:appearance");
	apd = apd->addSubdispatch(new ArgumentDispatcher(rid, "loc", _id));
	apd->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Appearance>("foo",
		SigC::slot(*this, &Room::recvAppear)
	));
	
// disappearance
	Dispatcher *disd = con->getDispatcherByPath("op:oog:disappearance");
	disd = disd->addSubdispatch(new ArgumentDispatcher(rid, "loc", _id));
	disd->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Disappearance>("foo",
		SigC::slot(*this, &Room::recvDisappear)
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
	
	Atlas::Objects::Operation::Talk t;
	
	Atlas::Message::Element::MapType speech;
	speech["say"] = tk;
	speech["loc"] = _id;
	
	t.setArgs(Atlas::Message::Element::ListType(1, speech));
	t.setTo(_id);
	t.setFrom(_lobby->getAccountID());
	t.setSerialno(getNewSerialno());
	
	_lobby->getConnection()->send(t);
}

void Room::emote(const std::string &em)
{
	if (!_lobby->getConnection()->isConnected())
		// FIXME - provide some feed-back here
		return;
	
	Atlas::Objects::Operation::Imaginary im;
	
	Atlas::Message::Element::MapType emote;
	emote["id"] = "emote";
	emote["description"] = em;
	emote["loc"] = _id;
	
	im.setArgs(Atlas::Message::Element::ListType(1, emote));
	im.setTo(_id);
	im.setFrom(_lobby->getAccountID());
	im.setSerialno(getNewSerialno());
	
	_lobby->getConnection()->send(im);
}

void Room::leave()
{
	Connection *c = _lobby->getConnection();
	if (!c->isConnected())
		throw InvalidOperation("Not connected to server");
		
	Atlas::Objects::Operation::Move part;
	part.setFrom(_lobby->getAccountID());
	part.setSerialno(getNewSerialno());
	
	Message::Element::MapType args;
	args["loc"] = _id;
	args["mode"] = "part";
	part.setArgs(Message::Element::ListType(1, args));
	
	c->send(part);
	// FIXME - confirm the part somehow?
	_parted = true;
}

Room* Room::createRoom(const std::string &name)
{
    Connection *c = _lobby->getConnection();
    if (!c->isConnected())
		throw InvalidOperation("Not connected to server");
    
    Atlas::Objects::Operation::Create cr;
    cr.setFrom(_lobby->getAccountID());
    cr.setTo(_id);
    int serial = getNewSerialno();
    cr.setSerialno(serial);
    
    Message::Element::MapType args;
    args["parents"] = Message::Element::ListType(1, "room");
    args["name"] = name;
    
    cr.setArgs(Message::Element::ListType(1, args));
    c->send(cr);
    
    Room *r = new Room(_lobby);
    // this lets the lobby do stitch up as necessary
    _lobby->addPendingCreate(r, serial);
    r->_name = name;	// I'm setting this since we have it already, and it might
    // help someone identify the room prior to the ID going valid.
    return r;
}

void Room::sight(const Atlas::Objects::Entity::RootEntity &room)
{
	log(LOG_NOTICE, "Got sight of room %s", _id.c_str());
	_initialGet = true;
		
	_name = room.getName();
	//_creator = room.getAttr("creator").asString();

	// extract the current people list
	if (room.hasAttr("people")) {
		Message::Element::ListType people = room.getAttr("people").asList();
			
		for (Message::Element::ListType::const_iterator i=people.begin(); i!=people.end(); ++i) {
	
			std::string account = i->asString();
			_people.insert(account);
		
			if (_lobby->getPerson(account) == NULL) {
				// mark as pending
				_pending.insert(account);
			}
		}
	}
	
	if (_pending.empty()) {
		log(LOG_NOTICE, "Doing immediate entry to room %s", _id.c_str());
		// FIXME  - this code will cause OOG entry, even if the server
		// doesn't really support it (just to an empty lobby). The option
		// is not to emit the 'entered' signal at all. I don't know which
		// makes more sense.
		Entered.emit(this);
		_initialGet = false;
	}

	// wire up the signal for initial get
	_lobby->SightPerson.connect(SigC::slot(*this, &Room::notifyPersonSight));
	
	if (room.hasAttr("rooms")) {
	    Message::Element::ListType rooms = room.getAttr("rooms").asList();
	    // rattle through the list, jamming them into our set
	    for (unsigned int R=0; R<rooms.size();R++)
		_subrooms.insert(rooms[R].asString());
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
	const Atlas::Message::Element &obj = getArg(tk, 0);
	Message::Element::MapType::const_iterator m = obj.asMap().find("say");
	if (m == obj.asMap().end())
		throw IllegalObject(tk, "No sound object in arg 0");
	std::string say = m->second.asString();
	
	// quick sanity check
	if (_pending.find(tk.getFrom()) != _pending.end()) {
		// supress this talk until we have the name
		// FIXME - buffer these and spool back?
		return;
	}
	// hit this assert if get a talk from somone we know *nothing* about
	if (_people.find(tk.getFrom()) == _people.end()) {
	    log(LOG_DEBUG, "unknown FROM %s in TALK operation");
	    assert(false);
	}
	
	// get the player name and emit the signal already
	Person *p = _lobby->getPerson(tk.getFrom());
	assert(p);
	Talk.emit(this, p->getAccount(), say);
}

void Room::recvSightImaginary(const Atlas::Objects::Operation::Imaginary &im)
{
    const Atlas::Message::Element &obj = getArg(im, 0);
    Message::Element::MapType::const_iterator m = obj.asMap().find("description");
    if (m == obj.asMap().end())
	return;
    const std::string & description = m->second.asString();
    // quick sanity check
    if (_pending.find(im.getFrom()) != _pending.end()) {
    	// supress this talk until we have the name
    	// FIXME - buffer these and spool back?
    	return;
    }
    // hit this assert if get a talk from somone we know *nothing* about
    if (_people.find(im.getFrom()) == _people.end()) {
        log(LOG_DEBUG, "unknown FROM %s in TALK operation");
        assert(false);
    }
    Person *p = _lobby->getPerson(im.getFrom());
    Emote.emit(this, p->getAccount(), description);
}

void Room::recvAppear(const Atlas::Objects::Operation::Appearance &ap)
{
	const AtlasListType &args = ap.getArgs();
	for (AtlasListType::const_iterator A=args.begin();A!=args.end();++A) {
		const AtlasMapType &app = A->asMap();
		AtlasMapType::const_iterator V(app.find("id"));
		std::string account(V->second.asString());
		
		_people.insert(account);
		if (_lobby->getPerson(account)) {
			// player is already known, can emit right now
			Appearance.emit(this, account);
		} else {
			// need to wait on the lookup
			if (_pending.empty())
				_lobby->SightPerson.connect(SigC::slot(*this, &Room::notifyPersonSight));
			
			_pending.insert(account);
		}
	}
}

void Room::recvDisappear(const Atlas::Objects::Operation::Disappearance &dis)
{
	
	const AtlasListType &args = dis.getArgs();
	for (AtlasListType::const_iterator A=args.begin();A!=args.end();++A) {
		const AtlasMapType &app = A->asMap();
		AtlasMapType::const_iterator V(app.find("id"));
		std::string account(V->second.asString());
		
		if (_people.find(account) == _people.end())
			throw IllegalObject(dis, "room disappearance for unknown person");
	
		_people.erase(account);
		Disappearance.emit(this, account);
	}
}

const std::string& Room::getID() const
{
    if (_id.empty() || _id == "") {
	throw InvalidOperation("called Room::getID() before the ID was available \
	    (wait till Entered signal is emitted");
    }
    
    return _id;
}

Person* Room::getPersonByUID(const std::string& uid)
{
    return _lobby->getPerson(uid);
}

} // of Eris namespace
