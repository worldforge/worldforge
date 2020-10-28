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
#include "TypeService.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>

using Atlas::Objects::Root;
using Atlas::Message::Element;
using namespace Atlas::Objects::Operation;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
typedef Atlas::Objects::Entity::Account AtlasAccount;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

class AccountRouter : public Router {
public:
	explicit AccountRouter(Account* pl) :
			m_account(pl) {
		m_account->getConnection().setDefaultRouter(this);
	}

	~AccountRouter() override {
		m_account->getConnection().clearDefaultRouter();
	}

	RouterResult handleOperation(const RootOperation& op) override {
		// logout
		if (op->getClassNo() == LOGOUT_NO) {
			debug() << "Account received forced logout from server";
			m_account->internalLogout(false);
			return HANDLED;
		}

		if ((op->getClassNo() == SIGHT_NO) && (op->getTo() == m_account->getId())) {
			const std::vector<Root>& args = op->getArgs();
			AtlasAccount acc = smart_dynamic_cast<AtlasAccount>(args.front());
			m_account->updateFromObject(acc);

			// refresh character data if it changed
			if (!acc->isDefaultCharacters()) {
				m_account->refreshCharacterInfo();
			}

			return HANDLED;
		}

		if (op->getClassNo() == CHANGE_NO) {
			m_account->getConnection().getTypeService().handleOperation(op);
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

Account::Account(Connection& con) :
		m_con(con),
		m_status(Status::DISCONNECTED),
		m_router(std::make_unique<AccountRouter>(this)),
		m_doingCharacterRefresh(false) {
	m_con.Connected.connect(sigc::mem_fun(this, &Account::netConnected));
	m_con.Failure.connect(sigc::mem_fun(this, &Account::netFailure));
}

Account::~Account() {
	ActiveCharacterMap::iterator it;
	for (it = m_activeAvatars.begin(); it != m_activeAvatars.end();) {
		auto cur = it++;
		deactivateCharacter(cur->second.get()); // send logout op
	}
	//Don't wait for logout response, delete all avatars now.
	while (!m_activeAvatars.empty()) {
		destroyAvatar(m_activeAvatars.begin()->first);
	}

	if (isLoggedIn()) {
		logout();
	}
}

Result Account::login(const std::string& uname, const std::string& password) {
	if (!m_con.isConnected()) {
		error() << "called login on unconnected Connection";
		return NOT_CONNECTED;
	}

	if (m_status != Status::DISCONNECTED) {
		error() << "called login, but state is not currently disconnected";
		return ALREADY_LOGGED_IN;
	}

	return internalLogin(uname, password);
}

Result Account::createAccount(const std::string& uname,
							  const std::string& fullName,
							  const std::string& pwd) {
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

Result Account::createAccount(const Atlas::Objects::Entity::Account& accountOp) {
	if (!m_con.isConnected()) return NOT_CONNECTED;
	if (m_status != Status::DISCONNECTED) return ALREADY_LOGGED_IN;

	m_status = Status::LOGGING_IN;

	Create c;
	c->setSerialno(getNewSerialno());
	c->setArgs1(accountOp);

	m_con.getResponder().await(c->getSerialno(), this, &Account::loginResponse);
	m_con.send(c);

	m_timeout = std::make_unique<TimedEvent>(m_con.getEventService(), std::chrono::seconds(5),
											 [&]() { this->handleLoginTimeout(); });

	return NO_ERR;
}

Result Account::logout() {
	if (!m_con.isConnected()) {
		error() << "called logout on bad connection ignoring";
		return NOT_CONNECTED;
	}

	if (m_status == Status::LOGGING_OUT) return NO_ERR;

	if (m_status != Status::LOGGED_IN) {
		error() << "called logout on non-logged-in Account";
		return NOT_LOGGED_IN;
	}

	m_status = Status::LOGGING_OUT;

	//Send a Logout to the connection, with the Account as entity reference.
	Logout l;
	Anonymous arg;
	arg->setId(m_accountId);
	l->setArgs1(arg);
	l->setSerialno(getNewSerialno());

	m_con.getResponder().await(l->getSerialno(), this, &Account::logoutResponse);
	m_con.send(l);

	m_timeout = std::make_unique<TimedEvent>(m_con.getEventService(), std::chrono::seconds(5),
											 [&]() { this->handleLogoutTimeout(); });

	return NO_ERR;
}

const CharacterMap& Account::getCharacters() {
	if (m_status != Status::LOGGED_IN)
		error() << "Not logged into an account : getCharacter returning empty dictionary";

	return _characters;
}

Result Account::refreshCharacterInfo() {
	if (!m_con.isConnected()) return NOT_CONNECTED;
	if (m_status != Status::LOGGED_IN) {
		return NOT_LOGGED_IN;
	}

	// silently ignore overlapping refreshes
	if (m_doingCharacterRefresh) return NO_ERR;

	_characters.clear();

	if (m_characterIds.empty()) {
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
		m_con.getResponder().await(lk->getSerialno(), this, &Account::sightCharacter);
		m_con.send(lk);
	}

	return NO_ERR;
}

Result Account::createCharacterThroughEntity(const Atlas::Objects::Entity::RootEntity& ent) {
	//Make sure we also possess the newly created character; we don't want to support non-possessed characters in the client for now.
	ent->setAttr("__account", m_username);
	Create c;
	c->setArgs1(ent);
	c->setFrom(m_accountId);
	c->setTo(m_accountId);

	return createCharacterThroughOperation(c);
}

Result Account::createCharacterThroughOperation(const Create& c) {
	if (!m_con.isConnected()) return NOT_CONNECTED;
	if (m_status != Status::LOGGED_IN) {
		if ((m_status == Status::CREATING_CHAR) || (m_status == Status::TAKING_CHAR)) {
			error() << "duplicate char creation / take";
			return DUPLICATE_CHAR_ACTIVE;
		} else {
			error() << "called createCharacter on unconnected Account, ignoring";
			return NOT_LOGGED_IN;
		}
	}

	c->setSerialno(getNewSerialno());
	m_con.send(c);

	//First response should be a Sight of the newly created character, followed by an Info of the Mind created.
	m_con.getResponder().await(c->getSerialno(), this, &Account::avatarCreateResponse);
	m_status = Status::CREATING_CHAR;
	return NO_ERR;
}

Result Account::takeTransferredCharacter(const std::string& id, const std::string& key) {
	if (!m_con.isConnected()) return NOT_CONNECTED;
	if (m_status != Status::LOGGED_IN) {
		if ((m_status == Status::CREATING_CHAR) || (m_status == Status::TAKING_CHAR)) {
			error() << "duplicate char creation / take";
			return DUPLICATE_CHAR_ACTIVE;
		} else {
			error() << "called takeCharacter on unconnected Account, ignoring";
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
	m_con.send(possessOp);

	m_con.getResponder().await(possessOp->getSerialno(), this, &Account::possessResponse);
	m_status = Status::TAKING_CHAR;
	return NO_ERR;
}

Result Account::takeCharacter(const std::string& id) {

	if (!m_con.isConnected()) return NOT_CONNECTED;
	if (m_status != Status::LOGGED_IN && m_status != Status::CREATED_CHAR) {
		if ((m_status == Status::CREATING_CHAR) || (m_status == Status::TAKING_CHAR)) {
			error() << "duplicate char creation / take";
			return DUPLICATE_CHAR_ACTIVE;
		} else {
			error() << "called takeCharacter on unconnected Account, ignoring";
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
	m_con.send(possessOp);

	m_con.getResponder().await(possessOp->getSerialno(), this, &Account::possessResponse);
	m_status = Status::TAKING_CHAR;
	return NO_ERR;
}

Result Account::deactivateCharacter(Avatar* av) {
	av->deactivate();
	return NO_ERR;
}

bool Account::isLoggedIn() const {
	return ((m_status == Status::LOGGED_IN) ||
			(m_status == Status::TAKING_CHAR) || (m_status == Status::CREATING_CHAR));
}

Result Account::internalLogin(const std::string& uname, const std::string& pwd) {
	assert(m_status == Status::DISCONNECTED);

	m_status = Status::LOGGING_IN;
	m_username = uname;

	AtlasAccount account;
	account->setPassword(pwd);
	account->setUsername(uname);

	Login l;
	l->setArgs1(account);
	l->setSerialno(getNewSerialno());
	m_con.getResponder().await(l->getSerialno(), this, &Account::loginResponse);
	m_con.send(l);

	m_timeout = std::make_unique<TimedEvent>(m_con.getEventService(), std::chrono::seconds(5),
											 [&]() { this->handleLoginTimeout(); });

	return NO_ERR;
}

void Account::logoutResponse(const RootOperation& op) {
	if (!op->instanceOf(INFO_NO))
		warning() << "received a logout response that is not an INFO";

	internalLogout(true);
}

void Account::internalLogout(bool clean) {
	if (clean) {
		if (m_status != Status::LOGGING_OUT)
			error() << "got clean logout, but not logging out already";
	} else {
		if ((m_status != Status::LOGGED_IN) && (m_status != Status::TAKING_CHAR) &&
			(m_status != Status::CREATING_CHAR))
			error() << "got forced logout, but not currently logged in";
	}

	m_con.unregisterRouterForTo(m_router.get(), m_accountId);
	m_status = Status::DISCONNECTED;
	m_timeout.reset();

	if (m_con.getStatus() == BaseConnection::DISCONNECTING) {
		m_con.unlock();
	} else {
		LogoutComplete.emit(clean);
	}
}

void Account::loginResponse(const RootOperation& op) {
	if (op->instanceOf(ERROR_NO)) {
		loginError(smart_dynamic_cast<Error>(op));
	} else if (op->instanceOf(INFO_NO)) {
		const std::vector<Root>& args = op->getArgs();
		loginComplete(smart_dynamic_cast<AtlasAccount>(args.front()));
	} else
		warning() << "received malformed login response: " << op->getClassNo();
}

void Account::loginComplete(const AtlasAccount& p) {
	if (m_status != Status::LOGGING_IN) {
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

	m_status = Status::LOGGED_IN;
	m_accountId = p->getId();

	m_con.registerRouterForTo(m_router.get(), m_accountId);
	updateFromObject(p);

	// notify an people watching us
	LoginSuccess.emit();

	m_con.Disconnecting.connect(sigc::mem_fun(this, &Account::netDisconnecting));
	m_timeout.reset();
}

void Account::avatarLogoutRequested(Avatar* avatar) {
	destroyAvatar(avatar->getId());
}

void Account::destroyAvatar(const std::string& avatarId) {
	auto I = m_activeAvatars.find(avatarId);
	//The avatar might have already have been deleted by another response; this is a normal case.
	if (I != m_activeAvatars.end()) {
		m_activeAvatars.erase(I);
		AvatarDeactivated(avatarId);
	}
}

void Account::updateFromObject(const AtlasAccount& p) {
	auto characters = std::set<std::string>(p->getCharacters().begin(), p->getCharacters().end());
	std::string createdCharacterId;
	if (m_status == Status::CREATING_CHAR) {
		//See which character was added
		for (auto& character : characters) {
			if (m_characterIds.find(character) == m_characterIds.end()) {
				createdCharacterId = character;
				break;
			}
		}
	}
	m_characterIds = std::move(characters);
	m_parent = p->getParent();

	if (p->hasAttr("spawns")) {
		m_spawnPoints.clear();
		const auto& spawns = p->getAttr("spawns");

		if (spawns.isList()) {
			auto& spawnsList = spawns.List();
			for (const auto& spawnElement : spawnsList) {
				if (spawnElement.isMap()) {
					auto& spawnMap = spawnElement.Map();
					std::string name;
					std::string description;
					std::string id;
					std::vector<SpawnProperty> properties;
					{
						auto I = spawnMap.find("name");
						if (I != spawnMap.end() && I->second.isString()) {
							name = I->second.String();
						}
					}
					{
						auto I = spawnMap.find("description");
						if (I != spawnMap.end() && I->second.isString()) {
							description = I->second.String();
						}
					}
					{
						auto I = spawnMap.find("id");
						if (I != spawnMap.end() && I->second.isString()) {
							id = I->second.String();
						}
					}
					{
						auto I = spawnMap.find("properties");
						if (I != spawnMap.end() && I->second.isList()) {
							auto propList = I->second.List();
							for (auto& entry : propList) {
								if (entry.isMap()) {
									auto& entryMap = entry.Map();
									std::string propName;
									std::string propLabel;
									std::string propDescription;
									SpawnProperty::Type propType = SpawnProperty::Type::STRING;
									std::vector<Atlas::Message::Element> propOptions;

									{
										auto J = entryMap.find("name");
										if (J != entryMap.end() && J->second.isString()) {
											propName = J->second.String();
										}
									}
									{
										auto J = entryMap.find("label");
										if (J != entryMap.end() && J->second.isString()) {
											propLabel = J->second.String();
										}
									}
									{
										auto J = entryMap.find("description");
										if (J != entryMap.end() && J->second.isString()) {
											propDescription = J->second.String();
										}
									}
									{
										auto J = entryMap.find("type");
										if (J != entryMap.end() && J->second.isString()) {
											auto& type = J->second.String();
											if (type == "string") {
												propType = SpawnProperty::Type::STRING;
											}
										}
									}
									{
										auto J = entryMap.find("options");
										if (J != entryMap.end() && J->second.isList()) {
											propOptions = J->second.List();
										}
									}
									properties.emplace_back(SpawnProperty{propName, propLabel, propDescription, propType, propOptions});
								}

							}
						}
					}
					m_spawnPoints.emplace_back(SpawnPoint{id, name, description, properties});
				}
			}
		} else {
			error() << "Account has attribute \"spawns\" which is not of type List.";
		}
	}
	if (m_status == Status::CREATING_CHAR && !createdCharacterId.empty()) {
		m_status = Status::CREATED_CHAR;
		takeCharacter(createdCharacterId);
	}
}

void Account::loginError(const Error& err) {
	if (!err) {
		warning() << "Got invalid error";
		return;
	}
	if (m_status != Status::LOGGING_IN) {
		error() << "got loginError while not logging in";
	}

	std::string msg = getErrorMessage(err);

	// update state before emitting signal
	m_status = Status::DISCONNECTED;
	m_timeout.reset();

	LoginFailure.emit(msg);
}

void Account::handleLoginTimeout() {
	m_status = Status::DISCONNECTED;
	m_timeout.reset();

	LoginFailure.emit("timed out waiting for server response");
}

void Account::possessResponse(const RootOperation& op) {
	if (op->instanceOf(ERROR_NO)) {
		std::string msg = getErrorMessage(op);

		// creating or taking a character failed for some reason
		AvatarFailure(msg);
		m_status = Account::Status::LOGGED_IN;
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
		auto entityObj = smart_dynamic_cast<RootEntity>(m_con.getFactories().createObject(entityMessage.Map()));

		if (!entityObj || entityObj->isDefaultId()) {
			warning() << "malformed character possess response";
			return;
		}

		if (m_activeAvatars.find(ent->getId()) != m_activeAvatars.end()) {
			warning() << "got possession response for character already created";
			return;
		}

		auto av = std::make_unique<Avatar>(*this, ent->getId(), entityObj->getId());
		AvatarSuccess.emit(av.get());
		m_status = Account::Status::LOGGED_IN;

		m_activeAvatars[av->getId()] = std::move(av);

	} else
		warning() << "received incorrect avatar create/take response";
}

void Account::avatarCreateResponse(const RootOperation& op) {
	if (op->instanceOf(ERROR_NO)) {
		std::string msg = getErrorMessage(op);

		// creating or taking a character failed for some reason
		AvatarFailure(msg);
		m_status = Account::Status::LOGGED_IN;
	}
}

void Account::internalDeactivateCharacter(Avatar* av) {
	if (m_activeAvatars.find(av->getId()) == m_activeAvatars.end()) {
		warning() << "trying to deactivate non active character";
	}
	m_activeAvatars.erase(av->getId());
}

void Account::sightCharacter(const RootOperation& op) {
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

void Account::netConnected() {
	// re-connection
	if (!m_username.empty() && !m_pass.empty() && (m_status == Status::DISCONNECTED)) {
		debug() << "Account " << m_username << " got netConnected, doing reconnect";
		internalLogin(m_username, m_pass);
	}
}

bool Account::netDisconnecting() {
	if (m_status == Status::LOGGED_IN) {
		m_con.lock();
		logout();
		return false;
	} else
		return true;
}

void Account::netFailure(const std::string& /*msg*/) {

}

void Account::handleLogoutTimeout() {
	error() << "LOGOUT timed out waiting for response";

	m_status = Status::DISCONNECTED;
	m_timeout.reset();

	LogoutComplete.emit(false);
}

void Account::avatarLogoutResponse(const RootOperation& op) {
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

	if (m_characterIds.find(charId) == m_characterIds.end()) {
		warning() << "character ID " << charId << " is unknown on account " << m_accountId;
	}

	auto it = m_activeAvatars.find(charId);
	if (it == m_activeAvatars.end()) {
		warning() << "character ID " << charId << " does not correspond to an active avatar.";
		return;
	}

	destroyAvatar(charId);
}

} // of namespace Eris
