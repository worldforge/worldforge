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
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
#include <sstream>

using namespace WF::Math;

template<const int size>
void test_matrix(const RotMatrix<size>& m)
{
  cout << "Testing matrix: " << m << std::endl;

  RotMatrix<size> minv = m.inverse();

  cout << "Inverse is: " << minv << std::endl;

  RotMatrix<size> ident; // Initialized to identity

  RotMatrix<size> try_ident = ProdInv(m, m);

  cout << "This should be the identity: " << try_ident << std::endl;

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      assert(IsFloatEqual(ident.elem(i, j), try_ident.elem(i, j)));

  std::string s_mat = m.toString();

  RotMatrix<size> str_m;

  if(!str_m.fromString(s_mat)) {
    cout << "Could not convert string back into matrix" << std::endl;
    exit(-1);
  }

  cout << "After conversion through a string, the matrix is " << str_m << std::endl;

  cout << "Element differences after conversion are: ";
  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
      cout << m.elem(i, j) - str_m.elem(i, j);
      if(i < size - 1 || j < size - 1)
        cout << ", ";
    }
  }
  cout << std::endl;

  cout << "Converted M * M^T identity check: " << Prod(str_m, str_m.inverse());
  cout << std::endl;
}

int main()
{
  RotMatrix<2> m2;
  RotMatrix<3> m3;

  m2.rotation(WFMATH_CONST_PI / 6);
  m3.rotation(Vector<3>(1, 0, WFMATH_CONST_SQRT2), WFMATH_CONST_PI / 5);

  test_matrix(m2);
  test_matrix(m3);

  return 0;
}
