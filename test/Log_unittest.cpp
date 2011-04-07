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


#include <Eris/Log.h>

#include <cassert>

int main()
{
    assert(Eris::getLogLevel() == Eris::DEFAULT_LOG);

    Eris::setLogLevel(Eris::LOG_ERROR);
    assert(Eris::getLogLevel() == Eris::LOG_ERROR);

    Eris::setLogLevel(Eris::LOG_WARNING);
    assert(Eris::getLogLevel() == Eris::LOG_WARNING);

    Eris::setLogLevel(Eris::LOG_NOTICE);
    assert(Eris::getLogLevel() == Eris::LOG_NOTICE);

    Eris::setLogLevel(Eris::LOG_VERBOSE);
    assert(Eris::getLogLevel() == Eris::LOG_VERBOSE);

    Eris::setLogLevel(Eris::LOG_DEBUG);
    assert(Eris::getLogLevel() == Eris::LOG_DEBUG);

    return 0;
}
