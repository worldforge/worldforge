#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/signal_system.h>
#include <algorithm>
#include <assert.h>

// various atlas headers we need
#include <Atlas/Bridge.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>

//#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Entity/Account.h>

#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Look.h>

#include "Player.h"
#include "Lobby.h"
#include "World.h"
#include "Connection.h"
#include "SignalDispatcher.h"
#include "ClassDispatcher.h"
#include "Utils.h"

typedef Atlas::Message::Object::ListType AtlasListType;
typedef Atlas::Message::Object::MapType AtlasMapType;

namespace Eris {

Player::Player(Connection *con) :
	_con(con),
	_account(""),
	_username(""),
	_lobby(Lobby::instance()),
	_world(NULL)
{
	_currentAction = "";
	assert(_con);

	_con->Connected.connect(SigC::slot(this, &Player::netConnected));
	_con->Failure.connect(SigC::slot(this, &Player::netFailure));
	
	Dispatcher *d = _con->getDispatcherByPath("op:error");
	assert(d);
	d->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Error>("player",
		SigC::slot(this, &Player::recvOpError)
	));
	
	_lobby->LoggedIn.connect(SigC::slot(this, &Player::loginComplete));
}	

Player::~Player()
{
	if (_con) {
		//_con->removeDispatcherByPath
		
		
	}
}

void Player::login(const std::string &uname,
	const std::string &password)
{
	if (!_con || (_con->getStatus() != BaseConnection::CONNECTED))
		throw InvalidOperation("Invalid connection");
	
	if (!_currentAction.empty())
		throw InvalidOperation("action in progress (" + _currentAction + ")");
	
	internalLogin(uname, password);
	
	// tell the lobby the serial number of the request, so
	// it knows which INFO op to use (assuming other INFOs going about..)
	_lobby->expectInfoRefno(_currentSerial);
	
	// store for re-logins
	_username = uname;
	_pass = password;
}

void Player::createAccount(const std::string &uname, 
	const std::string &name,
	const std::string &pwd)
{
	if (!_con || (_con->getStatus() != BaseConnection::CONNECTED))
		throw InvalidOperation("Invalid connection");
	
	if (!_currentAction.empty())
		throw InvalidOperation("action in progress (" + _currentAction + ")");

	// need option to create an admin object here
	Atlas::Objects::Entity::Player account = 
   		Atlas::Objects::Entity::Player::Instantiate();
 	account.SetId(uname);	// FIXME - I think this should be deprectaed in all of Atlas
 	account.SetPassword(pwd);
	account.SetName(name);
	account.SetAttr("username", uname);
	
 	Atlas::Message::Object::ListType args(1,account.AsObject());
	Atlas::Objects::Operation::Create c = 
  		Atlas::Objects::Operation::Create::Instantiate();
  	
	c.SetSerialno(getNewSerialno());
 	c.SetArgs(args);
	_con->send(c);
	
	_currentAction = "create-account";
	_currentSerial = c.GetSerialno();
	
	// let the lobby know what serial number to expect
	_lobby->expectInfoRefno(_currentSerial);
	
	// store for re-logins
	_username = uname;
	_pass = pwd;
}

void Player::logout()
{
	if (!_con)
		throw InvalidOperation("connection is invalid");
	
	if (!_con->isConnected()) {
		Eris::Log(LOG_WARNING, "connection not open, ignoring Player::logout");
		// FIXME - provide feedback here
		return;
	}
	
	Atlas::Objects::Operation::Logout l = 
		Atlas::Objects::Operation::Logout::Instantiate();
     	l.SetId(_account);
	l.SetSerialno(getNewSerialno());
	l.SetFrom(_account);
	
	_con->send(l);
	_currentAction = "logout";
	_currentSerial = l.GetSerialno();
}

CharacterList Player::getCharacters()
{
    if (_account.empty())
	    Eris::Log(LOG_ERROR, "Not logged into an account : getCharacter returning empty list");
    return _characters;
}

void Player::refreshCharacterInfo()
{
    if (!_con->isConnected())
	throw InvalidOperation("Not connected to server");
	
    _characters.clear();
	
    for (StringList::iterator I=_charIds.begin(); I!=_charIds.end(); ++I) {
	// send the look
	Atlas::Objects::Operation::Look lk =
		Atlas::Objects::Operation::Look::Instantiate();
	
	AtlasMapType args;
	args["id"] = *I;
	lk.SetArgs(AtlasListType(1, args));
	lk.SetFrom(_lobby->getAccountID());
	lk.SetTo(*I);
	lk.SetSerialno(getNewSerialno());
	
	_con->send(lk);
    }
}

World* Player::createCharacter(const Atlas::Objects::Entity::GameEntity &ent)
{
	if (!_lobby || _lobby->getAccountID().empty())
		throw InvalidOperation("no account exists!");

	if (!_con->isConnected())
		throw InvalidOperation("Not connected to server");	
	
	Atlas::Objects::Operation::Create c = 
		Atlas::Objects::Operation::Create::Instantiate();
	Atlas::Message::Object::ListType args(1,ent.AsObject());
	
	c.SetArgs(args);
	c.SetFrom(_lobby->getAccountID());
   	c.SetSerialno(getNewSerialno());
	
	if (!_world) {
		_world = new World(this, _con);
	}
	
	_con->send(c);
	
	return _world;
}

World* Player::takeCharacter(const std::string &id)
{
	StringList::iterator i = std::find(_charIds.begin(), _charIds.end(), id);
	if (i == _charIds.end())
		throw InvalidOperation("Character " + id + " not owned by player");
	
	if (!_con->isConnected())
		throw InvalidOperation("Not connected to server");
	
	Atlas::Objects::Operation::Look l = 
		Atlas::Objects::Operation::Look::Instantiate();
 
	l.SetFrom(id);  
	Atlas::Message::Object::MapType what;
	what["id"]=id;
	Atlas::Message::Object::ListType args(1,what);
	l.SetArgs(args);
  	l.SetSerialno(getNewSerialno());
	
	if (!_world)
		_world = new World(this, _con);
	_con->send(l);
	
  	return _world;
}

void Player::internalLogin(const std::string &uname, const std::string &pwd)
{
	Atlas::Objects::Entity::Account account = 
		Atlas::Objects::Entity::Account::Instantiate();
	account.SetId(uname);
 	account.SetPassword(pwd);

 	Atlas::Objects::Operation::Login l = 
   		Atlas::Objects::Operation::Login::Instantiate();
 	Atlas::Message::Object::ListType args(1,account.AsObject());
  	l.SetArgs(args);
	l.SetSerialno(getNewSerialno());
	
	_con->send(l);
	
	// setup error tracking
	_currentAction = "login";
	_currentSerial = l.GetSerialno();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// callbacks

void Player::loginComplete(const Atlas::Objects::Entity::Player &p)
{
    // FIXME - user p.GetUsername() to verify this is the correct player object
    
    _currentAction = "";
    _account = p.GetId();
    
    _charIds.clear();
    // extract character IDs
    Atlas::Message::Object::ListType cs = p.GetCharacters();
    for (Atlas::Message::Object::ListType::iterator C=cs.begin(); C!=cs.end(); ++C)
	_charIds.push_back(C->AsString());
    
    // setup dispatcher for sight of character ops
    Dispatcher *d = _con->getDispatcherByPath("op:oog:sight:entity");
    assert(d);
    
    if (d->getSubdispatch("character"))
	// second time around, don't try again
	return;
    
    d->addSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::GameEntity>("game_entity",
	SigC::slot(this, &Player::recvSightCharacter)),
	"character"
    );
    
    // notify an people watching us (as opposed to watching the lobby directly)
    LoginSuccess.emit();
    _con->Disconnecting.connect(SigC::slot(this, &Player::netDisconnecting));
}

void Player::recvOpError(const Atlas::Objects::Operation::Error &err)
{
	// skip errors if we're not doing anything
	if (_currentAction.empty() || (err.GetRefno() != _currentSerial))
		return;
	
	std::string serverMsg = getMember(getArg(err, 0), "message").AsString();
	// can actually use error[2]->parents here to detrmine the current action
	Eris::Log(LOG_WARNING, "Received Atlas error %s", serverMsg.c_str());
	
	std::string pr = getMember(getArg(err, 1), "parents").AsList().front().AsString();
	if (pr == "login") {
		// prevent re-logins on the account
		_username="";
		LoginFailure.emit(LOGIN_INVALID, serverMsg);
	}
	
	if (_currentAction == "create-account") {
		assert(pr == "create");
		// prevent re-logins on the account
		_username="";
		LoginFailure.emit(LOGIN_INVALID, serverMsg);
	}
	
	// clear these out
	_currentAction = "";
	_currentSerial = 0;
}

void Player::recvSightCharacter(const Atlas::Objects::Entity::GameEntity &ge)
{
    Eris::Log(LOG_DEBUG, "got sight of character %s", ge.GetName().c_str());
    
    _characters.push_back(ge);
    GotCharacterInfo.emit(ge);
    
    // check if we're all done
    if (_characters.size() == _charIds.size())
	GotAllCharacters.emit();
}

/* this will only ever get encountered after the connection is initally up;
thus we use it to trigger a reconnection. Note that some actions by
Lobby and World are also required to get everything back into the correct
state */

void Player::netConnected()
{
	// re-connection
	if (!_username.empty())
		internalLogin(_username, _pass);
	
	// ditto for IG (world)
}

bool Player::netDisconnecting()
{
	_con->lock();
	logout();
	// FIXME  -  decide what reponse lgout should have, install a handler
	// for it (in logout() itself), and then attach a signal to the handler
	// that calls _con->unlock();
	
	return false;
}

void Player::netFailure(const std::string& /*msg*/)
{
	; // do something useful here?
}

} // of namespace Eris
