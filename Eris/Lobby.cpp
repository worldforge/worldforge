#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Lobby.h>
#include <Eris/Connection.h>
#include <Eris/SignalDispatcher.h>
#include <Eris/ClassDispatcher.h>
#include <Eris/OpDispatcher.h>
#include <Eris/TypeDispatcher.h>
#include <Eris/EncapDispatcher.h>
#include <Eris/ArgumentDispatcher.h>

#include <Eris/Utils.h>
#include <Eris/Log.h>
#include <Eris/Player.h>
#include <Eris/Room.h>
#include <Eris/Person.h>
#include <Eris/atlas_utils.h>

// various atlas headers we need
#include <Atlas/Bridge.h>
#include <Atlas/EncoderBase.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Entity/Player.h>
#include <Atlas/Objects/Entity/GameEntity.h>

#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/signal_system.h>
#else
#include <sigc++/signal.h>
#endif
#include <algorithm> 
#include <cassert>

using namespace Atlas::Objects;
using namespace Atlas;

namespace AtlasEntity = Atlas::Objects::Entity;

namespace Eris {

Lobby::Lobby(Connection *con) :
	Room(this),
	_account(""),
	_con(con),
	_reconnect(false),
	_infoRefno(-1)
{
	assert(_con);
	_con->Failure.connect(SigC::slot(*this, &Lobby::netFailure));
	_con->Connected.connect(SigC::slot(*this, &Lobby::netConnected));
	
	if (_con->getStatus() == BaseConnection::CONNECTED) {
	    // otherwise we never see the Connected signal for obvious reasons
	    netConnected();
	}
}
	
Lobby::~Lobby()
{
	_con->removeIfDispatcherByPath("op:oog:sight:entity", "account");
	_con->removeIfDispatcherByPath("op:oog:sight:entity", "room");
}

const std::string& Lobby::getAccountID()
{
	if (_account.empty())
		throw InvalidOperation("Not logged into account yet");
	return _account;
}

Room* Lobby::join(const std::string &roomID)
{
	if (_account.empty())
		throw InvalidOperation("Not logged into account yet");
	
	if (!_con->isConnected())
		throw InvalidOperation("Not connected to server");
		
	Operation::Move join = Operation::Move::Instantiate();
	join.setFrom(_account);
	join.setSerialno(getNewSerialno());
	
	Message::Element::MapType args;
	args["loc"] = roomID;
	args["mode"] = "join";
	join.setArgs(Message::Element::ListType(1, args));
	
	_con->send(join);
	
	RoomDict::iterator R = _roomDict.find(roomID);
	if (R == _roomDict.end()) {
		Room *nr = new Room(this, roomID);
		_roomDict[roomID] = nr;
		return nr;
	}
	
	return R->second;
}

Person* Lobby::getPerson(const std::string &acc)
{
	PersonDict::iterator i = _peopleDict.find(acc);
	if (i == _peopleDict.end()) {
		look(acc);
		// create a NULL entry (indicates we are doing the look)
		_peopleDict[acc] = NULL;
		return NULL;
	} else
		return i->second; // NOTE may still be NULL!
}

Room* Lobby::getRoom(const std::string &id)
{
	RoomDict::iterator i =  _roomDict.find(id);
	if (i == _roomDict.end())
		throw InvalidOperation("Unknown room " + id);
	
	return i->second;
}

/*
Room* Lobby::Create()
{
	NOT IMPLEMENTED YET	
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Protected methods

void Lobby::expectInfoRefno(long ref)
{
	_infoRefno = ref;
}

void Lobby::registerCallbacks()
{
	if (_account.empty())
		throw InvalidOperation("can't register lobby dispatchers yet (need account ID)");
	
	Eris::log(LOG_DEBUG, "in Lobby::registerCallbacks");
	
	Dispatcher *rop = _con->getDispatcherByPath("op");
	assert(rop);	
	Dispatcher *oogd = rop->addSubdispatch(new OpToDispatcher("oog", _account));
	oogd = oogd->addSubdispatch(ClassDispatcher::newAnonymous(_con));
	
	// add in the basics so rooms can hook below
	Dispatcher *sndd = oogd->addSubdispatch(new EncapDispatcher("sound"), "sound");
	Dispatcher *d = sndd->addSubdispatch(new OpToDispatcher("private", _account));
	d = d->addSubdispatch(ClassDispatcher::newAnonymous(_con));
	d->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Talk>("lobby",
		SigC::slot(*this, &Lobby::recvPrivateChat)),
		"talk"
	);
	
	oogd->addSubdispatch(new StdBranchDispatcher("appearance"), "appearance");
	oogd->addSubdispatch(new StdBranchDispatcher("disappearance"), "disappearance");
	
	Dispatcher *sight = oogd->addSubdispatch(new EncapDispatcher("sight"), "sight");
	d = sight->addSubdispatch(new TypeDispatcher("op", "op"));
	// watch for creates
	
	Dispatcher *opsight = d->addSubdispatch(ClassDispatcher::newAnonymous(_con));
	Dispatcher *cr = opsight->addSubdispatch(new EncapDispatcher("create"), "create");
	cr->addSubdispatch(new SignalDispatcher2<Atlas::Objects::Operation::Create,
		Atlas::Objects::Entity::RootEntity>("lobby",
	    SigC::slot(*this, &Lobby::recvSightCreate)));
	
	d = sight->addSubdispatch(new TypeDispatcher("entity", "object"));
	Dispatcher *esight = d->addSubdispatch(ClassDispatcher::newAnonymous(_con));
	// the room entity callback
	esight->addSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::RootEntity>("lobby", 
		SigC::slot(*this, &Lobby::recvSightRoom)),
		"room"
	);
	
	// the account / player object callback
	esight->addSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::Account>("lobby", 
		SigC::slot(*this, &Lobby::recvSightPerson)),
		"account"
	);
}

void Lobby::netFailure(const std::string& /*msg*/)
{
	// what needs to be done here?
}

void Lobby::netDisconnected()
{
	// ditto
}

void Lobby::netConnected()
{
	// if the account is valid, we already connected at least one
	_reconnect = !_account.empty();

	Dispatcher *ied = _con->getDispatcherByPath("op:info:entity");
	assert(ied);
	Dispatcher *accd = ied->addSubdispatch(ClassDispatcher::newAnonymous(_con));
	
	accd->addSubdispatch( new SignalDispatcher2<Operation::Info, AtlasEntity::Account>(
		"lobby", SigC::slot(*this, &Lobby::recvInfoAccount)),
		"account"
	);
}

void Lobby::look(const std::string &id)
{
	if (!_con->isConnected())
		// FIXME - handle this more elegantly?
		return;
	
	Operation::Look look = 
  		Operation::Look::Instantiate();

	if(!id.empty()) {
		Atlas::Message::Element::MapType what;
		what["id"]=id;
		Atlas::Message::Element::ListType args(1,what);
		look.setArgs(args);
		look.setTo(id);
	} else {
		
	}

	look.setFrom(_account);
	look.setSerialno(getNewSerialno());
	
	_con->send(look);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dispatcher callbacks

void Lobby::recvInfoAccount(const Atlas::Objects::Operation::Info &ifo, 
	const Atlas::Objects::Entity::Account &account)
{
	Eris::log(LOG_VERBOSE, "in recvInfoAccount");
	
	// reject lots of extraneous INFOs we don't care about
	// FIXME  - enable the refno test once stage correctly processes it
	if (!_account.empty() && (ifo.getTo() != _account))
	    return;
	
	_infoRefno = -1; // clear the expect value to reject further INFOs (unless we set them!);
	
	_account = account.getId();
	if (!_reconnect) // obviously only register first time around
	    registerCallbacks();
	
	// broadcast the login
	LoggedIn.emit(Atlas::atlas_cast<AtlasEntity::Player>(account));
	_con->removeDispatcherByPath("op:info:entity", "lobby");
	
	if (_reconnect) {
		// don't issue an annonymous look; there is basically no need,
		// and it would confuse the logic in RecvSightRoom (calls Setup twice, etc)
		look(_id);
	} else {
		look("");
		
	}
}

void Lobby::recvSightPerson(const Atlas::Objects::Entity::Account &ac)
{
	PersonDict::iterator i = _peopleDict.find(ac.getId());
	if (i == _peopleDict.end()) {
		Eris::log(LOG_WARNING, "got un-requested sight of person %s", ac.getId().c_str());
		return;
	}
	
	if (i->second)
		i->second->sight(ac);
	else
		// install the new Person object
		i->second = new Person(this, ac);
	
	// emit the signal; this lets rooms waiting on this player's info update
	SightPerson.emit(i->second);
}

void Lobby::recvSightRoom(const Atlas::Objects::Entity::RootEntity &room)
{
	// check if this is initial room (lobby), from the anonymous LOOK
	if (_id.empty()) {
		log(LOG_NOTICE, "recieved sight of root room (lobby)");
		_roomDict[room.getId()] = this;
		
		_id = room.getId();
		// now id is valid, can safely call Setup on the lobby
		Room::setup();
		
		Room::sight(room);
	} else {
		RoomDict::iterator i = _roomDict.find(room.getId());
		if (i == _roomDict.end())
			throw InvalidOperation("Got sight of unknown room!");
		
		log(LOG_NOTICE, "recived sight of room %s", i->first.c_str());
		i->second->sight(room);
	}
	
	
}

// FIXME - this method is obsolete once Cyphesis supports the new OOG semantics
void Lobby::recvSightLobby(const Atlas::Objects::Entity::RootEntity &lobby)
{
	assert(_id.empty());
	_id = lobby.getId();
	Room::setup();
}

void Lobby::recvPrivateChat(const Atlas::Objects::Operation::Talk &tk)
{
	const Atlas::Message::Element &obj = getArg(tk, 0);
	Message::Element::MapType::const_iterator m = obj.asMap().find("say");
	if (m == obj.asMap().end())
		throw IllegalObject(tk, "No sound object in arg 0");
	std::string say = m->second.asString();
	
	// get the player name and emit the signal already
	Person *p = getPerson(tk.getFrom());
	assert(p);
	PrivateTalk.emit(p->getAccount(), say);
}

void Lobby::recvSightCreate(const Atlas::Objects::Operation::Create &cr,
	const Atlas::Objects::Entity::RootEntity &ent)
{	
    std::string type = ent.getParents()[0].asString();
    if (type == "room")
	processRoomCreate(cr, ent);
}

void Lobby::processRoomCreate(const Atlas::Objects::Operation::Create &cr,
	const Atlas::Objects::Entity::RootEntity &ent)
{
    log(LOG_DEBUG, "recieved sight of room creation");
	
    PendingCreateMap::iterator P = _pendingCreate.find(cr.getSerialno());
    if (P != _pendingCreate.end()) {
	// it was requested locally, so we already have the Room object
	P->second->_id = ent.getId(); // set the ID
	P->second->setup();		// now we can call setup safely
	P->second->sight(ent);	// finally slam the data in
	_pendingCreate.erase(P);	// get rid of the request
    }
    
    // find the containing room and update it's subrooms
    // note that we may not even know about it's containing room either!
    std::string containingRoom = ent.getAttr("loc").asString();
    if (_roomDict.find(containingRoom) == _roomDict.end())
	return; // we can't see it,, so we don't care [we'll get the rooms anyway if we ever join the containing room]
    
    Room *container = _roomDict[containingRoom];
    container->_subrooms.insert(ent.getId());	// jam it in
    
    StringSet strset;
    strset.insert("rooms");
    container->Changed.emit(strset);
}

void Lobby::addPendingCreate(Room *r, int serialno)
{
    PendingCreateMap::iterator P = _pendingCreate.find(serialno);
    if (P != _pendingCreate.end())
	throw InvalidOperation("duplicate serialno in addPendingCreate");
    
    _pendingCreate.insert(P, PendingCreateMap::value_type(serialno, r));
}

} // of namespace
