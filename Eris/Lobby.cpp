#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/signal_system.h>
#include <algorithm>

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

using namespace Atlas::Objects;
using namespace Atlas;

namespace Eris {

Lobby::Lobby(Player *p, Connection *con) :
	Room(this),
	_account(""),
	_con(con),
	_player(p),
	_reconnect(false),
	_infoRefno(-1)
{
	assert(_con);
	_con->Failure.connect(SigC::slot(this, &Lobby::netFailure));
	_con->Connected.connect(SigC::slot(this, &Lobby::netConnected));
	
	// the info callback	
	Dispatcher *d = _con->getDispatcherByPath("op:info");
	assert(d);

	d->addSubdispatch( new SignalDispatcher<Operation::Info>("lobby", 
		SigC::slot(this, &Lobby::recvOpInfo)
	));
}
	
Lobby::~Lobby()
{
	_con->removeDispatcherByPath("op:oog:sight:entity:player", "lobby");
	_con->removeDispatcherByPath("op:oog:sight:entity:room", "lobby");
	
	// FIXME - unecesary when Cyphesis updates to new OOG standards
	_con->removeDispatcherByPath("op:oog:sight:entity:lobby", "lobby");
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
	
	//cerr << "requesting join of room " << roomID << endl;
	
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
	} else
		return R->second;
}

Person* Lobby::getPerson(const std::string &acc)
{
	PersonDict::iterator i = _peopleDict.find(acc);
	if (i == _peopleDict.end()) {
		//cerr << "looking up account " << acc << endl;
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
	
	Dispatcher *rop = _con->getDispatcherByPath("op");
	assert(rop);	
	Dispatcher *oogd = rop->addSubdispatch(new OpToDispatcher("oog", _account));
	
	// add in the basics so rooms can hook below
	Dispatcher *sndd = oogd->addSubdispatch(new EncapDispatcher("sound", "sound"));
	Dispatcher *d = sndd->addSubdispatch(new OpToDispatcher("private", _account));
	d = d->addSubdispatch(new ClassDispatcher("talk", "talk"));
	d->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Talk>("lobby",
		SigC::slot(this, &Lobby::recvPrivateChat)
	));
	
	oogd->addSubdispatch(new ClassDispatcher("appearance", "appearance"));
	oogd->addSubdispatch(new ClassDispatcher("disappearance", "disappearance"));
	
	Dispatcher *sight = oogd->addSubdispatch(new EncapDispatcher("sight", "sight"));
	d = sight->addSubdispatch(new TypeDispatcher("op", "op"));
	
	d = sight->addSubdispatch(new TypeDispatcher("entity", "object"));
	// the room entity callback
	Dispatcher *room = d->addSubdispatch(new ClassDispatcher("room", "room"));
	room->addSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::RootEntity>("lobby", 
		SigC::slot(this, &Lobby::recvSightRoom)
	));
	
	// the account / player object callback
	Dispatcher *pl = d->addSubdispatch(new ClassDispatcher("player", "player"));
	pl->addSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::Account>("lobby", 
		SigC::slot(this, &Lobby::recvSightPerson)
	));
	
	// FIXME - temporary support for Cyphesis's 'lobby' entity
	Dispatcher *lobby = d->addSubdispatch(new ClassDispatcher("lobby", "lobby"));
	lobby->addSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::RootEntity>
		("lobby", SigC::slot(this, &Lobby::recvSightLobby)
	));
}

void Lobby::netFailure(std::string msg)
{
	// what needs to be done here?
}

void Lobby::netDisconnected()
{
	// ditto
}

void Lobby::netConnected()
{
	// probably in response to a re-conection, so we need to log in again
	_reconnect = true;
	
	// the info callback; note that in the reconnect case, this is parented
	// to OOG, not directly to OP (becuase we know what the account ID
	// is going to be)
	Dispatcher *d = _con->getDispatcherByPath("op:oog");
	assert(d);
	
	d = d->addSubdispatch(new ClassDispatcher("info", "info")); 
	d->addSubdispatch( new SignalDispatcher<Operation::Info>("lobby", 
		SigC::slot(this, &Lobby::recvOpInfo)
	));
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

void Lobby::recvOpInfo(const Operation::Info &ifo)
{
	// reject lots pof extraneous INFOs we don't care about
	// FIXME  - enable the refno test once stage correclty processes it
	if (!_account.empty() && (ifo.GetTo() != _account) /*|| (ifo.getRefno() != _infoRefno)*/) return;
	
	_infoRefno = -1; // clear the expect value to reject further INFOs (unless we set them!);
	
	const Atlas::Message::Object &account = getArg(ifo, 0);
	// build an Account object (isn't this what decoders are for?)
	Atlas::Objects::Entity::Player acc = 
		Atlas::Objects::Entity::Player::Instantiate();
	
	Atlas::Message::Object::MapType::const_iterator I;
	for(I = account.AsMap().begin(); I != account.AsMap().end(); I++)
		acc.SetAttr(I->first, I->second);
	  
	// verify things look okay [note we can't use inheritance querying yet, I suspect ..]
	if (!checkInherits(acc, "account") && !checkInherits(acc, "player"))
		throw IllegalObject(acc, "Lobby INFO argument 0 is not an account object");
	
	_account = acc.GetId();
	
	if (!_reconnect) // obviously only register first time around
		registerCallbacks();
	
	// broadcast the login
	LoggedIn.emit(acc);
	
	if (_reconnect) {
		// don't issue an annonymous look; there is basically no need,
		// and it would confuse the logic in RecvSightRoom (calls Setup twice, etc)
		look(_id);
		_con->removeDispatcherByPath("op:oog:info", "lobby");
	} else {
		look("");
		_con->removeDispatcherByPath("op:info", "lobby");
	}
}

/*
void Lobby::RecvOpInfoRecon(const Operation::Info &ifo)
{
	const Atlas::Message::Object &account = GetArg(ifo, 0);
	// build an Account object (isn't this what decoders are for?)
	Atlas::Objects::Entity::Player acc = 
		Atlas::Objects::Entity::Player::Instantiate();
	
	Atlas::Message::Object::MapType::const_iterator I;
	for(I = account.AsMap().begin(); I != account.AsMap().end(); I++)
		acc.SetAttr(I->first, I->second);
	  
	// verify things look okay [note we can't use inheritance querying yet, I suspect ..]
	if (!CheckInherits(acc, "account") && !CheckInherits(acc, "player"))
		throw IllegalObject(acc, "Lobby INFO argument 0 is not an account object");
	
	if (_account != acc.GetId()) 
		throw InvalidOperation("Mismatch beteen LOGIN INFO account and previous account ID");
	
	// broadcast the login
	LoggedIn.emit(acc);
	
	// perform the initial look
	Look("");
	
	_con->RemoveDispatcherByPath("op:oog:info", "lobby");
}
*/

void Lobby::recvSightPerson(const Atlas::Objects::Entity::Account &ac)
{
	PersonDict::iterator i = _peopleDict.find(ac.GetId());
	if (i == _peopleDict.end()) {
		cerr << "got un-requested sight of person!" << endl;
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
		//cerr << "doing root-room (lobby) boot-strap" << endl;
		Log("recieved sight of root room (lobby)");
		_roomDict[room.GetId()] = this;
		
		_id = room.GetId();
		// now id is valid, can safely call Setup on the lobby
		Room::setup();
		
		Room::sight(room);
	} else {
		RoomDict::iterator i = _roomDict.find(room.GetId());
		if (i == _roomDict.end())
			throw InvalidOperation("Got sight of unknown room!");
		
		Log("recived sight of room %s", i->first.c_str());
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

}; // of namespace
