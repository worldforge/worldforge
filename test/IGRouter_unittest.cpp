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
#include <Eris/Avatar.h>
#include <Eris/Connection.h>
#include <Eris/IGRouter.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>

using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;

class TestAvatar : public Eris::Avatar
{
  public:
    TestAvatar() :
          Eris::Avatar(*new Eris::Account(new Eris::Connection("", "", 0, false)), "") { }
};

class TestIGRouter : public Eris::IGRouter
{
  public:
    TestIGRouter(Eris::Avatar * av) : Eris::IGRouter(av) { }

    RouterResult test_handleOperation(const RootOperation& op) {
        return this->handleOperation(op);
    }
};

int main()
{
    {
        TestAvatar * av = new TestAvatar();
        new Eris::IGRouter(av);
    }

    {
        TestAvatar * av = new TestAvatar();
        Eris::IGRouter * ir = new Eris::IGRouter(av);
        delete ir;
    }

    {
        TestAvatar * av = new TestAvatar();
        TestIGRouter * ir = new TestIGRouter(av);

        RootOperation op;
        ir->test_handleOperation(op);
    }

    {
        TestAvatar * av = new TestAvatar();
        TestIGRouter * ir = new TestIGRouter(av);

        RootOperation op;
        op->setSeconds(0);
        ir->test_handleOperation(op);
    }

    {
        TestAvatar * av = new TestAvatar();
        TestIGRouter * ir = new TestIGRouter(av);

        Atlas::Objects::Operation::Sight op;
        op->setArgs1(Root());
        ir->test_handleOperation(op);
    }

    {
        TestAvatar * av = new TestAvatar();
        TestIGRouter * ir = new TestIGRouter(av);

        Atlas::Objects::Operation::Sight op;
        op->setArgs1(RootEntity());
        ir->test_handleOperation(op);
    }

    return 0;
}

// stubs

#include <Eris/Avatar.h>
#include <Eris/CharacterType.h>
#include <Eris/Connection.h>
#include <Eris/Entity.h>
#include <Eris/Log.h>
#include <Eris/SpawnPoint.h>
#include <Eris/TransferInfo.h>
#include <Eris/TypeInfo.h>
#include <Eris/TypeBoundRedispatch.h>
#include <Eris/TypeService.h>
#include <Eris/View.h>

namespace Atlas { namespace Objects { namespace Operation {

int UNSEEN_NO = -1;

} } }

namespace Eris {

Account::Account(Connection *con) :
    m_con(con),
    m_status(DISCONNECTED),
    m_doingCharacterRefresh(false)
{
}

Account::~Account()
{
}

void Account::updateFromObject(const Atlas::Objects::Entity::Account &p)
{
}

Avatar::Avatar(Account& pl, const std::string& entId) :
    m_account(pl),
    m_entityId(entId),
    m_entity(NULL),
    m_lastOpTime(0.0),
    m_isAdmin(false)
{
}

Avatar::~Avatar()
{
}

void Avatar::onTransferRequested(const TransferInfo &transfer)
{
}

Connection* Avatar::getConnection() const
{
    return m_account.getConnection();
}

void Avatar::logoutRequested()
{
}

void Avatar::logoutRequested(const TransferInfo& transferInfo)
{
}

void Avatar::updateWorldTime(double seconds)
{
}

BaseConnection::BaseConnection(const std::string &cnm, 
    const std::string &id,
    Atlas::Bridge *br) :
    _encode(NULL),
    _sc(NULL),
    m_codec(NULL),
    _status(DISCONNECTED),
    _id(id),
    _stream(NULL),
    _clientName(cnm),
    _bridge(br),
    _timeout(NULL),
    _host(""),
    _port(0)
{
}

BaseConnection::~BaseConnection()
{
}

int BaseConnection::connect(const std::string &host, short port)
{
    return 0;
}

void BaseConnection::onConnect()
{
}

void BaseConnection::setStatus(Status sc)
{
}

Connection::Connection(const std::string &cnm, const std::string& host, short port, bool dbg) :
    BaseConnection(cnm, "game_", this),
    _host(host),
    _port(port),
    m_typeService(new TypeService(this)),
    m_defaultRouter(NULL),
    m_lock(0),
    m_info(host),
    m_responder(0)
{
}

Connection::~Connection()
{
}

void Connection::objectArrived(const Root& obj)
{
}

void Connection::send(const Atlas::Objects::Root &obj)
{
}

void Connection::setStatus(Status ns)
{
}

void Connection::handleFailure(const std::string &msg)
{
}

void Connection::handleTimeout(const std::string& msg)
{
}

void Connection::onConnect()
{
}

void Connection::registerRouterForTo(Router* router, const std::string toId)
{
}

void Connection::unregisterRouterForTo(Router* router, const std::string fromId)
{
}

void Entity::setFromRoot(const Root& obj, bool allowMove, bool includeTypeInfoAttributes)
{       
}

TypeService::TypeService(Connection *con) : 
    m_con(con),
    m_inited(false)
{
}

TypeService::~TypeService()
{
}

TypeInfoPtr TypeService::getTypeForAtlas(const Root &obj)
{
    return new TypeInfo("18fda62d-7bc1-48cc-84ee-1b249a591ef6", this);
}

TypeInfoPtr TypeService::getTypeByName(const std::string &id)
{
    return 0;
}

void View::create(const RootEntity& gent)
{
}

void View::deleteEntity(const std::string& eid)
{
}

Entity* View::getEntity(const std::string& eid) const
{
    return 0;
}

bool View::isPending(const std::string& eid) const
{
    return false;
}

void View::appear(const std::string& eid, float stamp)
{
}

void View::disappear(const std::string& eid)
{
}

void View::sight(const RootEntity& gent)
{
}

void View::unseen(const std::string& eid)
{
}

TransferInfo::TransferInfo(const std::string &host, int port, 
                            const std::string &key, const std::string &id)
                               : m_host(host),
                                m_port(port),
                                m_possess_key(key),
                                m_possess_entity_id(id)
{
}

TypeInfo::TypeInfo(const std::string &id, TypeService *ts) :
    m_bound(false),
    m_name(id),
    m_atlasClassNo(0),
    m_moveCount(0),
    m_typeService(ts)
{
}

bool TypeInfo::isA(TypeInfoPtr tp)
{
    return false;
}

void TypeInfo::onAttributeChanges(const std::string& attributeName,
                                  const Atlas::Message::Element& element)
{
}

TypeBoundRedispatch::TypeBoundRedispatch(Connection* con, 
        const Root& obj, 
        TypeInfo* unbound) :
    Redispatch(con, obj),
    m_con(con)
{
}

ServerInfo::ServerInfo(const std::string &host) :
    m_status(INVALID),
    _host(host)
{
}

SpawnPoint::~SpawnPoint()
{
}

Router::~Router()
{
}

Router::RouterResult Router::handleObject(const Root& obj)
{
    return IGNORED;
}

Router::RouterResult Router::handleOperation(const RootOperation& )
{
    return IGNORED;
}

Router::RouterResult Router::handleEntity(const RootEntity& )
{
    return IGNORED;
}

void doLog(LogLevel lvl, const std::string& msg)
{
}

} // namespace Eris
