#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Account.h"
#include "Connection.h"
#include "LogStream.h"
#include "Exceptions.h"
#include "Avatar.h"
#include "Router.h"
#include "Response.h"
#include "EventService.h"
#include "SpawnPoint.h"
#include "CharacterType.h"
#include "TypeService.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/objectFactory.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>

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
    explicit AccountRouter(Account* pl) :
        m_account(pl)
    {
        m_account->getConnection()->setDefaultRouter(this);
    }

    ~AccountRouter() override {
        m_account->getConnection()->clearDefaultRouter();
    }

    RouterResult handleOperation(const RootOperation& op) override {
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

        if (op->getParent() == "change") {
            m_account->getConnection()->getTypeService()->handleOperation(op);
            return HANDLED;
        }

        if (op->getClassNo() == ERROR_NO) {
            auto message = getErrorMessage(op);
            if (!message.empty()) {
                notice() << "Got error message from server: " << message;
                m_account->ErrorMessage.emit(message);
            }

            return HANDLED;
        }

        return IGNORED;
    }

private:
    Account* m_account;
};

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
        auto cur = it++;
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
    // store for re-logins
    m_username = uname;
    m_pass = pwd;

    //We should clarify that we want a player account.
    Atlas::Objects::Entity::Player account;
    account->setPassword(pwd);
    account->setName(fullName);
    account->setUsername(uname);

    return createAccount(account);
}

Result Account::createAccount(const Atlas::Objects::Entity::Account& accountOp)
{
    if (!m_con->isConnected()) return NOT_CONNECTED;
    if (m_status != DISCONNECTED) return ALREADY_LOGGED_IN;

    m_status = LOGGING_IN;

    Create c;
    c->setSerialno(getNewSerialno());
    c->setArgs1(accountOp);

    m_con->getResponder()->await(c->getSerialno(), this, &Account::loginResponse);
    m_con->send(c);

    m_timeout.reset(new TimedEvent(m_con->getEventService(), boost::posix_time::seconds(5), [&](){this->handleLoginTimeout();}));

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

    m_timeout.reset(new TimedEvent(m_con->getEventService(), boost::posix_time::seconds(5), [&](){this->handleLogoutTimeout();}));

    return NO_ERR;
}

const std::vector< std::string > & Account::getCharacterTypes() const
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

    for (const auto& id : m_characterIds) {
        obj->setId(id);
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

    //Make sure we also possess the newly created character; we don't want to support non-possessed characters in the client for now.
    ent->setAttr("possess", 1);
    Create c;
    c->setArgs1(ent);
    c->setFrom(m_accountId);
    c->setSerialno(getNewSerialno());
    m_con->send(c);

    //First response should be a Sight of the newly created character, followed by an Info of the Mind created.
    m_con->getResponder()->await(c->getSerialno(), this, &Account::avatarCreateResponse);
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

    Atlas::Objects::Operation::Generic possessOp;
    possessOp->setParent("possess");
    possessOp->setFrom(m_accountId);
    possessOp->setArgs1(what);
    possessOp->setSerialno(getNewSerialno());
    m_con->send(possessOp);

    m_con->getResponder()->await(possessOp->getSerialno(), this, &Account::possessResponse);
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

    Atlas::Objects::Operation::Generic possessOp;
    possessOp->setParent("possess");
    possessOp->setFrom(m_accountId);
    possessOp->setArgs1(what);
    possessOp->setSerialno(getNewSerialno());
    m_con->send(possessOp);

    m_con->getResponder()->await(possessOp->getSerialno(), this, &Account::possessResponse);
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

    m_timeout.reset(new TimedEvent(m_con->getEventService(), boost::posix_time::seconds(5), [&](){this->handleLoginTimeout();}));

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

void Account::destroyAvatar(const std::string& avatarId)
{
    auto I = m_activeCharacters.find(avatarId);
    //The avatar might have already have been deleted by another response; this is a normal case.
    if (I != m_activeCharacters.end()) {
        AvatarDeactivated(I->second);
        delete I->second;
        m_activeCharacters.erase(I);
    }
}

void Account::updateFromObject(const AtlasAccount &p)
{
    m_characterIds = StringSet(p->getCharacters().begin(), p->getCharacters().end());
    m_parent = p->getParent();

    if(p->hasAttr("character_types"))
    {
        const Atlas::Message::Element& CharacterTypes(p->getAttr("character_types"));

        if(CharacterTypes.isList())
        {
            const Atlas::Message::ListType & CharacterTypesList(CharacterTypes.asList());
            auto iCharacterType(CharacterTypesList.begin());
            auto iEnd(CharacterTypesList.end());

            m_characterTypes.reserve(CharacterTypesList.size());
            while(iCharacterType != iEnd)
            {
                if(iCharacterType->isString())
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
            m_spawnPoints.clear();
            const Atlas::Message::ListType & spawnsList(spawns.asList());
            for (const auto& spawnElement : spawnsList) {
                if (spawnElement.isMap()) {
                    const Atlas::Message::MapType& spawnMap = spawnElement.asMap();
                    auto spawnNameI = spawnMap.find("name");
                    if (spawnNameI != spawnMap.end()) {
                        const Atlas::Message::Element& name(spawnNameI->second);
                        if (name.isString()) {
                            CharacterTypeStore characterTypes;
                            auto characterTypesI = spawnMap.find("character_types");

                            if (characterTypesI != spawnMap.end()) {
                                const Atlas::Message::Element& characterTypesElement(characterTypesI->second);
                                if (characterTypesElement.isList()) {
                                    const Atlas::Message::ListType & characterTypesList(characterTypesElement.asList());
                                    for (const auto& characterElement : characterTypesList) {
                                        if (characterElement.isString()) {
                                            characterTypes.push_back(CharacterType(characterElement.asString(), ""));
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
    m_timeout.release();

    LoginFailure.emit("timed out waiting for server response");
}

void Account::possessResponse(const RootOperation& op)
{
    if (op->instanceOf(ERROR_NO)) {
        std::string msg = getErrorMessage(op);

        // creating or taking a character failed for some reason
        AvatarFailure(msg);
        m_status = Account::LOGGED_IN;
    } else if (op->instanceOf(INFO_NO)) {
        const std::vector<Root>& args = op->getArgs();
        if (args.empty()) {
            warning() << "no args character possess response";
            return;
        }

        auto ent = smart_dynamic_cast<RootEntity>(args.front());
        if (!ent.isValid()) {
            warning() << "malformed character possess response";
            return;
        }

        if (!ent->hasAttr("entity")) {
            warning() << "malformed character possess response";
            return;
        }

        auto entityMessage = ent->getAttr("entity");

        if (!entityMessage.isMap()) {
            warning() << "malformed character possess response";
            return;
        }
        auto entityObj = smart_dynamic_cast<RootEntity>(Atlas::Objects::Factories::instance()->createObject(entityMessage.Map()));

        if (!entityObj || entityObj->isDefaultId()) {
            warning() << "malformed character possess response";
            return;
        }

        auto av = new Avatar(*this, ent->getId(), entityObj->getId());
        AvatarSuccess.emit(av);
        m_status = Account::LOGGED_IN;

        assert(m_activeCharacters.count(av->getId()) == 0);
        m_activeCharacters[av->getId()] = av;

    } else
        warning() << "received incorrect avatar create/take response";
}

void Account::avatarCreateResponse(const RootOperation& op)
{
    //When creating a character the first op is a Sight of the new entity,
    //followed by an INFO about the Mind setup to control the entity.
    if (op->instanceOf(ERROR_NO)) {
        std::string msg = getErrorMessage(op);

        // creating or taking a character failed for some reason
        AvatarFailure(msg);
        m_status = Account::LOGGED_IN;
    } else if (op->instanceOf(SIGHT_NO)) {
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


        _characters.emplace(ge->getId(), ge);
        GotCharacterInfo.emit(ge);
        // expect another op with the same refno
        m_con->getResponder()->ignore(op->getRefno());
    } else if (op->instanceOf(INFO_NO)) {
        possessResponse(op);
    } else {
        warning() << "received incorrect avatar create/take response";
    }
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

    auto C = _characters.find(ge->getId());
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
    m_timeout.release();

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

    auto it = m_activeCharacters.find(charId);
    if (it == m_activeCharacters.end()) {
        warning() << "character ID " << charId << " does not correspond to an active avatar.";
        return;
    }

    destroyAvatar(charId);
}

} // of namespace Eris
