// Eris Online RPG Protocol Library
// Copyright (C) 2011 Alistair Riddoch
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
#include <Eris/Connection.h>
#include <Eris/Exceptions.h>

int main()
{
    // Constructor with bad args throws
    {
        try {
            new Eris::Account(0);
            return 1;
        }
        catch (Eris::InvalidOperation io) {
        }
    }

    // Constructor
    {
        Eris::Connection * c = new Eris::Connection("", "", 0, false);
        new Eris::Account(c);
    }

    // Destructor
    {
        Eris::Connection * c = new Eris::Connection("", "", 0, false);
        Eris::Account * ac = new Eris::Account(c);
        delete ac;
    }

    return 0;
}

// stubs

#include <Eris/Avatar.h>
#include <Eris/CharacterType.h>
#include <Eris/DeleteLater.h>
#include <Eris/Log.h>
#include <Eris/Response.h>
#include <Eris/Router.h>
#include <Eris/SpawnPoint.h>
#include <Eris/Timeout.h>

#include <Atlas/Objects/RootOperation.h>

using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Root;

namespace Eris {

bool NullResponse::responseReceived(const RootOperation&)
{
    return false;
}

ResponseBase::~ResponseBase()
{
}

BaseDeleteLater::~BaseDeleteLater()
{
}

void pushDeleteLater(BaseDeleteLater* bl)
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

void Avatar::deactivate()
{
}

void Avatar::onTransferRequested(const TransferInfo &transfer)
{
}

BaseConnection::BaseConnection(const std::string &cnm, 
    const std::string &id,
    Atlas::Bridge *br)
{
}

BaseConnection::~BaseConnection()
{
}

int BaseConnection::connect(const std::string &host, short port)
{
    return 0;
}

void BaseConnection::setStatus(Status sc)
{
}

void BaseConnection::onConnect()
{
}

Connection::Connection(const std::string &cnm, const std::string& host, short port, bool dbg) :
    BaseConnection(cnm, "game_", this), _port(port)
{
}

Connection::~Connection()
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

void Connection::objectArrived(const Root& obj)
{
}

void Connection::onConnect()
{
}

void Connection::setDefaultRouter(Router* router)
{
}

void Connection::clearDefaultRouter()
{
}

void Connection::registerRouterForTo(Router* router, const std::string toId)
{
}

void Connection::unregisterRouterForTo(Router* router, const std::string toId)
{
}

void Connection::lock()
{
}

void Connection::unlock()
{
}

void Connection::send(const Atlas::Objects::Root &obj)
{
}

CharacterType::CharacterType(const std::string& name,
                             const std::string& description)
: m_name(name), m_description(description)
{
}

CharacterType::~CharacterType()
{
}

void ResponseTracker::await(int serialno, ResponseBase* resp)
{
}

ServerInfo::ServerInfo()
{
}

SpawnPoint::SpawnPoint(const std::string& name,
        const CharacterTypeStore& availableCharacterTypes,
        const std::string& description) :
    m_name(name), m_availableCharacterTypes(availableCharacterTypes),
            m_description(description)
{
}

SpawnPoint::~SpawnPoint()
{
}

const std::string& SpawnPoint::getName() const
{
    return m_name;
}

Timeout::Timeout(unsigned long milli) :
        _fired(false)
{
}

Timeout::~Timeout()
{
}

void Timeout::expired()
{
}

void doLog(LogLevel lvl, const std::string& msg)
{
}

void* clearMemberResponse(void* d)
{
    return 0;
}

long getNewSerialno()
{
    static long _nextSerial = 1001;
    // note this will eventually loop (in theorey), but that's okay
    // FIXME - using the same intial starting offset is problematic
    // if the client dies, and quickly reconnects
    return _nextSerial++;
}

} // namespace Eris
