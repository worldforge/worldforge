// -*-C++-*-
// atlas_tests.cpp (WFMath/Atlas Message conversion test code)
//
//  The WorldForge Project
//  Copyright (C) 2001  The WorldForge Project
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
// Created: 2001-12-12

#include<Atlas/Message/Object.h>
#include "atlasconv.h"

using namespace WF::Math;

template<class C>
void atlas_test(const C& c)
{
  Atlas::Message::Object a = ToAtlas(c);
  C out;
  assert(FromAtlas(out, a));
  assert(c.isEqualTo(out, 1e-5)); // Only match to string precision
}

int main()
{
  Point<3> p(1, 0, WFMATH_CONST_SQRT2);

  atlas_test(p);

  return 0;
}
