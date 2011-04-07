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

#include <Eris/Calendar.h>

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/Avatar.h>
#include <Eris/View.h>
#include <Eris/Log.h>

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>

#include <cassert>

class TestAvatar : public Eris::Avatar {
  public:
    TestAvatar(Eris::Account * ac, const std::string & ent_id) :
               Eris::Avatar(*ac, ent_id) { }

};

class TestCalendar : public Eris::Calendar {
  public:
    TestCalendar(Eris::Avatar * av) : Eris::Calendar(av) { }

    void test_topLevelEntityChanged() {
        topLevelEntityChanged();
    }
};

class TestConnection : public Eris::Connection {
  public:
    TestConnection(const std::string & name, const std::string & host,
                   short port, bool debug) :
                   Eris::Connection(name, host, port, debug) { }

    virtual void send(const Atlas::Objects::Root &obj) {
        std::cout << "Sending " << obj->getParents().front()
                  << std::endl << std::flush;
    }
};

static void writeLog(Eris::LogLevel, const std::string & msg)
{       
    std::cerr << msg << std::endl << std::flush;
}

int main()
{
    Eris::Logged.connect(sigc::ptr_fun(writeLog));
    Eris::setLogLevel(Eris::LOG_DEBUG);

    /////////////////////////// DateTime ///////////////////////////

    // Test constructor and destructor
    {
        Eris::DateTime dt;
    }

    // Test valid()
    {
        Eris::DateTime dt;

        assert(!dt.valid());
    }

    // Test year()
    {
        Eris::DateTime dt;

        dt.year();
    }

    // Test month()
    {
        Eris::DateTime dt;

        dt.month();
    }

    // Test dayOfMonth()
    {
        Eris::DateTime dt;

        dt.dayOfMonth();
    }

    // Test seconds()
    {
        Eris::DateTime dt;

        dt.seconds();
    }

    // Test minutes()
    {
        Eris::DateTime dt;

        dt.minutes();
    }

    // Test hours()
    {
        Eris::DateTime dt;

        dt.hours();
    }

    // Test year()
    {
        Eris::DateTime dt;

        dt.year();
    }

    /////////////////////////// Calendar ///////////////////////////

    // Test constructor
    {
        std::string fake_char_id("1");
        Eris::Avatar * ea = new TestAvatar(0, fake_char_id);
        Eris::Calendar ec(ea);
    }

    // FIXME Can't set the toplevel on the view, which is required to
    // test all paths through the constructor.

    // Test topLevelEntityChanged()
    {
        std::string fake_char_id("1");
        Eris::Avatar * ea = new TestAvatar(0, fake_char_id);

        TestCalendar ec(ea);

        ec.test_topLevelEntityChanged();
    }

    return 0;
}

// stubs

#include <Eris/Entity.h>

namespace Eris {

sigc::signal<void, LogLevel, const std::string&> Logged;

void setLogLevel(LogLevel lvl)
{
}

Avatar::Avatar(Account& pl, const std::string& entId) :
    m_account(pl),
    m_entityId(entId),
    m_entity(NULL),
    m_stampAtLastOp(WFMath::TimeStamp::now()),
    m_lastOpTime(0.0),
    m_isAdmin(false)
{
    m_view = new View(this);
}

Avatar::~Avatar()
{
}

double Avatar::getWorldTime()
{
    return 0.;
}

View::View(Avatar* av) :
    m_owner(av),
    m_topLevel(NULL),
    m_maxPendingCount(10)
{
    
}

View::~View()
{
}

bool Entity::hasAttr(const std::string&) const
{
    return false;
}

const Atlas::Message::Element& Entity::valueOfAttr(const std::string& attr) const
{
}

sigc::connection Entity::observe(const std::string& attr, const AttrChangedSlot& slot)
{
}

}
