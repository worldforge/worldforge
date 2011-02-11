// general_test.h (generic class interface test functions)
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
// Created: 2001-1-6

#ifndef WFMATH_GENERAL_TEST_H
#define WFMATH_GENERAL_TEST_H

#include "const.h"
#include "stream.h"
#include <string>
#include <iostream>

#include <cstdlib>

namespace WFMath {

template<class C>
void test_general(const C& c)
{
  C c1, c2(c); // Generic and copy constructors

  c1 = c; // operator=()

  assert(Equal(c1, c2));
  assert(c1 == c2);
  assert(!(c1 != c2));

  std::string s = ToString(c); // Uses operator<<() implicitly
  C c3;
  try {
    FromString(c3, s); // Uses operator>>() implicitly
  }
  catch(ParseError) {
    std::cerr << "Couldn't parse generated string: " << s << std::endl;
    abort();
  }

  // We lose precision in string conversion
  assert(Equal(c3, c, FloatMax(WFMATH_EPSILON, 1e-5)));
}

} // namespace WFMath

#endif // WFMATH_GENERAL_TEST_H
