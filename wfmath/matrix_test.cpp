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
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace WF::Math;

template<const int dim>
void test_matrix(const RotMatrix<dim>& m)
{
  cout << "Testing matrix: " << m.toString() << std::endl;

  RotMatrix<dim> minv = m.inverse();

//  cout << "Inverse is: " << minv.toString() << std::endl;

  RotMatrix<dim> ident; // Initialized to identity

  RotMatrix<dim> try_ident;

  try_ident = ProdInv(m, m);

//  cout << "This should be the identity: " << try_ident.toString() << std::endl;

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      assert(IsFloatEqual(ident.elem(i, j), try_ident.elem(i, j)));

  try_ident = Prod(m, minv);

//  cout << "This should be the identity: " << try_ident.toString() << std::endl;

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      assert(IsFloatEqual(ident.elem(i, j), try_ident.elem(i, j)));

  std::string s_mat = m.toString();

  RotMatrix<dim> str_m;

  if(!str_m.fromString(s_mat)) {
    cout << "Could not convert string back into matrix" << std::endl;
    exit(-1);
  }

//  cout << "After conversion through a string, the matrix is " << str_m.toString() << std::endl;

//  cout << "Element differences after conversion are: ";
  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      double diff = m.elem(i, j) - str_m.elem(i, j);
//      cout << diff;
      assert(fabs(diff) < WFMATH_STRING_EPSILON);
//      if(i < dim - 1 || j < dim - 1)
//        cout << ", ";
    }
  }
//  cout << std::endl;

  RotMatrix<dim> conv_ident = Prod(str_m, str_m.inverse());

//  cout << "Converted M * M^T identity check: " << conv_ident.toString();
//  cout << std::endl;

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      assert(IsFloatEqual(conv_ident.elem(i, j), (i == j) ? 1 : 0));
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
