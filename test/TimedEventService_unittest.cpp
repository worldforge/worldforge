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

#include <Eris/TimedEventService.h>

#include <sigc++/functors/mem_fun.h>

#include <cassert>

class TestSignalTracker 
{
  protected:
    bool m_called;
  public:
    TestSignalTracker() : m_called(false) { }

    bool called() const { return m_called; }

    void call() { m_called = true; }
};

int main()
{
    {
        Eris::TimedEventService * ted = Eris::TimedEventService::instance();

        assert(ted != 0);

        Eris::TimedEventService::del();
    }

    {
        Eris::TimedEventService * ted = Eris::TimedEventService::instance();

        assert(ted != 0);
        ted->Idle.emit();

        Eris::TimedEventService::del();
    }

    {
        TestSignalTracker tst;
        assert(!tst.called());

        Eris::TimedEventService * ted = Eris::TimedEventService::instance();
        assert(ted != 0);

        ted->Idle.emit();
        assert(!tst.called());

        ted->Idle.connect(sigc::mem_fun(tst, &TestSignalTracker::call));
        assert(!tst.called());

        ted->Idle.emit();
        assert(tst.called());

        Eris::TimedEventService::del();
    }

    return 0;
}

// stubs

#include <Eris/Poll.h>

bool Eris::Poll::new_timeout_ = false;
