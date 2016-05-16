// intstring_test.cpp (IntToString() test functions)
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
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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

#include "int_to_string.h"
#include "randgen.h"

#include <climits>
#include <cstdlib>

#include <cassert>

using namespace WFMath;

static void TestConvert()
{
  for(unsigned i = 0; i < 100; ++i) {
    unsigned long val = MTRand::instance.randInt();
    assert(strtoul(IntToString(val).c_str(), 0, 0) == val);
    // This assignment changes the value, but we just want a
    // random number, so we don't care. Large unsigned ints will
    // provide us negative numbers for testing.
    long val2 = (long) val;
    assert(atol(IntToString(val2).c_str()) == val2);
  }
}

int main()
{
  TestConvert();

  return 0;
}
