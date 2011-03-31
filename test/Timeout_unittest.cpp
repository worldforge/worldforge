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

#include <Eris/Timeout.h>

#include <sigc++/trackable.h>
#include <sigc++/functors/mem_fun.h>

#include <cassert>

class TestTimeout : public Eris::Timeout
{
  public:
    TestTimeout(unsigned long milli) : Eris::Timeout(milli) { }

    bool test_fired() const { return _fired; }
};

class SignalChecker : public sigc::trackable
{
  private:
    bool m_fired;
  public:
    SignalChecker() : m_fired(false) { }
    void emit() { m_fired = true; }
    bool fired() const { return m_fired; }
};

int main()
{
    {
        Eris::Timeout * to = new Eris::Timeout(1);

        delete to;
    }

    {
        TestTimeout * to = new TestTimeout(1);

        assert(!to->test_fired());

        delete to;
    }

    {
        TestTimeout * to = new TestTimeout(1);
        assert(!to->test_fired());

        SignalChecker sc;
        assert(!sc.fired());
        to->Expired.connect(sigc::mem_fun(&sc, &SignalChecker::emit));

        to->expired();

        assert(to->test_fired());
        assert(sc.fired());

        delete to;
    }

    return 0;
}

// stubs

#include <Eris/TimedEventService.h>

namespace Eris {

TimedEventService* TimedEventService::static_instance = NULL;

TimedEventService::TimedEventService()
{
}

TimedEventService* TimedEventService::instance()
{
    if (!static_instance)
    {
        static_instance = new TimedEventService;
    }
    
    return static_instance;
}

void TimedEventService::registerEvent(TimedEvent* te)
{
    assert(te);
}

void TimedEventService::unregisterEvent(TimedEvent* te)
{
    assert(te);    
}

} // namespace Eris
