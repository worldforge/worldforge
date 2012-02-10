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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "const.h"
#include "vector.h"
#include "rotmatrix.h"
#include "stream.h"

#include "general_test.h"

#include <cmath>

using namespace WFMath;

template<int dim>
void test_vector(const Vector<dim>& v)
{
  std::cout << "Testing vector: " << v << std::endl;

  test_general(v);

  CoordType sqr_mag = v.sqrMag();

  assert(Equal(std::sqrt(sqr_mag), v.mag()));

  assert(Equal(sqr_mag, Dot(v, v)));

  Vector<dim> v1, v2;

  v1.zero();
  v1[0] = 1;
  for(int i = 0; i < dim; ++i)
    v2[i] = 1;

  const int steps = 8;

  Vector<dim> vcopy = v;

  for(int j = 0; j < dim; ++j) {
    for(int i = 0; i < steps; ++i) {
      vcopy.rotate(v1, v2, 2 * numeric_constants<CoordType>::pi() / steps);
//      std::cout << vcopy << std::endl;
      assert(Equal(sqr_mag, vcopy.sqrMag()));
    }

    for(int i = 0; i < dim; ++i)
      assert(Equal(v[i], vcopy[i]));

    v2 -= v1 / 2; // operator-=(), operator/()

    int k = (j < dim - 1) ? j + 1 : 0;
    v1.rotate(j, k, numeric_constants<CoordType>::pi() / 2);
  }

  v2 *= 2; // operator*=()

  for(int i = 0; i < dim; ++i)
    assert(Equal(v2[i], 1.0f));

  // operator+(), operator-(), operator*() (pre and post), operator/()
  CoordType check = Dot((v1 + v2) * 5 - v2 / 4, 2 * v2);
  assert(Equal((10.0f + dim * 38.0f / 4.0f), check));

  Vector<dim> v3 = v;
  v3 += v;
  v3 *= 2;
  v3 -= 2 * v;
  v3 /= 2;
  assert(v == v3);
  for(int i = 0; i < dim; ++i)
    assert(v[i] == v3[i]); // const and non-const operator[]()

  CoordType check_mag = v.sloppyMag() / v.mag();

  assert(1 - WFMATH_EPSILON < check_mag);
  assert(check_mag < Vector<dim>::sloppyMagMax() + WFMATH_EPSILON);

  // Still need Dot(), Angle(), normalize(), mirror()
}

int main()
{
  Vector<2> v2(1, -1);
  Vector<3> v3(1, -1, numeric_constants<CoordType>::sqrt2());

  test_vector(v2);
  test_vector(v3);
  
  Vector<2> zero2 = Vector<2>::ZERO();
  assert(zero2.x() == 0 && zero2.y() == 0);
  Vector<3> zero3 = Vector<3>::ZERO();
  assert(zero3.x() == 0 && zero3.y() == 0 && zero3.z() == 0);

  assert(v2.sloppyMag() / v2.mag() < Vector<2>::sloppyMagMax());
  assert(v3.sloppyMag() / v3.mag() < Vector<3>::sloppyMagMax());

  v2.sloppyNorm(1);
  v3.sloppyNorm(1);

  assert((Vector<3>(1, 0, 0).rotate(Cross(Vector<3>(1, 0, 0), Vector<3>(0, 1, 0)),
	 numeric_constants<CoordType>::pi() / 2) - Vector<3>(0, 1, 0)).sqrMag()
	 < WFMATH_EPSILON * WFMATH_EPSILON); 

  // Need 2D+3D stuff

  return 0;
}
