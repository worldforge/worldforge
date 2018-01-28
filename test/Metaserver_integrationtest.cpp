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

#include <Eris/Metaserver.h>

int main()
{
    return 0;
}

// stubs

#include <Eris/Exceptions.h>
#include <Eris/LogStream.h>
#include <Eris/MetaQuery.h>
#include <Eris/EventService.h>

namespace Eris
{

BaseConnection::BaseConnection(boost::asio::io_service& io_service, const std::string &cnm,
    const std::string &id,
    Atlas::Bridge &br) :
            _io_service(io_service),
    _status(DISCONNECTED),
    _id(id),
    _clientName(cnm),
    _bridge(br),
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

int BaseConnection::connectLocal(const std::string &socket)
{
    return 0;
}

void BaseConnection::onConnect()
{
}

void BaseConnection::setStatus(Status sc)
{
}

Atlas::Objects::ObjectsEncoder& Eris::StreamSocket::getEncoder()
{
    return *(Atlas::Objects::ObjectsEncoder*)(0);
}

ServerInfo::ServerInfo(const std::string &host) :
    m_status(INVALID),
    _host(host)
{
}

void ServerInfo::processServer(const Atlas::Objects::Entity::RootEntity &svr)
{
}

void ServerInfo::setPing(int p)
{
}

EventService::EventService(boost::asio::io_service& io_service)
: m_io_service(io_service)
{}

EventService::~EventService()
{
}

void EventService::runOnMainThread(std::function<void ()> const&)
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

}
