// -*-C++-*-
// matrix_test.cpp (Matrix<> test functions)
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
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2001-12-7

#include "vector.h"
#include "vector_funcs.h"
#include "matrix.h"
#include "matrix_funcs.h"
#include "const.h"
#include <assert.h>
#include "stream_funcs.h"
#include <iostream>

using namespace WF::Math;

//TODO tests for non-square matrices

template<const int size>
void test_matrix(const Matrix<size>& m)
{
  cout << "Testing matrix: " << m << std::endl;

  cout << "Transpose is: " << m.transpose() << std::endl;

  Matrix<size> minv = m.inverse();

  double mdet = m.determinant(), minvdet = minv.determinant();

  cout << "Inverse is: " << minv << std::endl;

  assert(fabs(mdet * minvdet - 1) < WFMATH_EPSILON);

  Matrix<size> nothing;

  nothing.identity();

  nothing -= m * minv;

  cout << "This should be zero: " << nothing << std::endl;

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      assert(fabs(nothing.elem(i, j)) < WFMATH_EPSILON);
}

int main()
{
  Matrix<2> m2;
  Matrix<3> m3;

  m2.identity();
  m2.elem(1, 0) = 1;

  test_matrix(m2);

  m3.identity();
  m3.elem(1, 0) = 1;
  m3.elem(0, 2) = WFMATH_CONST_SQRT2;
  m3.elem(2, 0) = WFMATH_CONST_SQRT3;

  test_matrix(m3);

  return 0;
}
