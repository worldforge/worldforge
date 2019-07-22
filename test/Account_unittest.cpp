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
#include <Eris/EventService.h>

int main()
{
    // Constructor with bad args throws
    {
        try {
            new Eris::Account(0);
            return 1;
        }
        catch (const Eris::InvalidOperation& io) {
        }
    }

    // Constructor
    {
        boost::asio::io_service io_service;
        Eris::EventService eventService(io_service);
        Eris::Connection * c = new Eris::Connection(io_service, eventService, "", "", 0);
        new Eris::Account(c);
    }

    // Destructor
    {
        boost::asio::io_service io_service;
        Eris::EventService eventService(io_service);
        Eris::Connection * c = new Eris::Connection(io_service, eventService, "", "", 0);
        Eris::Account * ac = new Eris::Account(c);
        delete ac;
    }

    return 0;
}

// stubs

#include <Eris/Avatar.h>
#include <Eris/CharacterType.h>
#include <Eris/Log.h>
#include <Eris/Response.h>
#include <Eris/Router.h>
#include <Eris/SpawnPoint.h>
#include <Eris/TypeService.h>

#include <Atlas/Objects/RootOperation.h>

using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Root;

namespace Eris {

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

Avatar::Avatar(Account& pl, std::string mindId, std::string entityId) :
    m_account(pl),
    m_mindId(mindId),
    m_entityId(entityId),
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

BaseConnection::BaseConnection(boost::asio::io_service& io_service, const std::string &cnm,
    const std::string &id,
    Atlas::Bridge& br) : _io_service(io_service), _bridge(br)
{
}

BaseConnection::~BaseConnection()
{
}

int BaseConnection::connectRemote(const std::string &host, short port)
{
    return 0;
}

int BaseConnection::connectLocal(const std::string &socket)
{
    return 0;
}

void BaseConnection::setStatus(Status sc)
{
}

void BaseConnection::onConnect()
{
}

Connection::Connection(boost::asio::io_service& io_service, Eris::EventService& event_service, const std::string &cnm, const std::string& host, short port) :
    BaseConnection(io_service, cnm, "game_", *this), _eventService(event_service), _port(port)
{
}

Connection::~Connection()
{
}

void Connection::dispatch()
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

void Connection::registerRouterForTo(Router* router, const std::string& toId)
{
}

void Connection::unregisterRouterForTo(Router* router, const std::string& toId)
{
}

void Connection::lock()
{
}

void Connection::unlock()
{
}

EventService& Connection::getEventService()
{
    return _eventService;
}

void Connection::send(const Atlas::Objects::Root &obj)
{
}

CharacterType::CharacterType(const std::string& name,
                             const std::string& description)
: m_name(name), m_description(description)
{
}

ServerInfo::ServerInfo()
{
}

SpawnPoint::SpawnPoint(std::string name,
        CharacterTypeStore availableCharacterTypes,
        std::string description) :
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

TimedEvent::TimedEvent(EventService& eventService, const boost::posix_time::time_duration& duration, const std::function<void()>& callback)
{
}

TimedEvent::~TimedEvent()
{
}

EventService::EventService(boost::asio::io_service& io_service)
: m_io_service(io_service)
{}

EventService::~EventService()
{
}

void doLog(LogLevel lvl, const std::string& msg)
{
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
