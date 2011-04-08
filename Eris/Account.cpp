#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Account.h>
#include <Eris/Connection.h>
#include <Eris/LogStream.h>
#include <Eris/Exceptions.h>
#include <Eris/Avatar.h>
#include <Eris/Router.h>
#include <Eris/Response.h>
#include <Eris/DeleteLater.h>
#include <Eris/Timeout.h>
#include "SpawnPoint.h"
#include "CharacterType.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#pragma warning(disable: 4068)  //unknown pragma

using Atlas::Objects::Root;
using Atlas::Message::Element;
using namespace Atlas::Objects::Operation;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
typedef Atlas::Objects::Entity::Account AtlasAccount;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

class AccountRouter : public Router
{
public:
    AccountRouter(Account* pl) :
        m_account(pl)
    {
        m_account->getConnection()->setDefaultRouter(this);
    }

    virtual ~AccountRouter()
    {
        m_account->getConnection()->clearDefaultRouter();
    }

    virtual RouterResult handleOperation(const RootOperation& op)
    {
        // logout
        if (op->getClassNo() == LOGOUT_NO) {
            debug() << "Account received forced logout from server";
            m_account->internalLogout(false);
            return HANDLED;
        }

        if ((op->getClassNo() == SIGHT_NO) && (op->getTo() == m_account->getId()))
        {
            const std::vector<Root>& args = op->getArgs();
            AtlasAccount acc = smart_dynamic_cast<AtlasAccount>(args.front());
            m_account->updateFromObject(acc);

            // refresh character data if it changed
            if (!acc->isDefaultCharacters()) m_account->refreshCharacterInfo();

            return HANDLED;
        }

        return IGNORED;
    }

private:
    Account* m_account;
};

#pragma mark -

Account::Account(Connection *con) :
    m_con(con),
    m_status(DISCONNECTED),
    m_doingCharacterRefresh(false)
{
    if (!m_con) throw InvalidOperation("invalid Connection passed to Account");

    m_router = new AccountRouter(this);

    m_con->Connected.connect(sigc::mem_fun(this, &Account::netConnected));
    m_con->Failure.connect(sigc::mem_fun(this, &Account::netFailure));
}

Account::~Account()
{
    ActiveCharacterMap::iterator it;
    for (it = m_activeCharacters.begin(); it != m_activeCharacters.end(); )
    {
        ActiveCharacterMap::iterator cur = it++;
        deactivateCharacter(cur->second); // send logout op
        // cur gets invalidated by innerDeactivateCharacter
        delete cur->second;
    }

    if (isLoggedIn()) logout();
    delete m_router;
}

Result Account::login(const std::string &uname, const std::string &password)
{
    if (!m_con->isConnected()) {
        error() << "called login on unconnected Connection";
        return NOT_CONNECTED;
    }

    if (m_status != DISCONNECTED) {
        error() << "called login, but state is not currently disconnected";
        return ALREADY_LOGGED_IN;
    }

    return internalLogin(uname, password);
}

Result Account::createAccount(const std::string &uname,
    const std::string &fullName,
    const std::string &pwd)
{
    if (!m_con->isConnected()) return NOT_CONNECTED;
    if (m_status != DISCONNECTED) return ALREADY_LOGGED_IN;

    m_status = LOGGING_IN;

// okay, build and send the create(account) op
    AtlasAccount account;
    account->setPassword(pwd);
    account->setName(fullName);
    account->setUsername(uname);

    Create c;
    c->setSerialno(getNewSerialno());
    c->setArgs1(account);

    m_con->getResponder()->await(c->getSerialno(), this, &Account::loginResponse);
    m_con->send(c);

// store for re-logins
    m_username = uname;
    m_pass = pwd;

    m_timeout.reset(new Timeout(5000));
    m_timeout->Expired.connect(sigc::mem_fun(this, &Account::handleLoginTimeout));

    return NO_ERR;
}

Result Account::logout()
{
    if (!m_con->isConnected()) {
        error() << "called logout on bad connection ignoring";
        return NOT_CONNECTED;
    }

    if (m_status == LOGGING_OUT) return NO_ERR;

    if (m_status != LOGGED_IN) {
        error() << "called logout on non-logged-in Account";
        return NOT_LOGGED_IN;
    }

    m_status = LOGGING_OUT;

    Logout l;
    Anonymous arg;
    arg->setId(m_accountId);
    l->setArgs1(arg);
    l->setSerialno(getNewSerialno());

    m_con->getResponder()->await(l->getSerialno(), this, &Account::logoutResponse);
    m_con->send(l);

    m_timeout.reset(new Timeout(5000));
    m_timeout->Expired.connect(sigc::mem_fun(this, &Account::handleLogoutTimeout));

    return NO_ERR;
}

const std::vector< std::string > & Account::getCharacterTypes(void) const
{
    return m_characterTypes;
}

const CharacterMap& Account::getCharacters()
{
    if (m_status != LOGGED_IN)
        error() << "Not logged into an account : getCharacter returning empty dictionary";

    return _characters;
}

Result Account::refreshCharacterInfo()
{
    if (!m_con->isConnected()) return NOT_CONNECTED;
    if (m_status != LOGGED_IN) return NOT_LOGGED_IN;

    // silently ignore overlapping refreshes
    if (m_doingCharacterRefresh) return NO_ERR;

    _characters.clear();

    if (m_characterIds.empty())
    {
        GotAllCharacters.emit(); // we must emit the done signal
        return NO_ERR;
    }

// okay, now we know we have at least one character to lookup, set the flag
    m_doingCharacterRefresh = true;

    Look lk;
    Anonymous obj;
    lk->setFrom(m_accountId);

    for (StringSet::iterator I=m_characterIds.begin(); I!=m_characterIds.end(); ++I)
    {
        obj->setId(*I);
        lk->setArgs1(obj);
        lk->setSerialno(getNewSerialno());
        m_con->getResponder()->await(lk->getSerialno(), this, &Account::sightCharacter);
        m_con->send(lk);
    }

    return NO_ERR;
}

Result Account::createCharacter(const Atlas::Objects::Entity::RootEntity &ent)
{
    if (!m_con->isConnected()) return NOT_CONNECTED;
    if (m_status != LOGGED_IN) {
        if ((m_status == CREATING_CHAR) || (m_status == TAKING_CHAR)) {
            error() << "duplicate char creation / take";
            return DUPLICATE_CHAR_ACTIVE;
        } else {
            error() << "called createCharacter on unconnected Account, ignoring";
            return NOT_LOGGED_IN;
        }
    }

    Create c;
    c->setArgs1(ent);
    c->setFrom(m_accountId);
    c->setSerialno(getNewSerialno());
    m_con->send(c);

    m_con->getResponder()->await(c->getSerialno(), this, &Account::avatarResponse);
    m_status = CREATING_CHAR;
    return NO_ERR;
}

/*
void Account::createCharacter()
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

void Account::createCharacterHandler(long serialno)
{
    if (serialno)
        NewCharacter((new World(this, _con))->createAvatar(serialno));
}
*/

Result Account::takeTransferredCharacter(const std::string &id, const std::string &key)
{
    if (!m_con->isConnected()) return NOT_CONNECTED;
    if (m_status != LOGGED_IN) {
        if ((m_status == CREATING_CHAR) || (m_status == TAKING_CHAR)) {
            error() << "duplicate char creation / take";
            return DUPLICATE_CHAR_ACTIVE;
        } else {
            error() << "called createCharacter on unconnected Account, ignoring";
            return NOT_LOGGED_IN;
        }
    }

    Anonymous what;
    what->setId(id);
    what->setAttr("possess_key", key);

    Look l;
    l->setFrom(getId());
    l->setArgs1(what);
    l->setSerialno(getNewSerialno());
    m_con->send(l);

    m_con->getResponder()->await(l->getSerialno(), this, &Account::avatarResponse);
    m_status = TAKING_CHAR;
    return NO_ERR;
}

Result Account::takeCharacter(const std::string &id)
{
    if (m_characterIds.count(id) == 0) {
        error() << "Character '" << id << "' not owned by Account " << m_username;
        return BAD_CHARACTER_ID;
    }

    if (!m_con->isConnected()) return NOT_CONNECTED;
    if (m_status != LOGGED_IN) {
        if ((m_status == CREATING_CHAR) || (m_status == TAKING_CHAR)) {
            error() << "duplicate char creation / take";
            return DUPLICATE_CHAR_ACTIVE;
        } else {
            error() << "called createCharacter on unconnected Account, ignoring";
            return NOT_LOGGED_IN;
        }
    }

    Anonymous what;
    what->setId(id);

    Look l;
    l->setFrom(id);  // should this be m_accountId?
    l->setArgs1(what);
    l->setSerialno(getNewSerialno());
    m_con->send(l);

    m_con->getResponder()->await(l->getSerialno(), this, &Account::avatarResponse);
    m_status = TAKING_CHAR;
    return NO_ERR;
}

Result Account::deactivateCharacter(Avatar* av)
{
    av->deactivate();
    return NO_ERR;
}

bool Account::isLoggedIn() const
{
    return ((m_status == LOGGED_IN) ||
        (m_status == TAKING_CHAR) || (m_status == CREATING_CHAR));
}

#pragma mark -

Result Account::internalLogin(const std::string &uname, const std::string &pwd)
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
    m_con->getResponder()->await(l->getSerialno(), this, &Account::loginResponse);
    m_con->send(l);

    m_timeout.reset(new Timeout(5000));
    m_timeout->Expired.connect(sigc::mem_fun(this, &Account::handleLoginTimeout));

    return NO_ERR;
}

void Account::logoutResponse(const RootOperation& op)
{
    if (!op->instanceOf(INFO_NO))
        warning() << "received a logout response that is not an INFO";

    internalLogout(true);
}

void Account::internalLogout(bool clean)
{
    if (clean) {
        if (m_status != LOGGING_OUT)
            error() << "got clean logout, but not logging out already";
    } else {
        if ((m_status != LOGGED_IN) && (m_status != TAKING_CHAR) && (m_status != CREATING_CHAR))
            error() << "got forced logout, but not currently logged in";
    }

    m_con->unregisterRouterForTo(m_router, m_accountId);
    m_status = DISCONNECTED;
    m_timeout.reset();

    if (m_con->getStatus() == BaseConnection::DISCONNECTING) {
        m_con->unlock();
    } else {
        LogoutComplete.emit(clean);
    }
}

void Account::loginResponse(const RootOperation& op)
{
    if (op->instanceOf(ERROR_NO)) {
        loginError(smart_dynamic_cast<Error>(op));
    } else if (op->instanceOf(INFO_NO)) {
        const std::vector<Root>& args = op->getArgs();
        loginComplete(smart_dynamic_cast<AtlasAccount>(args.front()));
    } else
        warning() << "received malformed login response: " << op->getClassNo();
}

void Account::loginComplete(const AtlasAccount &p)
{
    if (m_status != LOGGING_IN) {
        error() << "got loginComplete, but not currently logging in!";
    }

    if (!p.isValid()) {
        error() << "no account in response.";
        return;
    }

    //The user name being different should not be a fatal thing.
    if (p->getUsername() != m_username) {
        warning() << "received username does not match existing";
        m_username = p->getUsername();
    }

    m_status = LOGGED_IN;
    m_accountId = p->getId();

    m_con->registerRouterForTo(m_router, m_accountId);
    updateFromObject(p);

    // notify an people watching us
    LoginSuccess.emit();

    m_con->Disconnecting.connect(sigc::mem_fun(this, &Account::netDisconnecting));
    m_timeout.reset();
}

void Account::avatarLogoutRequested(Avatar* avatar)
{
    AvatarDeactivated.emit(avatar);
    delete avatar;
}

void Account::updateFromObject(const AtlasAccount &p)
{
    m_characterIds = StringSet(p->getCharacters().begin(), p->getCharacters().end());
    m_parents = p->getParents();

    if(p->hasAttr("character_types") == true)
    {
        const Atlas::Message::Element& CharacterTypes(p->getAttr("character_types"));

        if(CharacterTypes.isList() == true)
        {
            const Atlas::Message::ListType & CharacterTypesList(CharacterTypes.asList());
            Atlas::Message::ListType::const_iterator iCharacterType(CharacterTypesList.begin());
            Atlas::Message::ListType::const_iterator iEnd(CharacterTypesList.end());

            m_characterTypes.reserve(CharacterTypesList.size());
            while(iCharacterType != iEnd)
            {
                if(iCharacterType->isString() == true)
                {
                    m_characterTypes.push_back(iCharacterType->asString());
                }
                else
                {
                    error() << "An element of the \"character_types\" list is not a String.";
                }
                ++iCharacterType;
            }
        }
        else
        {
            error() << "Account has attribute \"character_types\" which is not of type List.";
        }
    }

    if (p->hasAttr("spawns")) {
        const Atlas::Message::Element& spawns(p->getAttr("spawns"));

        if (spawns.isList()) {
            const Atlas::Message::ListType & spawnsList(spawns.asList());
            for (Atlas::Message::ListType::const_iterator I = spawnsList.begin(); I != spawnsList.end(); ++I) {
                if (I->isMap()) {
                    const Atlas::Message::MapType& spawnMap = I->asMap();
                    Atlas::Message::MapType::const_iterator spawnNameI = spawnMap.find("name");
                    if (spawnNameI != spawnMap.end()) {
                        const Atlas::Message::Element& name(spawnNameI->second);
                        if (name.isString()) {
                            CharacterTypeStore characterTypes;
                            Atlas::Message::MapType::const_iterator characterTypesI = spawnMap.find("character_types");

                            if (characterTypesI != spawnMap.end()) {
                                const Atlas::Message::Element& characterTypesElement(characterTypesI->second);
                                if (characterTypesElement.isList()) {
                                    const Atlas::Message::ListType & characterTypesList(characterTypesElement.asList());
                                    for (Atlas::Message::ListType::const_iterator J = characterTypesList.begin(); J != characterTypesList.end(); ++J)
                                    {
                                        if (J->isString()) {
                                            characterTypes.push_back(CharacterType(J->asString(), ""));
                                        } else {
                                            error() << "Character type is not of type string.";
                                        }
                                    }
                                } else {
                                    error() << "Character type element is not of type list.";
                                }
                            }
                            SpawnPoint spawnPoint(name.asString(), characterTypes, "");
                            m_spawnPoints.insert(SpawnPointMap::value_type(spawnPoint.getName(), spawnPoint));
                        } else {
                            error() << "Spawn name is not a string.";
                        }
                    } else {
                        error() << "Spawn point has no name defined.";
                    }
                }
            }
        } else {
            error() << "Account has attribute \"spawns\" which is not of type List.";
        }
    }
}

std::string getErrorMessage(const RootOperation & err)
{
    std::string msg;
    const std::vector<Root>& args = err->getArgs();
    if (args.empty()) {
        error() << "got Error error op from server without args";
        msg = "Unknown error.";
    } else {
        const Root & arg = args.front();
        Atlas::Message::Element message;
        if (arg->copyAttr("message", message) != 0) {
            error() << "got Error error op from server without message";
            msg = "Unknown error.";
        } else {
            if (!message.isString()) {
                error() << "got Error error op from server with bad message";
                msg = "Unknown error.";
            } else {
                msg = message.String();
            }
        }
    }
    return msg;
}

void Account::loginError(const Error& err)
{
    assert(err.isValid());
    if (m_status != LOGGING_IN) {
        error() << "got loginError while not logging in";
    }

    std::string msg = getErrorMessage(err);

    // update state before emitting signal
    m_status = DISCONNECTED;
    m_timeout.reset();

    LoginFailure.emit(msg);
}

void Account::handleLoginTimeout()
{
    m_status = DISCONNECTED;
    deleteLater(m_timeout.release());

    LoginFailure.emit("timed out waiting for server response");
}

void Account::avatarResponse(const RootOperation& op)
{
    if (op->instanceOf(ERROR_NO)) {
        std::string msg = getErrorMessage(op);

        // creating or taking a character failed for some reason
        AvatarFailure(msg);
        m_status = Account::LOGGED_IN;
    } else if (op->instanceOf(INFO_NO)) {
        const std::vector<Root>& args = op->getArgs();
        if (args.empty()) {
            warning() << "no args character create/take response";
            return;
        }

        RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
        if (!ent.isValid()) {
            warning() << "malformed character create/take response";
            return;
        }

        Avatar* av = new Avatar(*this, ent->getId());
        AvatarSuccess.emit(av);
        m_status = Account::LOGGED_IN;

        assert(m_activeCharacters.count(av->getId()) == 0);
        m_activeCharacters[av->getId()] = av;

        // expect another op with the same refno
        m_con->getResponder()->ignore(op->getRefno());
    } else
        warning() << "received incorrect avatar create/take response";
}

void Account::internalDeactivateCharacter(Avatar* av)
{
    assert(m_activeCharacters.count(av->getId()) == 1);
    m_activeCharacters.erase(av->getId());
}

void Account::sightCharacter(const RootOperation& op)
{
    if (!m_doingCharacterRefresh) {
        error() << "got sight of character outside a refresh, ignoring";
        return;
    }

    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        error() << "got sight of character with no args";
        return;
    }

    RootEntity ge = smart_dynamic_cast<RootEntity>(args.front());
    if (!ge.isValid()) {
        error() << "got sight of character with malformed args";
        return;
    }

    CharacterMap::iterator C = _characters.find(ge->getId());
    if (C != _characters.end()) {
        error() << "duplicate sight of character " << ge->getId();
        return;
    }

    // okay, we can now add it to our map
    _characters.insert(C, CharacterMap::value_type(ge->getId(), ge));
    GotCharacterInfo.emit(ge);

    // check if we're done
    if (_characters.size() == m_characterIds.size()) {
        m_doingCharacterRefresh = false;
        GotAllCharacters.emit();
    }
}

/* this will only ever get encountered after the connection is initally up;
thus we use it to trigger a reconnection. Note that some actions by
Lobby and World are also required to get everything back into the correct
state */

void Account::netConnected()
{
    // re-connection
    if (!m_username.empty() && !m_pass.empty() && (m_status == DISCONNECTED)) {
        debug() << "Account " << m_username << " got netConnected, doing reconnect";
        internalLogin(m_username, m_pass);
    }
}

bool Account::netDisconnecting()
{
    if (m_status == LOGGED_IN) {
        m_con->lock();
        logout();
        return false;
    } else
        return true;
}

void Account::netFailure(const std::string& /*msg*/)
{

}

void Account::handleLogoutTimeout()
{
    error() << "LOGOUT timed out waiting for response";

    m_status = DISCONNECTED;
    deleteLater(m_timeout.release());

    LogoutComplete.emit(false);
}

void Account::avatarLogoutResponse(const RootOperation& op)
{
    if (!op->instanceOf(INFO_NO)) {
        warning() << "received an avatar logout response that is not an INFO";
        return;
    }

    const std::vector<Root>& args(op->getArgs());

    if (args.empty() || (args.front()->getClassNo() != LOGOUT_NO)) {
        warning() << "argument of avatar logout INFO is not a logout op";
        return;
    }

    RootOperation logout = smart_dynamic_cast<RootOperation>(args.front());
    const std::vector<Root>& args2(logout->getArgs());
    if (args2.empty()) {
        warning() << "argument of avatar logout INFO is logout without args";
        return;
    }

    std::string charId = args2.front()->getId();
    debug() << "got logout for character " << charId;

    if (!m_characterIds.count(charId)) {
        warning() << "character ID " << charId << " is unknown on account " << m_accountId;
    }

    ActiveCharacterMap::iterator it = m_activeCharacters.find(charId);
    if (it == m_activeCharacters.end()) {
        warning() << "character ID " << charId << " does not correspond to an active avatar.";
        return;
    }

    AvatarDeactivated.emit(it->second);
    delete it->second; // will call back into internalDeactivateCharacter
}

} // of namespace Eris
