// const_test.cpp (Equal() test functions)
//
//  The WorldForge Project
//  Copyright (C) 2002  The WorldForge Project
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2002-1-28

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "const.h"

#include <cassert>

using namespace WFMath;

// TestEqual() directly stolen from Numbers.cpp in Willow (thanks, Jesse!)

static void TestEqual()
{
        assert( Equal(1.0001, 1.0, 1.0e-3));
        assert(!Equal(1.0001, 1.0, 1.0e-5));

        assert( Equal(0.00010000, 0.00010002, 1.0e-3));
        assert(!Equal(0.00010000, 0.00010002, 1.0e-6));

        assert( Equal(1000100.0, 1000000.0, 1.0e-3));
        assert(!Equal(1000100.0, 1000000.0, 1.0e-6));
}

int main()
{
  TestEqual();

  return 0;
}
