// polygon_test.cpp (Polygon<> test functions)
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
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2002-1-20

#include "const.h"
#include "vector.h"
#include "rotmatrix.h"
#include "point.h"
#include "polygon.h"
#include "polygon_intersect.h"
#include "stream.h"
#include <vector>

#include "general_test.h"
#include "shape_test.h"

using namespace WFMath;

template<const int dim>
void test_polygon(const Polygon<dim>& p)
{
  std::cout << "Testing " << p << std::endl;

  test_general(p);
  test_shape(p);

  // FIXME more tests

  // Just check that these compile
  Point<dim> point;
  point.setToOrigin();
  AxisBox<dim> a(point, point, true);
  Vector<dim> vec;
  vec.zero();
  RotMatrix<dim> mat;
  mat.identity();
  RotBox<dim> r(point, vec, mat);

  Intersect(p, a, false);
  Intersect(p, r, false);
}

int main()
{
  bool succ;

  Polygon<2> p2;

  succ = p2.addCorner(0, Point<2>(1, -1));
  assert(succ);
  succ = p2.addCorner(0, Point<2>(2, -1));
  assert(succ);
  succ = p2.addCorner(0, Point<2>(1, -3));
  assert(succ);

  test_polygon(p2);

  Polygon<3> p3;

  succ = p3.addCorner(0, Point<3>(1, -1, 5));
  assert(succ);
  succ = p3.addCorner(0, Point<3>(2, -1, sqrt(3.0/2)));
  assert(succ);
  succ = p3.addCorner(0, Point<3>(1, -3, 2.0/3));
  assert(succ);

  test_polygon(p3);

  return 0;
}
