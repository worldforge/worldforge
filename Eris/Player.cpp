#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/signal_system.h>
#include <algorithm>

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

namespace Eris {

Player::Player() :
	_con(NULL),
	_account(""),
	_username(""),
	_lobby(NULL),
	_world(NULL)
{
	_currentAction = "";
}

Player::~Player()
{
	;
}

Lobby* Player::login(Connection *con,
	const std::string &uname,
	const std::string &password)
{
	if (!con || (con->getStatus() != BaseConnection::CONNECTED))
		throw InvalidOperation("Invalid connection");
	
	if (!_currentAction.empty())
		throw InvalidOperation("action in progress (" + _currentAction + ")");
	
	_con = con;
	_lobby = new Lobby(this, con);
	
	// notification when we are 'in'
	_lobby->LoggedIn.connect(SigC::slot(this, &Player::loginComplete));
	
	_con->Connected.connect(SigC::slot(this, &Player::netConnected));
	
	Dispatcher *d = _con->getDispatcherByPath("op:error");
	d->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Error>("player",
		SigC::slot(this, &Player::recvOpError)
	));
	
	internalLogin(uname, password);
	
	// tell the lobby the serial number of the request, so
	// it knows which INFO op to use (assuming other INFOs going about..)
	_lobby->expectInfoRefno(_currentSerial);
	
	// store for re-logins
	_username = uname;
	_pass = password;
	
	return _lobby;
}

Lobby* Player::createAccount(Connection *con, 
	const std::string &uname, 
	const std::string &pwd)
{
	if (!con || (con->getStatus() != BaseConnection::CONNECTED))
		throw InvalidOperation("Invalid connection");
	_con = con;
	
	if (!_currentAction.empty())
		throw InvalidOperation("action in progress (" + _currentAction + ")");
	
	_con->Connected.connect(SigC::slot(this, &Player::netConnected));
	_lobby = new Lobby(this, con);
	
	// notification when we are 'in'
	_lobby->LoggedIn.connect(SigC::slot(this, &Player::loginComplete));
	
	// need option to create an admin object here
	Atlas::Objects::Entity::Player account = 
   		Atlas::Objects::Entity::Player::Instantiate();
 	account.SetId(uname);
 	account.SetPassword(pwd);

 	Atlas::Message::Object::ListType args(1,account.AsObject());
	Atlas::Objects::Operation::Create c = 
  		Atlas::Objects::Operation::Create::Instantiate();
  	
	c.SetSerialno(getNewSerialno());
 	c.SetArgs(args);
	con->send(c);
	
	Dispatcher *d = _con->getDispatcherByPath("op:error");
	assert(d);
	d->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Error>("player",
		SigC::slot(this, &Player::recvOpError)
	));
	
	_currentAction = "create-account";
	_currentSerial = c.GetSerialno();
	
	// let the lobby know what serial number to expect
	_lobby->expectInfoRefno(_currentSerial);
	
	// store for re-logins
	_username = uname;
	_pass = pwd;
	
	// set a callback for failure
	
	return _lobby;
}

void Player::logout()
{
	if (!_con)
		throw InvalidOperation("connection is invalid");
	
	if (!_con->isConnected())
		// FIXME - provide feedback here
		return;
	
	Atlas::Objects::Operation::Logout l = 
		Atlas::Objects::Operation::Logout::Instantiate();
     	l.SetId(_account);
	l.SetSerialno(getNewSerialno());
	
	//_con->send(&l);
	//l.SendContents(_con->GetEncoder());
	_con->send(l);
	
	_currentAction = "logout";
	_currentSerial = l.GetSerialno();
}

StringList Player::getCharacters()
{
	if (_account.empty())
		throw InvalidOperation("Not logged into account yet");
	return _characters;
}

void Player::requestCharacter(const std::string &id)
{
	StringList::iterator i = std::find(_characters.begin(), _characters.end(), id);
	if (i == _characters.end())
		throw InvalidOperation("Character " + id + " not owned by player");
	
	if (!_con->isConnected())
		throw InvalidOperation("Not connected to server");
	
	// send the look
	Atlas::Objects::Operation::Look lk =
		Atlas::Objects::Operation::Look::Instantiate();
	
	lk.SetFrom(_lobby->getAccountID());
	lk.SetTo(id);
	lk.SetSerialno(getNewSerialno());
	
	_con->send(lk);
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
	
	_world = new World(this, _con);
	_con->send(c);
	
	return _world;
}

World* Player::takeCharacter(const std::string &id)
{
	StringList::iterator i = std::find(_characters.begin(), _characters.end(), id);
	if (i == _characters.end())
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
	
  	_world = new World(this, _con);
	//l.SendContents(_con->GetEncoder()); 
	_con->send(l);
	
  	return _world;
}

/*
void Player::ActionComplete
{
	// character (game entity) callback	
	Dispatcher *ge = d->AddSubdispatch(new ClassDispatcher("game_entity", "gameentity"));
	ge->AddSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::GameEntity>("lobby", 
		SigC::slot(this, &Lobby::RecvSightEntity)
	));
}
*/


void Player::internalLogin(const string &uname, const string &pwd)
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
	
	// extract character IDs
	Atlas::Message::Object::ListType cs = p.GetCharacters();
	for (Atlas::Message::Object::ListType::iterator C=cs.begin(); C!=cs.end(); ++C)
		_characters.push_back(C->AsString());
	
	// setup dispatcher for sight of character ops
	Dispatcher *d = _con->getDispatcherByPath("op:oog:sight:entity");
	assert(d);
	
	Dispatcher *ged = d->addSubdispatch(new ClassDispatcher("character", "game_entity"));
	ged->addSubdispatch(new SignalDispatcher<Atlas::Objects::Entity::GameEntity>("player",
		SigC::slot(this, &Player::recvSightCharacter)
	));
}

void Player::recvOpError(const Atlas::Objects::Operation::Error &err)
{
	cerr << "recieved atlas error!" << endl;
	// work out which operation went wrong
	
	if (_currentAction.empty())
		// nothing to do with us, guv'
		return;
		
	if (err.GetRefno() != _currentSerial) {
		cerr << "skipping non-targeted error" << endl;
		return;
	}
	
	std::string serverMsg = getMember(getArg(err, 0), "message").AsString();
	cerr << "Got atlas error, msg is " << serverMsg << endl;
	// can actually use error[2]->parents here to detrmine the current action
	
	std::string pr = getMember(getArg(err, 1), "parents").AsList().front().AsString();
	if (pr == "login") {
		// prevent re-logins on the account
		_username="";
		throw InvalidOperation("Login failed, server reponded: " + serverMsg);
	}
	
	if (_currentAction == "create-account") {
		assert(pr == "create");
		// prevent re-logins on the account
		_username="";
		throw InvalidOperation("Create-Account failed, server responded: " + serverMsg);
	}
	
	// FIXME - handle failures of create account / take-over / create character
	assert(false);
}

void Player::recvSightCharacter(const Atlas::Objects::Entity::GameEntity &ge)
{
	
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

/*
void Player::LobbyFailure()
{
	
}
*/
}; // of namespace Eris
