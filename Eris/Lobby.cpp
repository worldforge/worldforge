#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/signal_system.h>
#include <algorithm> 
#include <assert.h>

// various atlas headers we need
#include <Atlas/Bridge.h>
#include <Atlas/EncoderBase.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Entity/Player.h>
#include <Atlas/Objects/Entity/GameEntity.h>

#include "Lobby.h"
#include "Connection.h"
#include "SignalDispatcher.h"
#include "ClassDispatcher.h"
#include "OpDispatcher.h"
#include "TypeDispatcher.h"
#include "EncapDispatcher.h"
#include "ArgumentDispatcher.h"

#include "Utils.h"
#include "Player.h"
#include "Room.h"
#include "Person.h"
#include "atlas_utils.h"

using namespace Atlas::Objects;
using namespace Atlas;

namespace AtlasEntity = Atlas::Objects::Entity;

namespace Eris {

// declare the static member
Lobby* Lobby::_theLobby = NULL;	
	
Lobby::Lobby(/*Player *p,*/ Connection *con) :
	Room(this),
	_account(""),
	_con(con),
	//_player(p),
	_reconnect(false),
	_infoRefno(-1)
{
	assert(_con);
	_con->Failure.connect(SigC::slot(this, &Lobby::netFailure));
	_con->Connected.connect(SigC::slot(this, &Lobby::netConnected));
	
	if (_con->getStatus() == BaseConnection::CONNECTED) {
		// otherwise we never see the Connected signal for obvious reasons
		netConnected();
	}
}
	
Lobby::~Lobby()
{
	_con->removeDispatcherByPath("op:oog:sight:entity", "account");
	_con->removeDispatcherByPath("op:oog:sight:entity", "room");
}

Lobby* Lobby::instance()
{
	if (_theLobby) return _theLobby;
		
	if (!Connection::Instance())
		throw InvalidOperation("called Lobby::instance() before the Connection exists");
	
	_theLobby = new Lobby(Connection::Instance());
	return _theLobby;
}

std::string Lobby::getAccountID()
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
	join.SetFrom(_account);
	join.SetSerialno(getNewSerialno());
	
	Message::Object::MapType args;
	args["loc"] = roomID;
	args["mode"] = "join";
	join.SetArgs(Message::Object::ListType(1, args));
	
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
	
	Eris::Log(LOG_DEBUG, "in Lobby::registerCallbacks");
	
	Dispatcher *rop = _con->getDispatcherByPath("op");
	assert(rop);	
	Dispatcher *oogd = rop->addSubdispatch(new OpToDispatcher("oog", _account));
	oogd = oogd->addSubdispatch(ClassDispatcher::newAnonymous());
	
	// add in the basics so rooms can hook below
	Dispatcher *sndd = oogd->addSubdispatch(new EncapDispatcher("sound"), "sound");
	Dispatcher *d = sndd->addSubdispatch(new OpToDispatcher("private", _account));
	d = d->addSubdispatch(ClassDispatcher::newAnonymous());
	d->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Talk>("lobby",
		SigC::slot(this, &Lobby::recvPrivateChat)),
		"talk"
	);
	
	oogd->addSubdispatch(new StdBranchDispatcher("appearance"), "appearance");
	oogd->addSubdispatch(new StdBranchDispatcher("disappearance"), "disappearance");
	
	Dispatcher *sight = oogd->addSubdispatch(new EncapDispatcher("sight"), "sight");
	d = sight->addSubdispatch(new TypeDispatcher("op", "op"));
	
	d = sight->addSubdispatch(new TypeDispatcher("entity", "object"));
	Dispatcher *esight = d->addSubdispatch(ClassDispatcher::newAnonymous());
	// the room entity callback
	esight->addSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::RootEntity>("lobby", 
		SigC::slot(this, &Lobby::recvSightRoom)),
		"room"
	);
	
	// the account / player object callback
	esight->addSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::Account>("lobby", 
		SigC::slot(this, &Lobby::recvSightPerson)),
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
	Dispatcher *accd = ied->addSubdispatch(ClassDispatcher::newAnonymous());
	
	accd->addSubdispatch( new SignalDispatcher2<Operation::Info, AtlasEntity::Account>(
		"lobby", SigC::slot(this, &Lobby::recvInfoAccount)),
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
		Atlas::Message::Object::MapType what;
		what["id"]=id;
		Atlas::Message::Object::ListType args(1,what);
		look.SetArgs(args);
		look.SetTo(id);
	} else {
		
	}

	look.SetFrom(_account);
	look.SetSerialno(getNewSerialno());
	
	_con->send(look);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dispatcher callbacks

void Lobby::recvInfoAccount(const Operation::Info &ifo, 
	const AtlasEntity::Account &account)
{
	Eris::Log(LOG_VERBOSE, "in recvInfoAccount");
	
	// reject lots of extraneous INFOs we don't care about
	// FIXME  - enable the refno test once stage correctly processes it
	if (!_account.empty() && (ifo.GetTo() != _account) /*|| (ifo.getRefno() != _infoRefnno)*/) return;
	
	_infoRefno = -1; // clear the expect value to reject further INFOs (unless we set them!);
	
	_account = account.GetId();
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
	PersonDict::iterator i = _peopleDict.find(ac.GetId());
	if (i == _peopleDict.end()) {
		Eris::Log(LOG_WARNING, "got un-requested sight of person %s", ac.GetId().c_str());
		return;
	}
	
	if (i->second)
		i->second->sight(ac);
	else
		// install the new Person object
		i->second = new Person(ac);
	
	// emit the signal; this lets rooms waiting on this player's info update
	SightPerson.emit(i->second);
}

void Lobby::recvSightRoom(const Atlas::Objects::Entity::RootEntity &room)
{
	// check if this is initial room (lobby), from the anonymous LOOK
	if (_id.empty()) {
		Log(LOG_NOTICE, "recieved sight of root room (lobby)");
		_roomDict[room.GetId()] = this;
		
		_id = room.GetId();
		// now id is valid, can safely call Setup on the lobby
		Room::setup();
		
		Room::sight(room);
	} else {
		RoomDict::iterator i = _roomDict.find(room.GetId());
		if (i == _roomDict.end())
			throw InvalidOperation("Got sight of unknown room!");
		
		Log(LOG_NOTICE, "recived sight of room %s", i->first.c_str());
		i->second->sight(room);
	}
	
	
}

// FIXME - this method is obsolete once Cyphesis supports the new OOG semantics
void Lobby::recvSightLobby(const Atlas::Objects::Entity::RootEntity &lobby)
{
	assert(_id.empty());
	_id = lobby.GetId();
	Room::setup();
}

void Lobby::recvPrivateChat(const Atlas::Objects::Operation::Talk &tk)
{
	const Atlas::Message::Object &obj = getArg(tk, 0);
	Message::Object::MapType::const_iterator m = obj.AsMap().find("say");
	if (m == obj.AsMap().end())
		throw IllegalObject(tk, "No sound object in arg 0");
	std::string say = m->second.AsString();
	
	// get the player name and emit the signal already
	Person *p = getPerson(tk.GetFrom());
	assert(p);
	PrivateTalk.emit(p->getAccount(), say);
}

} // of namespace
