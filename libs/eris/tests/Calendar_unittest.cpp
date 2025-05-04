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
#include <Eris/EventService.h>

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>
#include <limits>

#include <cassert>

static const int MPY = 12;
static const int DPM = 28;
static const int HPD = 24;
static const int MPH = 60;
static const int SPM = 60;

struct TestCalendar : public Eris::Calendar {

	std::chrono::milliseconds test_now = {};

    TestCalendar() : Eris::Calendar([this](){return test_now;}) { }

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

};

class TestConnection : public Eris::Connection {
  public:
    TestConnection(boost::asio::io_context& io_service,
    		Eris::EventService& eventService,
            const std::string &cnm,
            const std::string& host,
            short port) :
                Eris::Connection(io_service, eventService,  cnm, host, port) {
    }

    virtual void send(const Atlas::Objects::Root &obj) {
        std::cout << "Sending " << obj->getParent()
                  << std::endl;
    }
};


int main()
{

    /////////////////////////// DateTime ///////////////////////////

    // Test constructor and destructor
    {
        Eris::DateTime dt{};
    }

    // Test valid()
    {
        Eris::DateTime dt{};

        assert(!dt.valid());
    }

    // Test year()
    {
        Eris::DateTime dt{};

        dt.year();
    }

    // Test month()
    {
        Eris::DateTime dt{};

        dt.month();
    }

    // Test dayOfMonth()
    {
        Eris::DateTime dt{};

        dt.dayOfMonth();
    }

    // Test seconds()
    {
		Eris::DateTime dt{};

        dt.seconds();
    }

    // Test minutes()
    {
		Eris::DateTime dt{};

        dt.minutes();
    }

    // Test hours()
    {
		Eris::DateTime dt{};

        dt.hours();
    }

    // Test year()
    {
		Eris::DateTime dt{};

        dt.year();
    }

    /////////////////////////// Calendar ///////////////////////////

    // Test constructor
    {
        Eris::Calendar ec([](){return std::chrono::milliseconds(0);});
    }

    /// Test overflow of 32bit seconds

    // Test calendar process time at the origin of time
    {

        TestCalendar ec;

        ec.test_setSaneDefault();

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

        TestCalendar ec;

        ec.test_setSaneDefault();

        ec.test_now = std::chrono::milliseconds{1LL * MPY * DPM * HPD * MPH * SPM * 1000};

        Eris::DateTime dt = ec.now();

        assert(dt.year() == 1);
        assert(dt.month() == 0);
        assert(dt.dayOfMonth() == 0);
        assert(dt.hours() == 0);
        assert(dt.minutes() == 0);
        assert(dt.seconds() == 0);

        ec.test_now = std::chrono::milliseconds{1000LL * MPY * DPM * HPD * MPH * SPM * 1000};

        dt = ec.now();

        assert(dt.year() == 1000);
        assert(dt.month() == 0);
        assert(dt.dayOfMonth() == 0);
        assert(dt.hours() == 0);
        assert(dt.minutes() == 0);
        assert(dt.seconds() == 0);
    }

    // Test event emitted
    {
        SignalFlagger flagger;

        TestCalendar ec;
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

Avatar::Avatar(Account& pl, std::string mindId, std::string entId) :
    m_account(pl),
    m_mindId(mindId),
    m_entityId(entId),
    m_entity(nullptr),
    m_isAdmin(false)
{
    m_view = std::make_unique<View>(*this);
}

Avatar::~Avatar()
{
}

void Avatar::onTransferRequested(const TransferInfo &transfer) {

}

View::View(Avatar& av) :
    m_owner(av),
    m_topLevel(nullptr),
    m_maxPendingCount(10)
{

}

View::~View()
{
}

bool Entity::hasProperty(const std::string& p) const
{
    return false;
}

const Atlas::Message::Element& Entity::valueOfProperty(const std::string& name) const
{
    static Atlas::Message::Element res;
    return res;
}

sigc::connection Entity::observe(const std::string& attr, const PropertyChangedSlot& slot, bool)
{
    return m_observers[attr].connect(slot);
}

Router::RouterResult View::handleOperation(Atlas::Objects::SmartPtr<Atlas::Objects::Operation::RootOperationData> const&) {
	return HANDLED;
}

}
