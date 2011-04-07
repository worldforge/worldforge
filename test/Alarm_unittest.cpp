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

#include <Eris/Alarm.h>

#include <Eris/Log.h>

#include "SignalFlagger.h"

#include <iostream>

#include <cassert>

static void writeLog(Eris::LogLevel, const std::string & msg)
{
    std::cerr << msg << std::endl << std::flush;
}

int main()
{
    Eris::Logged.connect(sigc::ptr_fun(writeLog));
    Eris::setLogLevel(Eris::LOG_DEBUG);

    // Test constructor
    {
        SignalFlagger expired_checker;
        Eris::Alarm alm(1000, sigc::mem_fun(expired_checker, &SignalFlagger::set));

        assert(!expired_checker.flagged());
    }

    // Test due()
    {
        SignalFlagger expired_checker;
        Eris::Alarm alm(1000, sigc::mem_fun(expired_checker, &SignalFlagger::set));

        alm.due();

        assert(!expired_checker.flagged());
    }

    // Test expired()
    {
        SignalFlagger expired_checker;
        Eris::Alarm alm(1000, sigc::mem_fun(expired_checker, &SignalFlagger::set));

        alm.expired();

        assert(expired_checker.flagged());
    }

    return 0;
}
