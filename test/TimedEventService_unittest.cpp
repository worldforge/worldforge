// Eris Online RPG Protocol Library
// Copyright (C) 2011 Alistair Riddoch
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

#include <Eris/TimedEventService.h>

#include <sigc++/functors/mem_fun.h>

#include <cassert>

int main()
{
    boost::asio::io_service io_service;

    {
        Eris::TimedEventService ted(io_service);

        Eris::TimedEventService * ted2 = &Eris::TimedEventService::instance();

        assert(ted2 != 0);

    }

    {
        Eris::TimedEventService ted(io_service);
        bool called = false;
        Eris::TimedEvent te(boost::posix_time::seconds(-10), [&](){called = true;});
        io_service.run_one();
        io_service.reset();
        assert(called);
    }


    return 0;
}

// stubs


