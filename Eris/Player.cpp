#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/signal_system.h>
#else
#include <sigc++/signal.h>
#endif
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

#include <Eris/Player.h>
#include <Eris/Lobby.h>
#include <Eris/World.h>
#include <Eris/Connection.h>
#include <Eris/SignalDispatcher.h>
#include <Eris/ClassDispatcher.h>
#include <Eris/Utils.h>
#include <Eris/Log.h>

typedef Atlas::Message::Element::ListType AtlasListType;
typedef Atlas::Message::Element::MapType AtlasMapType;

using namespace Atlas::Objects;

namespace Eris {

Player::Player(Connection *con) :
	_con(con),
	_account(""),
	_username(""),
	_lobby(con->getLobby())
{
    _currentAction = "";
    _logoutTimeout = NULL;
    
	assert(_con);

	_con->Connected.connect(SigC::slot(*this, &Player::netConnected));
	_con->Failure.connect(SigC::slot(*this, &Player::netFailure));
	
	Dispatcher *d = _con->getDispatcherByPath("op:error");
	assert(d);
	d->addSubdispatch(new SignalDispatcher<Operation::Error>("player",
		SigC::slot(*this, &Player::recvOpError)
	));

	d = _con->getDispatcherByPath("op");
	d = d->addSubdispatch(ClassDispatcher::newAnonymous(_con));
	d->addSubdispatch(new SignalDispatcher<Operation::Logout>("logout",
		SigC::slot(*this, &Player::recvRemoteLogout)), "logout");
	
	_lobby->LoggedIn.connect(SigC::slot(*this, &Player::loginComplete));
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
 	account.setId(uname);	// FIXME - I think this should be deprectaed in all of Atlas
 	account.setPassword(pwd);
	account.setName(name);
	account.setAttr("username", uname);
	
 	Atlas::Message::Element::ListType args(1,account.asObject());
	Atlas::Objects::Operation::Create c = 
  		Atlas::Objects::Operation::Create::Instantiate();
  	
	c.setSerialno(getNewSerialno());
 	c.setArgs(args);
	_con->send(c);
	
	_currentAction = "create-account";
	_currentSerial = c.getSerialno();
	
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
	    Eris::log(LOG_WARNING, "connection not open, ignoring Player::logout");
	    // FIXME - provide feedback here
	    return;
    }
	
    if (!_currentAction.empty()) {
		Eris::log(LOG_WARNING, "got logout with action (%s) already in progress",
	    	_currentAction.c_str());
		return;
    }
    
    Atlas::Objects::Operation::Logout l = 
	    Atlas::Objects::Operation::Logout::Instantiate();
    l.setId(_account);
    l.setSerialno(getNewSerialno());
    l.setFrom(_account);
    
    _con->send(l);
    _currentAction = "logout";
    _currentSerial = l.getSerialno();
	
    _logoutTimeout = new Timeout("logout", this, 5000);
    _logoutTimeout->Expired.connect(SigC::slot(*this, &Player::handleLogoutTimeout));
}

CharacterList Player::getCharacters()
{
    if (_account.empty())
	    Eris::log(LOG_ERROR, "Not logged into an account : getCharacter returning empty list");
    return _characters;
}

void Player::refreshCharacterInfo()
{
    if (!_con->isConnected())
		throw InvalidOperation("Not connected to server");
	
    // we need to be logged in too
    if (_account.empty()) {
		Eris::log(LOG_ERROR, "refreshCharacterInfo: Not logged into an account yet");
		return;
    }
    
    _characters.clear();

    if (_charIds.empty()) {
        GotAllCharacters.emit();
        return;
    }
	
    if (_charIds.empty()) {
		// handle the case where there are no characters; we should still emit the signal
		GotAllCharacters.emit();
		return;
    }
    
    for (StringList::iterator I=_charIds.begin(); I!=_charIds.end(); ++I) {
		// send the look
		Atlas::Objects::Operation::Look lk =
			Atlas::Objects::Operation::Look::Instantiate();
		
		AtlasMapType args;
		args["id"] = *I;
		lk.setArgs(AtlasListType(1, args));
		lk.setFrom(_lobby->getAccountID());
		lk.setTo(*I);
		lk.setSerialno(getNewSerialno());
		
		_con->send(lk);
    }
}

Avatar* Player::createCharacter(const Atlas::Objects::Entity::GameEntity &ent)
{
	if (!_lobby || _lobby->getAccountID().empty())
		throw InvalidOperation("no account exists!");

	if (!_con->isConnected())
		throw InvalidOperation("Not connected to server");

	if (ent.getName().empty())
		throw InvalidOperation("Character unnamed");

	Atlas::Objects::Operation::Create c = 
		Atlas::Objects::Operation::Create::Instantiate();
	Atlas::Message::Element::ListType args(1,ent.asObject());

	c.setArgs(args);
	c.setFrom(_lobby->getAccountID());
 	c.setSerialno(getNewSerialno());

	World* world = new World(this, _con);
	Avatar* avatar = world->createAvatar(c.getSerialno());

	_con->send(c);

	return avatar;
}

void Player::createCharacter()
{
	if (!_lobby || _lobby->getAccountID().empty())
		throw InvalidOperation("no account exists!");

	if (!_con->isConnected())
		throw InvalidOperation("Not connected to server");

	throw InvalidOperation("No UserInterface handler defined");

	// FIXME look up the dialog, create the instance,
	// hook in a slot to feed the serialno of any Create op
	// the dialog passes back to createCharacterHandler()
}

void Player::createCharacterHandler(long serialno)
{
	if(serialno)
		NewCharacter((new World(this, _con))->createAvatar(serialno));
}

Avatar* Player::takeCharacter(const std::string &id)
{
	StringList::iterator i = std::find(_charIds.begin(), _charIds.end(), id);
	if (i == _charIds.end())
		throw InvalidOperation("Character " + id + " not owned by player");
	
	if (!_con->isConnected())
		throw InvalidOperation("Not connected to server");
	
	Atlas::Objects::Operation::Look l = 
		Atlas::Objects::Operation::Look::Instantiate();
 
	l.setFrom(id);  
	Atlas::Message::Element::MapType what;
	what["id"]=id;
	Atlas::Message::Element::ListType args(1,what);
	l.setArgs(args);
  	l.setSerialno(getNewSerialno());
	
	World* world = new World(this, _con);
	Avatar* avatar = world->createAvatar(l.getSerialno(), id);
	
	_con->send(l);

	return avatar;
}

void Player::internalLogin(const std::string &uname, const std::string &pwd)
{
	Atlas::Objects::Entity::Account account = 
		Atlas::Objects::Entity::Account::Instantiate();
	account.setId(uname);
 	account.setPassword(pwd);
	account.setAttr("username", uname);

 	Atlas::Objects::Operation::Login l = 
   		Atlas::Objects::Operation::Login::Instantiate();
 	Atlas::Message::Element::ListType args(1,account.asObject());
  	l.setArgs(args);
	l.setSerialno(getNewSerialno());
	
	_con->send(l);
	
	// setup error tracking
	_currentAction = "login";
	_currentSerial = l.getSerialno();
}

void Player::internalLogout(bool clean)
{
    _currentAction = "";
    if (_logoutTimeout)
		delete _logoutTimeout;
	
	_con->disconnect();
	
	LogoutComplete.emit(clean);
}

const std::string& Player::getAccountID() const
{
    return _account;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// callbacks

void Player::loginComplete(const Atlas::Objects::Entity::Player &p)
{
    // FIXME - user p.GetUsername() to verify this is the correct player object
    
    _currentAction = "";
    _account = p.getId();
    
    _charIds.clear();
    // extract character IDs
    Atlas::Message::Element::ListType cs = p.getCharacters();
    for (Atlas::Message::Element::ListType::iterator C=cs.begin(); C!=cs.end(); ++C)
		_charIds.push_back(C->asString());
    
    // setup dispatcher for sight of character ops
    Dispatcher *d = _con->getDispatcherByPath("op:oog:sight:entity");
    assert(d);
    
    // notify an people watching us (as opposed to watching the lobby directly)
    LoginSuccess.emit();
    
    if (d->getSubdispatch("character"))
		// second time around, don't try again
		return;
    
    // there will be several anonymous children, I guess
    d = d->addSubdispatch(ClassDispatcher::newAnonymous(_con));
    d->addSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::GameEntity>("character",
	SigC::slot(*this, &Player::recvSightCharacter)),
	"game_entity"
    );
    
    d = _con->getDispatcherByPath("op:info:op");
    Dispatcher *infoLogout = d->addSubdispatch(ClassDispatcher::newAnonymous(_con));
    infoLogout->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Logout>(
	"player", SigC::slot(*this, &Player::recvLogoutInfo)),
	"logout"
    );
    
    _con->Disconnecting.connect(SigC::slot(*this, &Player::netDisconnecting));
}

void Player::recvOpError(const Atlas::Objects::Operation::Error &err)
{
	// skip errors if we're not doing anything
	if (_currentAction.empty() || (err.getRefno() != _currentSerial))
		return;
	
	std::string serverMsg = getMember(getArg(err, 0), "message").asString();
	// can actually use error[2]->parents here to detrmine the current action
	Eris::log(LOG_WARNING, "Received Atlas error %s", serverMsg.c_str());
	
	std::string pr = getMember(getArg(err, 1), "parents").asList().front().asString();
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
    Eris::log(LOG_DEBUG, "got sight of character %s", ge.getName().c_str());
    
    _characters.push_back(ge);
    GotCharacterInfo.emit(ge);
    
    // check if we're all done
    if (_characters.size() == _charIds.size())
	GotAllCharacters.emit();
}

void Player::recvLogoutInfo(const Atlas::Objects::Operation::Logout &lo)
{
    Eris::log(LOG_DEBUG, "got INFO(logout)");
    internalLogout(true);
}

void Player::recvRemoteLogout(const Operation::Logout &op)
{	
	Eris::log(LOG_DEBUG, "got server-initated LOGOUT");
	internalLogout(false);
}

/* this will only ever get encountered after the connection is initally up;
thus we use it to trigger a reconnection. Note that some actions by
Lobby and World are also required to get everything back into the correct
state */

void Player::netConnected()
{
	// re-connection
	if (!_username.empty() && _currentAction.empty())
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

void Player::handleLogoutTimeout()
{
    Eris::log(LOG_DEBUG, "LOGOUT timed out waiting for response");
    
    _currentAction = "";
    delete _logoutTimeout;
    
    LogoutComplete.emit(false);
}

} // of namespace Eris
