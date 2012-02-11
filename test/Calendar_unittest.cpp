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

#include "SignalFlagger.h"

#include <Eris/Calendar.h>

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/Avatar.h>
#include <Eris/View.h>
#include <Eris/Log.h>

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>
#include <limits>

#include <cassert>

class TestAvatar : public Eris::Avatar {
  public:
    TestAvatar(Eris::Account * ac, const std::string & ent_id) :
               Eris::Avatar(*ac, ent_id) { }

};

static const int MPY = 12;
static const int DPM = 28;
static const int HPD = 24;
static const int MPH = 60;
static const int SPM = 60;

class TestCalendar : public Eris::Calendar {
  public:
    TestCalendar(Eris::Avatar * av) : Eris::Calendar(av) { }

    void test_setSaneDefault() {
        Atlas::Message::MapType data;
        data["months_per_year"] = MPY;
        data["days_per_month"] = DPM;
        data["hours_per_day"] = HPD;
        data["minutes_per_hour"] = MPH;
        data["seconds_per_minute"] = SPM;
        initFromCalendarAttr(data);
    }

    void test_setInvalidDefault() {
        Atlas::Message::MapType data;
        initFromCalendarAttr(data);
    }

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

static double stub_worldtime = 0.;
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

    /// Test overflow of 32bit seconds

    // Test calendar process time at the origin of time
    {
        std::string fake_char_id("1");
        Eris::Avatar * ea = new TestAvatar(0, fake_char_id);

        TestCalendar ec(ea);

        ec.test_setSaneDefault();

        stub_worldtime = 0;

        Eris::DateTime dt = ec.now();

        assert(dt.year() == 0);
        assert(dt.month() == 0);
        assert(dt.dayOfMonth() == 0);
        assert(dt.hours() == 0);
        assert(dt.minutes() == 0);
        assert(dt.seconds() == 0);
    }

    // Test calendar process time at the origin of time plus some time
    {
        std::string fake_char_id("1");
        Eris::Avatar * ea = new TestAvatar(0, fake_char_id);

        TestCalendar ec(ea);

        ec.test_setSaneDefault();

        stub_worldtime = 1LL * MPY * DPM * HPD * MPH * SPM;

        Eris::DateTime dt = ec.now();

        assert(dt.year() == 1);
        assert(dt.month() == 0);
        assert(dt.dayOfMonth() == 0);
        assert(dt.hours() == 0);
        assert(dt.minutes() == 0);
        assert(dt.seconds() == 0);

        stub_worldtime = 1000LL * MPY * DPM * HPD * MPH * SPM;

        dt = ec.now();

        assert(dt.year() == 1000);
        assert(dt.month() == 0);
        assert(dt.dayOfMonth() == 0);
        assert(dt.hours() == 0);
        assert(dt.minutes() == 0);
        assert(dt.seconds() == 0);
    }

    // Test calendar process time at the limit of an unsigned 32bit int seconds
    {
        std::string fake_char_id("1");
        Eris::Avatar * ea = new TestAvatar(0, fake_char_id);

        TestCalendar ec(ea);

        ec.test_setSaneDefault();

        stub_worldtime = std::numeric_limits<unsigned int>::max() + 10LL;

        Eris::DateTime dt = ec.now();

        // std::cout << dt.year() << ":" << dt.month() << ":" << dt.dayOfMonth() << ":" << dt.hours() << ":" << dt.minutes() << ":" << dt.seconds() << std::endl;

        assert(dt.year() > 0);
    }

    // Test event emitted
    {
        SignalFlagger flagger;
        std::string fake_char_id("1");
        Eris::Avatar * ea = new TestAvatar(0, fake_char_id);

        TestCalendar ec(ea);
        ec.Updated.connect(sigc::mem_fun(flagger,
                &SignalFlagger::set));

        ec.test_setSaneDefault();

        assert(flagger.flagged());
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

void doLog(LogLevel lvl, const std::string& msg)
{
    std::cerr << msg << std::endl << std::flush;
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
    return stub_worldtime;
}

void Avatar::onTransferRequested(const TransferInfo &transfer) {

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
    static Atlas::Message::Element res;
    return res;
}

sigc::connection Entity::observe(const std::string& attr, const AttrChangedSlot& slot)
{
    return m_observers[attr].connect(slot);
}

}
