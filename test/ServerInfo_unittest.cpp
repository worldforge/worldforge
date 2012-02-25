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

#include "Eris/ServerInfo.h"
#include "ElementExerciser.h"
#include <Atlas/Objects/RootEntity.h>
#include <sigc++/slot.h>

namespace Eris
{

class TestServerInfo: public ServerInfo
{
public:
    void injectAtlas(const Atlas::Objects::Entity::RootEntity& svr)
    {
        processServer(svr);
    }
};
}

void testServerInfo(const Atlas::Objects::Entity::RootEntity& svr)
{
    Eris::TestServerInfo serverInfo;
    serverInfo.injectAtlas(svr);
}

int main()
{

    //ctor
    new Eris::ServerInfo();

    {
        //ctor and dtor
        Eris::ServerInfo();
    }

    Eris::TestServerInfo info;

    sigc::slot<void, const Atlas::Objects::Entity::RootEntity&> slot(
            sigc::ptr_fun(&testServerInfo));
    ElementExerciser<Atlas::Objects::Entity::RootEntity> exerciser(slot);
    exerciser.addParam("ruleset", ElementParam("mason"));
    exerciser.addParam("name", ElementParam("a server", false));
    exerciser.addParam("clients", ElementParam(2));
    exerciser.addParam("server", ElementParam("localhost"));
    exerciser.addParam("uptime", ElementParam(1000.0f));
    exerciser.addParam("entities", ElementParam(200));
    exerciser.addParam("version", ElementParam("1.0"));
    exerciser.addParam("builddate", ElementParam("2000-01-01"));

    exerciser.exercise();

    return 0;
}
