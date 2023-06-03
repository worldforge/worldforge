// Eris Online RPG Protocol Library
// Copyright (C) 2007 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Eris/Account.h>
#include "Eris/EntityRouter.h"
#include <Eris/Avatar.h>
#include <Eris/Connection.h>
#include <Eris/IGRouter.h>
#include <Eris/Response.h>
#include <Eris/EventService.h>
#include <Eris/WaitFreeQueue.h>
#include <Eris/View.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>

#include "SignalFlagger.h"

using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;

static bool stub_type_bound = false;

static sigc::signal<void()> _test_avatar_logoutRequested;
static sigc::signal<void()> _test_avatar_logoutWithTransferRequested;

struct TestAvatar : public Eris::Avatar {
	explicit TestAvatar(Eris::Account& account) :
			Eris::Avatar(
					account,
					"",
					"") {}
};

struct TestIGRouter : public Eris::IGRouter {

	explicit TestIGRouter(Eris::Avatar& av) : Eris::IGRouter(av, av.getView()) {}

	RouterResult test_handleOperation(const RootOperation& op) {
		return this->handleOperation(op);
	}
};

int main() {
	boost::asio::io_service io_service;
	Eris::EventService event_service(io_service);
	Eris::Connection connection(io_service, event_service, "", "", 0);
	Eris::Account account(connection);
	
	{
		TestAvatar av(account);
		Eris::IGRouter ir(av, av.getView());
	}

	{
		TestAvatar av(account);
		Eris::IGRouter ir(av, av.getView());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		RootOperation op;
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::IGNORED);
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		RootOperation op;
		op->setSeconds(0);
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::IGNORED);
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		Atlas::Objects::Operation::Sight op;
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::IGNORED);
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		Atlas::Objects::Operation::Sight op;
		op->setArgs1(Root());
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		Atlas::Objects::Operation::Sight op;
		op->setArgs1(RootEntity());
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		stub_type_bound = true;

		Atlas::Objects::Operation::Sight op;
		op->setArgs1(RootEntity());
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);

		stub_type_bound = false;
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		Atlas::Objects::Operation::Sight op;
		op->setArgs1(RootOperation());
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		Atlas::Objects::Operation::Appearance op;
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		Atlas::Objects::Operation::Disappearance op;
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		Atlas::Objects::Operation::Unseen op;
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::IGNORED);
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		Atlas::Objects::Operation::Unseen op;
		op->setArgs1(Root());
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(!transferRequested.flagged());
		assert(logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		op->setArgs1(Root());
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(!transferRequested.flagged());
		assert(logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		op->modifyArgs().push_back(Root());
		op->modifyArgs().push_back(Root());
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(!transferRequested.flagged());
		assert(logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		Root arg1;
		Root arg2;
		arg2->setAttr("teleport_host", 1);
		op->modifyArgs().push_back(arg1);
		op->modifyArgs().push_back(arg2);
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(!transferRequested.flagged());
		assert(logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		Root arg1;
		Root arg2;
		arg2->setAttr("teleport_host", "ec66b165-9814-44d4-8326-ba9f31ce3224");
		op->modifyArgs().push_back(arg1);
		op->modifyArgs().push_back(arg2);
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(!transferRequested.flagged());
		assert(logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		Root arg1;
		Root arg2;
		arg2->setAttr("teleport_host", "ec66b165-9814-44d4-8326-ba9f31ce3224");
		arg2->setAttr("teleport_port", "");
		op->modifyArgs().push_back(arg1);
		op->modifyArgs().push_back(arg2);
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(!transferRequested.flagged());
		assert(logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		Root arg1;
		Root arg2;
		arg2->setAttr("teleport_host", "ec66b165-9814-44d4-8326-ba9f31ce3224");
		arg2->setAttr("teleport_port", 0xebc);
		op->modifyArgs().push_back(arg1);
		op->modifyArgs().push_back(arg2);
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(!transferRequested.flagged());
		assert(logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		Root arg1;
		Root arg2;
		arg2->setAttr("teleport_host", "ec66b165-9814-44d4-8326-ba9f31ce3224");
		arg2->setAttr("teleport_port", 0xebc);
		arg2->setAttr("possess_key", 0x139);
		op->modifyArgs().push_back(arg1);
		op->modifyArgs().push_back(arg2);
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(!transferRequested.flagged());
		assert(logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		Root arg1;
		Root arg2;
		arg2->setAttr("teleport_host", "ec66b165-9814-44d4-8326-ba9f31ce3224");
		arg2->setAttr("teleport_port", 0xebc);
		arg2->setAttr("possess_key", "2f281182-f285-48d7-812a-4f706954aa56");
		op->modifyArgs().push_back(arg1);
		op->modifyArgs().push_back(arg2);
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(!transferRequested.flagged());
		assert(logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		Root arg1;
		Root arg2;
		arg2->setAttr("teleport_host", "ec66b165-9814-44d4-8326-ba9f31ce3224");
		arg2->setAttr("teleport_port", 0xebc);
		arg2->setAttr("possess_key", "2f281182-f285-48d7-812a-4f706954aa56");
		arg2->setAttr("possess_entity_id", 0xa56);
		op->modifyArgs().push_back(arg1);
		op->modifyArgs().push_back(arg2);
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(!transferRequested.flagged());
		assert(logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		Root arg1;
		Root arg2;
		arg2->setAttr("teleport_host", "ec66b165-9814-44d4-8326-ba9f31ce3224");
		arg2->setAttr("teleport_port", 0xebc);
		arg2->setAttr("possess_key", "2f281182-f285-48d7-812a-4f706954aa56");
		arg2->setAttr("possess_entity_id", "1dab48d5-8784-4cfb-b1a2-e801fa99fc3a");
		op->modifyArgs().push_back(arg1);
		op->modifyArgs().push_back(arg2);
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(transferRequested.flagged());
		assert(!logoutRequested.flagged());
	}

	{
		TestAvatar av(account);
		TestIGRouter ir(av);

		SignalFlagger transferRequested;
		SignalFlagger logoutRequested;
		_test_avatar_logoutWithTransferRequested.connect(sigc::mem_fun(transferRequested, &SignalFlagger::set));
		_test_avatar_logoutRequested.connect(sigc::mem_fun(logoutRequested, &SignalFlagger::set));

		Atlas::Objects::Operation::Logout op;
		Root arg1;
		Root arg2;
		Root arg3;
		arg2->setAttr("teleport_host", "ec66b165-9814-44d4-8326-ba9f31ce3224");
		arg2->setAttr("teleport_port", 0xebc);
		arg2->setAttr("possess_key", "2f281182-f285-48d7-812a-4f706954aa56");
		arg2->setAttr("possess_entity_id", "1dab48d5-8784-4cfb-b1a2-e801fa99fc3a");
		op->modifyArgs().push_back(arg1);
		op->modifyArgs().push_back(arg2);
		op->modifyArgs().push_back(arg3);
		Eris::Router::RouterResult r = ir.test_handleOperation(op);
		assert(r == Eris::Router::HANDLED);
		assert(transferRequested.flagged());
		assert(!logoutRequested.flagged());
	}

	return 0;
}

// stubs

#include <Eris/Avatar.h>
#include <Eris/Connection.h>
#include <Eris/Entity.h>
#include <Eris/Log.h>
#include <Eris/SpawnPoint.h>
#include <Eris/TransferInfo.h>
#include <Eris/TypeInfo.h>
#include <Eris/TypeBoundRedispatch.h>
#include <Eris/TypeService.h>
#include <Eris/View.h>


namespace Eris {

Account::Account(Connection& con) :
		m_con(con),
		m_status(Status::DISCONNECTED),
		m_doingCharacterRefresh(false) {
}

Account::~Account() {
}

void Account::updateFromObject(const Atlas::Objects::Entity::Account& p) {
}

class AccountRouter : public Router {
public:
	explicit AccountRouter(Account* pl) {
	}

	~AccountRouter() override {
	}
};


Avatar::Avatar(Account& pl, std::string mindId, std::string entId) :
		m_account(pl),
		m_mindId(mindId),
		m_entityId(entId),
		m_entity(nullptr),
		m_lastOpTime(0.0),
		m_view(new View(*this)),
		m_isAdmin(false) {
}

Avatar::~Avatar() {
}

void Avatar::onTransferRequested(const TransferInfo& transfer) {
}

Connection& Avatar::getConnection() const {
	return m_account.getConnection();
}

void Avatar::logoutRequested() {
	_test_avatar_logoutRequested();
}

void Avatar::logoutRequested(const TransferInfo& transferInfo) {
	_test_avatar_logoutWithTransferRequested();
}

void Avatar::updateWorldTime(double seconds) {
}

BaseConnection::BaseConnection(boost::asio::io_service& io_service, std::string cnm,
							   std::string id) :
		_io_service(io_service),
		_factories(new Atlas::Objects::Factories()),
		_status(DISCONNECTED),
		_id(id),
		_clientName(cnm),
		_bridge(nullptr),
		_host(""),
		_port(0) {
}

BaseConnection::~BaseConnection() {
}

int BaseConnection::connectRemote(const std::string& host, short port) {
	return 0;
}

int BaseConnection::connectLocal(const std::string& socket) {
	return 0;
}

void BaseConnection::onConnect() {
}

void BaseConnection::setStatus(Status sc) {
}

struct ConnectionDecoder : Atlas::Objects::ObjectsDecoder {
	Connection& m_connection;

	ConnectionDecoder(Connection& connection, const Atlas::Objects::Factories& factories) :
			ObjectsDecoder(factories), m_connection(connection) {
	}

	void objectArrived(Atlas::Objects::Root obj) override {
		m_connection.objectArrived(obj);
	}
};


Connection::Connection(boost::asio::io_service& io_service,
					   Eris::EventService& event_service,
					   std::string cnm,
					   const std::string& host,
					   short port) :
		BaseConnection(io_service, cnm, "game_"),
		m_decoder(new ConnectionDecoder(*this, *_factories)),
		_eventService(event_service),
		_host(host),
		_port(port),
		m_typeService(new TypeService(*this)),
		m_defaultRouter(nullptr),
		m_lock(0),
		m_info{host},
		m_responder(nullptr) {
	_bridge = m_decoder.get();
}

Connection::~Connection() {
}

void Connection::objectArrived(Root obj) {
}

void Connection::send(const Atlas::Objects::Root& obj) {
}

void Connection::setStatus(Status ns) {
}

void Connection::handleFailure(const std::string& msg) {
}

void Connection::handleTimeout(const std::string& msg) {
}

void Connection::dispatch() {
}

void Connection::onConnect() {
}

void Connection::registerRouterForTo(Router* router, const std::string& toId) {
}

void Connection::unregisterRouterForTo(Router* router, const std::string& fromId) {
}

void Entity::setFromRoot(const Root& obj, bool includeTypeInfoAttributes) {
}

TypeService::TypeService(Connection& con) :
		m_con(con),
		m_inited(false) {
}

TypeService::~TypeService() {
}

TypeInfo* TypeService::getTypeForAtlas(const Root& obj) {
	Eris::TypeInfo* ti = new TypeInfo("18fda62d-7bc1-48cc-84ee-1b249a591ef6", *this);
	if (stub_type_bound) {
		ti->validateBind();
	}
	return ti;
}

TypeInfo* TypeService::getTypeByName(const std::string& id) {
	return 0;
}

void TypeInfo::validateBind() {
	m_bound = true;
}

View::View(Avatar& avatar)
		: m_owner(avatar) {

}

View::~View() {
}


void View::deleteEntity(const std::string& eid) {
}

ViewEntity* View::getEntity(const std::string& eid) const {
	return 0;
}

bool View::isPending(const std::string& eid) const {
	return false;
}

void View::appear(const std::string& eid, double stamp) {
}

void View::disappear(const std::string& eid) {
}

void View::sight(const RootEntity& gent) {
}

void View::unseen(const std::string& eid) {
}

void View::sendLookAt(const std::string& eid) {
}

void View::getEntityFromServer(const std::string& eid) {
}

bool Entity::isVisible() const {
	return false;
}

TransferInfo::TransferInfo(std::string host, int port,
						   std::string key, std::string id)
		: m_host(host),
		  m_port(port),
		  m_possess_key(key),
		  m_possess_entity_id(id) {
}

TypeInfo::TypeInfo(std::string id, TypeService& ts) :
		m_bound(false),
		m_name(id),
		m_typeService(ts) {
}

bool TypeInfo::isA(TypeInfo* tp) const {
	return false;
}

void TypeInfo::onPropertyChanges(const std::string& attributeName,
								 const Atlas::Message::Element& element) {
}

TypeBoundRedispatch::TypeBoundRedispatch(Connection& con,
										 const Root& obj,
										 TypeInfo* unbound) :
		Redispatch(con, obj),
		m_con(con) {
}

EventService::EventService(boost::asio::io_service& io_service)
		: m_io_service(io_service) {}

EventService::~EventService() {
}

void EventService::runOnMainThread(std::function<void()> const&,
								   std::shared_ptr<bool> activeMarker) {
}

Router::~Router() {
}

Router::RouterResult Router::handleObject(const Root& obj) {
	return IGNORED;
}

Router::RouterResult Router::handleOperation(const RootOperation&) {
	return IGNORED;
}

Router::RouterResult Router::handleEntity(const RootEntity&) {
	return IGNORED;
}

TimedEvent::~TimedEvent() {
}

void doLog(LogLevel lvl, const std::string& msg) {
}

} // namespace Eris
