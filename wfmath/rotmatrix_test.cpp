// rotmatrix_test.cpp (RotMatrix<> test functions)
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
void test_rotmatrix(const RotMatrix<dim>& m)
{
  std::cout << "Testing RotMatrix: " << m << std::endl;

  test_general(m);

  RotMatrix<dim> minv = m.inverse();

//  cout << "Inverse is: " << minv << std::endl;

  RotMatrix<dim> ident;

  ident.identity();

  RotMatrix<dim> try_ident;

  try_ident = ProdInv(m, m);

//  cout << "This should be the identity: " << try_ident << std::endl;

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      assert(Equal(ident.elem(i, j), try_ident.elem(i, j)));

  try_ident = Prod(m, minv);

//  cout << "This should be the identity: " << try_ident << std::endl;

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      assert(Equal(ident.elem(i, j), try_ident.elem(i, j)));

  std::string s_mat = ToString(m);

  RotMatrix<dim> str_m;

  FromString(str_m, s_mat);

//  cout << "After conversion through a string, the matrix is " << str_m << std::endl;

//  cout << "Element differences after conversion are: ";
  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      CoordType diff = m.elem(i, j) - str_m.elem(i, j);
//      cout << diff;
      assert(std::fabs(diff) < FloatMax(numeric_constants<CoordType>::epsilon(), 1e-6));
//      if(i < dim - 1 || j < dim - 1)
//        cout << ", ";
    }
  }
//  cout << std::endl;

  RotMatrix<dim> conv_ident = Prod(str_m, str_m.inverse());

//  cout << "Converted M * M^T identity check: " << conv_ident;
//  cout << std::endl;

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      assert(Equal(conv_ident.elem(i, j), (i == j) ? 1 : 0));

  // FIXME much more
}

int main()
{
  RotMatrix<2> m2;
  RotMatrix<3> m3;

  m2.rotation(numeric_constants<CoordType>::pi() / 6);
  m3.rotation(Vector<3>(1, 0, numeric_constants<CoordType>::sqrt2()),
              numeric_constants<CoordType>::pi() / 5);

  test_rotmatrix(m2);
  test_rotmatrix(m3);

  // FIXME toEuler(), fromEuler()

  return 0;
}
