#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Player.h>
#include <Eris/Connection.h>
#include <Eris/LogStream.h>
#include <Eris/Exceptions.h>
#include <Eris/Avatar.h>
#include <Eris/Router.h>

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

#include <sigc++/object_slot.h>

#include <algorithm>
#include <cassert>

using Atlas::Objects::Root;
using namespace Atlas::Objects::Operation;
using Atlas::Objects::Entity::GameEntity;
typedef Atlas::Objects::Entity::Account AtlasAccount;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

typedef std::map<int, Avatar*> RefnoAvatarMap;
static RefnoAvatarMap global_pendingInfoAvatars;

class AccountRouter : public Router
{
public:
    AccountRouter(Player* pl) :
        m_player(pl)
    {
        m_player->getConnection()->setDefaultRouter(this);
    }

    virtual ~AccountRouter()
    {
        //m_player->getConnection()->
    }

    /** specify the serialNo of the operation used to connect the
    account to the server (either a create or a login). */
    void setLoginSerial(int serialNo)
    {
        m_loginRefno = serialNo;
    }

    virtual RouterResult handleOperation(const RootOperation& op)
    {
         // errors
        if (op->instanceOf(ERROR_NO))
        {
            debug() << "player recived an error";
            if (m_player->isLoggedIn())
            {
                warning() << "Player got error op while logged in";
            } else {
                m_player->loginError(smart_dynamic_cast<Error>(op));
                return HANDLED;
            }
        }

        // logout
        if (op->instanceOf(LOGOUT_NO))
        {
            m_player->internalLogout(false);
            return HANDLED;
        }
           
    // only-when connected ops
        if (m_player->isLoggedIn() && (op->getTo() == m_player->getID()))
        {
            // character looks
            if (op->instanceOf(SIGHT_NO))
            {            
                const std::vector<Root>& args = op->getArgs();
                assert(!args.empty());
                GameEntity character = smart_dynamic_cast<GameEntity>(args.front());
                if (character.isValid())
                {
                    m_player->sightCharacter(character);
                    return HANDLED;
                }
            }
        }
        
        if (op->instanceOf(INFO_NO))
            return handleInfo(smart_dynamic_cast<Info>(op));
            
        return IGNORED;
    }

private:
    /** INFO operations mean various things, so the decode is slightly complex
    */
    RouterResult handleInfo(const Info& info)
    {
        const std::vector<Root>& args = info->getArgs();
        if (args.empty())
            throw InvalidOperation("Account got INFO() with no arguments");
            
        if (info->getRefno() == m_loginRefno)
        {            
            AtlasAccount acc = smart_dynamic_cast<AtlasAccount>(args.front());
            if (!acc.isValid()) 
                throw InvalidOperation("Account got INFO() whose's arg is not an account");
            
            m_player->loginComplete(acc);
            return HANDLED;
        }
        
        Logout logout = smart_dynamic_cast<Logout>(args.front());
        if (logout.isValid())
        {
            m_player->internalLogout(true);
            return HANDLED;
        }
           
        if (m_player->isLoggedIn() && (info->getTo() == m_player->getID()))
        {
            GameEntity ent = smart_dynamic_cast<GameEntity>(args.front());
            if (ent.isValid())
            {
                // IG transition info, maybe
                RefnoAvatarMap::iterator A = global_pendingInfoAvatars.find(info->getRefno());
                if (A != global_pendingInfoAvatars.end())
                {
                    A->second->setEntity(ent->getId());
                    global_pendingInfoAvatars.erase(A);
                    return HANDLED;
                } else
                    debug() << "Player got info(game_entity) with serial "
                     << info->getRefno() << ", but not a IG transition";
            }
        }
          
        return IGNORED;
    }

    Player* m_player;
    int m_loginRefno;
};

#pragma mark -

Player::Player(Connection *con) :
    m_con(con),
    m_status(DISCONNECTED),
    m_doingCharacterRefresh(false)
{
    if (!m_con)
        throw InvalidOperation("invalid Connection passed to Player");

    m_router = new AccountRouter(this);

    m_con->Connected.connect(SigC::slot(*this, &Player::netConnected));
    m_con->Failure.connect(SigC::slot(*this, &Player::netFailure));
}	

Player::~Player()
{
    delete m_router;
}

void Player::login(const std::string &uname, const std::string &password)
{
    if (!m_con->isConnected() || (m_status != DISCONNECTED))
    {
        error() << "called login on unconnected Connection / already logged-in Player";
        return;
    }
        	
    internalLogin(uname, password);
}

void Player::createAccount(const std::string &uname, 
	const std::string &fullName,
	const std::string &pwd)
{
    if (!m_con->isConnected() || (m_status != DISCONNECTED))
    {
        error() << "called createAccount on unconnected Connection / already logged-in Player";
        return;
    }

    m_status = LOGGING_IN;

// okay, build and send the create(account) op
    AtlasAccount account;
    account->setPassword(pwd);
    account->setName(fullName);
    account->setUsername(uname);
    account->setObjtype("obj");
    std::list<std::string> parents(1, "player");
    account->setParents(parents);
    
    Create c;
    c->setSerialno(getNewSerialno());
    c->setArgs1(account);
    
    m_router->setLoginSerial(c->getSerialno());
    m_con->send(c);
	
// store for re-logins
    m_username = uname;
    m_pass = pwd;
}

void Player::logout()
{
    if (!m_con->isConnected() || (m_status != LOGGED_IN))
    {
        error() << "called logout on bad connection / unconnected Player, ignoring";
        return;
    }
    
    m_status = LOGGING_OUT;
    
    Logout l;
    l->setId(m_accountId);
    l->setSerialno(getNewSerialno());
    l->setFrom(m_accountId);
    
    m_con->send(l);
	
    _logoutTimeout = new Timeout("logout", this, 5000);
    _logoutTimeout->Expired.connect(SigC::slot(*this, &Player::handleLogoutTimeout));
}

const CharacterMap& Player::getCharacters()
{
    if (m_status != LOGGED_IN)
        error() << "Not logged into an account : getCharacter returning empty dictionary";
    
    if (m_doingCharacterRefresh)
        warning() << "client retrieving partial / incomplete character dictionary";
    
    return _characters;
}

void Player::refreshCharacterInfo()
{
    if (!m_con->isConnected() || (m_status != LOGGED_IN))
    {
        error() << "called refreshCharacterInfo on unconnected Player, ignoring";
        return;
    }    
    
    if (m_doingCharacterRefresh)
        return; // silently ignore overlapping refreshes
        
    _characters.clear();

    if (m_characterIds.empty())
    {
        GotAllCharacters.emit(); // we must emit the done signal
        return;
    }
    
// okay, now we know we have at least one character to lookup, set the flag
    m_doingCharacterRefresh = true;
    
    Look lk;
    Root obj;
    lk->setFrom(m_accountId);
        
    for (StringSet::iterator I=m_characterIds.begin(); I!=m_characterIds.end(); ++I)
    {
        obj->setId(*I);
        lk->setArgs1(obj);
        lk->setSerialno(getNewSerialno());
        m_con->send(lk);
    }
}

Avatar* Player::createCharacter(const Atlas::Objects::Entity::GameEntity &ent)
{
    if (!m_con->isConnected() || (m_status != LOGGED_IN))
    {
        error() << "called createCharacter on unconnected Player, ignoring";
        return NULL;
    }    

    Create c;    
    c->setArgs1(ent);
    c->setFrom(m_accountId);
    c->setSerialno(getNewSerialno());
    m_con->send(c);

    Avatar *av = new Avatar(this);
    global_pendingInfoAvatars[c->getSerialno()] = av;
    return av;
}

/*
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
    if (serialno)
        NewCharacter((new World(this, _con))->createAvatar(serialno));
}
*/

Avatar* Player::takeCharacter(const std::string &id)
{
    StringSet::iterator C = m_characterIds.find(id);
    if (C == m_characterIds.end())
    {
        error() << "Character '" << id << "' not owned by Player " << m_username;
        return NULL;
    }
	
    Root what;
    what->setId(id);
    
    Look l;
    l->setFrom(id);  // should this be m_accountId?
    l->setArgs1(what);
    l->setSerialno(getNewSerialno());
    m_con->send(l);
    
    Avatar *av = new Avatar(this);
    debug() << "setting pendingInfoAvatars entry " << l->getSerialno() 
        << " to " << av;
        
    global_pendingInfoAvatars[l->getSerialno()] = av;
    return av;
}

#pragma mark -

void Player::internalLogin(const std::string &uname, const std::string &pwd)
{
    assert(m_status == DISCONNECTED);
    m_status = LOGGING_IN;
    m_username = uname; // store for posterity

    AtlasAccount account;
    account->setPassword(pwd);
    account->setUsername(uname);

    Login l;
    l->setArgs1(account);
    l->setSerialno(getNewSerialno());
    
    m_router->setLoginSerial(l->getSerialno());
    m_con->send(l);
}

void Player::internalLogout(bool clean)
{
    if (clean) {
        if (m_status != LOGGING_OUT)
        {
            error() << "got clean logout, but not logging out already";
        }
    } else {
        if (m_status != LOGGED_IN)
        {
            error() << "got forced logout, but not currently logged in";
        }
    }
    
    m_status = DISCONNECTED;
    
    if (_logoutTimeout)
        delete _logoutTimeout;
	
    m_con->disconnect();
    LogoutComplete.emit(clean);
}

void Player::loginComplete(const AtlasAccount &p)
{
    if (m_status != LOGGING_IN)
        error() << "got loginComplete, but not currently logging in!";
        
    if (p->getUsername()  != m_username)
        error() << "missing or incorrect username on login INFO";
        
    m_status = LOGGED_IN;
    m_accountId = p->getId();
    debug() << "login, account ID is " << m_accountId;
    
    m_characterIds = StringSet(p->getCharacters().begin(), p->getCharacters().end());
    // notify an people watching us 
    LoginSuccess.emit();
      
    m_con->Disconnecting.connect(SigC::slot(*this, &Player::netDisconnecting));
}

void Player::loginError(const Error& err)
{
    if (m_status != LOGGING_IN)
        error() << "got loginError while not logging in";
        
    const std::vector<Root>& args = err->getArgs();
    std::string msg = args[0]->getAttr("message").asString();
    warning() << "got login error: " << msg;
    LoginFailure.emit(msg);
}

void Player::sightCharacter(const GameEntity& ge)
{
    if (!m_doingCharacterRefresh)
    {
        error() << "got sight of character " << ge->getId() << " while outside a refresh, ignoring";
        return;
    }
    
    CharacterMap::iterator C = _characters.find(ge->getId());
    if (C != _characters.end())
    {
        error() << "duplicate sight of character " << ge->getId();
        return;
    }
    
    // okay, we can now add it to our map
    _characters.insert(C, CharacterMap::value_type(ge->getId(), ge));
    GotCharacterInfo.emit(ge);
    
// check if we'redone
    if (_characters.size() == m_characterIds.size())
    {
	GotAllCharacters.emit();
        m_doingCharacterRefresh = false;
    }
}

/* this will only ever get encountered after the connection is initally up;
thus we use it to trigger a reconnection. Note that some actions by
Lobby and World are also required to get everything back into the correct
state */

void Player::netConnected()
{
    // re-connection
    if (!m_username.empty())
        internalLogin(m_username, m_pass);
}

bool Player::netDisconnecting()
{
    if (m_status == LOGGED_IN) {
        m_con->lock();
        logout();
        // FIXME  -  decide what reponse logout should have, install a handler
        // for it (in logout() itself), and then attach a signal to the handler
        // that calls m_con->unlock();
        return false;
    } else
        return true;
}

void Player::netFailure(const std::string& /*msg*/)
{
  
}

void Player::handleLogoutTimeout()
{
    error() << "LOGOUT timed out waiting for response";
    
    m_status = DISCONNECTED;
    delete _logoutTimeout;
    
    LogoutComplete.emit(false);
}

} // of namespace Eris
