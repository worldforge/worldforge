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

#include <Eris/IGRouter.h>

int main()
{
    return 0;
}

// stubs

#include <Eris/Avatar.h>
#include <Eris/Connection.h>
#include <Eris/Entity.h>
#include <Eris/Log.h>
#include <Eris/TransferInfo.h>
#include <Eris/TypeInfo.h>
#include <Eris/TypeBoundRedispatch.h>
#include <Eris/TypeService.h>
#include <Eris/View.h>

using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;

namespace Atlas { namespace Objects { namespace Operation {

int UNSEEN_NO = -1;

} } }

namespace Eris {

Connection* Avatar::getConnection() const
{
    return 0;
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

void Connection::registerRouterForTo(Router* router, const std::string toId)
{
}

void Connection::unregisterRouterForTo(Router* router, const std::string fromId)
{
}

void Entity::setFromRoot(const Root& obj, bool allowMove, bool includeTypeInfoAttributes)
{       
}

TypeInfoPtr TypeService::getTypeForAtlas(const Root &obj)
{
    return 0;
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

bool TypeInfo::isA(TypeInfoPtr tp)
{
    return false;
}

TypeBoundRedispatch::TypeBoundRedispatch(Connection* con, 
        const Root& obj, 
        TypeInfo* unbound) :
    Redispatch(con, obj),
    m_con(con)
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
