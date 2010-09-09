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

#include <Eris/DeleteLater.h>
#include <Eris/Exceptions.h>
#include <Eris/LogStream.h>
#include <Eris/MetaQuery.h>
#include <Eris/PollDefault.h>
#include <Eris/Timeout.h>

namespace Eris
{

MetaQuery::MetaQuery(Meta *ms, const std::string &host, unsigned int sindex) :
    BaseConnection("eris-metaquery", "mq_" + host + "-", ms),
    _host(host),
    _meta(ms),
    _queryNo(0),
    m_serverIndex(sindex),
    m_complete(false)
{

}

MetaQuery::~MetaQuery()
{
}

void MetaQuery::setComplete()
{
}

void MetaQuery::onConnect()
{
}

void MetaQuery::handleFailure(const std::string &msg)
{
}

void MetaQuery::handleTimeout(const std::string&)
{
}

long MetaQuery::getElapsed()
{
    return 0L;
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

void BaseConnection::recv()
{
}

Eris::Poll* Eris::Poll::_inst = 0;

Eris::Poll& Eris::Poll::instance()
{
  if(!_inst)
    _inst = new PollDefault();

  return *_inst;
}

void PollDefault::addStream(const basic_socket_stream* str, Check c)
{
}

void PollDefault::changeStream(const basic_socket_stream* str, Check c)
{
}

void PollDefault::removeStream(const basic_socket_stream* str)
{
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

void Timeout::reset(unsigned long milli)
{
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

BaseException::~BaseException() throw()
{
}

void pushDeleteLater(BaseDeleteLater* bl)
{
}

BaseDeleteLater::~BaseDeleteLater()
{
}

void doLog(LogLevel lvl, const std::string& msg)
{
}

}
