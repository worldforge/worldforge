// quaternion_test.cpp (Quaternion test functions)
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
// Created: 2002-1-20

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "const.h"
#include "vector.h"
#include "rotmatrix_funcs.h"
#include "quaternion.h"
#include "stream.h"
#include <vector>

#include "general_test.h"

#include <cmath>

using namespace WFMath;

void test_quaternion(const Quaternion& q)
{
  std::cout << "Testing " << q << std::endl;

  test_general(q);

  Quaternion q2;

  q2.identity();

  std::cout << "q = " << q << std::endl;
  std::cout << "q2 = " << q2 << std::endl;
  std::cout << "q  * q2= " << q * q2 << std::endl;
  std::cout << "q2 * q = " << q2 * q << std::endl;

  assert(q == q * q2);
  assert(q == q2 * q);
  assert(q == q / q2);

  q2 /= q;

  std::cout << "q = " << q << std::endl;
  std::cout << "q2 = " << q2 << std::endl;

  assert(q * q2 == Quaternion().identity());

  q2 *= q;

  std::cout << "q2 = " << q2 << std::endl;

  assert(q2 == Quaternion().identity());

  Quaternion q3(0, 5.0/12);

  RotMatrix<3> m(q);

  // Check orthogonality of created matrix

  for(int i = 0; i < 3; ++i) {
    for(int j = i; j < 3; ++j) {
      CoordType dot_sum = 0;
      for(int k = 0; k < 3; ++k)
        dot_sum += m.elem(i, k) * m.elem(j, k);
//      std::cout << '(' << i << ',' << j << ") dot_sum == " << dot_sum << std::endl;
      assert(Equal(dot_sum, (i == j) ? 1 : 0));
    }
  }

  Quaternion q4;

  assert(q4.fromRotMatrix(m));

//  std::cout << m << std::endl << q4 << std::endl;

  assert(q4 == q); //FIXME 2->1 Quaternion->RotMatrix<3> mapping

  Vector<3> v(1, 2, 3), v2(v);

  v.rotate(q);
  v.rotate(q2 / q);

  assert(v2 == v);

//  std::cout << v << std::endl << v2 << std::endl;

  v.rotate(q);
  v2 = Prod(v2, m);

//  std::cout << v << std::endl << v2 << std::endl;

  assert(v == v2);

  CoordType s(q.scalar());

  assert(Equal(s * s + q.vector().sqrMag(), 1));

  Quaternion q_other(1, 2, 3, 4);

  v.rotate(q).rotate(q_other);
  v2.rotate(q * q_other);

  assert(v == v2);
}

int main()
{
  Quaternion q(Vector<3>(1, 3, -sqrt(0.7)), .3);

  test_quaternion(q);

  return 0;
}
