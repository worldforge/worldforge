// -*-C++-*-
// vector_test.cpp (Vector<> test functions)
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
// Created: 2001-12-7

#include "vector.h"
#include "vector_funcs.h"
#include "matrix_funcs.h"
#include "const.h"
#include <assert.h>
#include "stream_funcs.h"
#include <iostream>

using namespace WF::Math;

template<const int len>
void test_vector(const Vector<len>& v)
{
  cout << "Testing vector: " << v << std::endl;

  double sqr_mag = v.sqrMag();

  assert(IsFloatEqual(sqrt(sqr_mag), v.mag()));

  assert(IsFloatEqual(sqr_mag, Dot(v, v)));

  Vector<len> v1, v2;

  v1.zero();
  v1[0] = 1;
  for(int i = 0; i < len; ++i)
    v2[i] = 1;

  const int steps = 8;

  Vector<len> vcopy = v;

  for(int j = 0; j < len; ++j) {
    for(int i = 0; i < steps; ++i) {
      vcopy.rotate(v1, v2, 2 * WFMATH_CONST_PI / steps);
//      cout << vcopy << std::endl;
      assert(IsFloatEqual(sqr_mag, vcopy.sqrMag()));
    }

    for(int i = 0; i < len; ++i)
      assert(IsFloatEqual(v[i], vcopy[i]));

    v2 -= v1 / 2;

    int k = (j < len - 1) ? j + 1 : 0;
    v1.rotate(j, k, WFMATH_CONST_PI / 2);
  }

  v2 *= 2;

  for(int i = 0; i < len; ++i)
    assert(IsFloatEqual(v2[i], 1));

  double check = Dot((v1 + v2) * 5 - v2 / 4, 2 * v2);

  assert(IsFloatEqual((10.0 + len * 38.0 / 4.0), check));

  double check_mag = v.sloppyMag() / v.mag();

  assert(1 - WFMATH_EPSILON < check_mag);
  assert(check_mag < Vector<len>::sloppyMagMax() + WFMATH_EPSILON);
}

int main()
{
  test_vector(Vector<2>(1, -1));
  test_vector(Vector<3>(1, -1, WFMATH_CONST_SQRT2));

  assert((Vector<3>(1, 0, 0).rotate(Cross(Vector<3>(1, 0, 0), Vector<3>(0, 1, 0)),
	 WFMATH_CONST_PI / 2) - Vector<3>(0, 1, 0)).sqrMag() < WFMATH_EPSILON);

  return 0;
}
