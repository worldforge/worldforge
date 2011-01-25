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

/**
 * Test intersection between a square polygon and axis boxes at each of its four sides.
 */
void test_intersect()
{

  Polygon<2> p;

  p.addCorner(0, Point<2>(0, 0));
  p.addCorner(0, Point<2>(4, 0));
  p.addCorner(0, Point<2>(4, -4));
  p.addCorner(0, Point<2>(0, -4));
  p.isValid();

  AxisBox<2> a1(Point<2>(-1, -1), Point<2>(1, -3));
  std::cout << "Testing intersection of " << p << " and " << a1 << std::endl;
  assert(WFMath::Intersect(p, a1, false));

  AxisBox<2> a2(Point<2>(1, -5), Point<2>(2, -3));
  std::cout << "Testing intersection of " << p << " and " << a2 << std::endl;
  assert(WFMath::Intersect(p, a2, false));

  AxisBox<2> a3(Point<2>(5, -1), Point<2>(3, -3));
  std::cout << "Testing intersection of " << p << " and " << a3 << std::endl;
  assert(WFMath::Intersect(p, a3, false));

  AxisBox<2> a4(Point<2>(1, 1), Point<2>(2, -1));
  std::cout << "Testing intersection of " << p << " and " << a4 << std::endl;
  assert(WFMath::Intersect(p, a4, false));


  RotBox<2> r1(Point<2>(-1, -1), Vector<2>(2, -2), RotMatrix<2>().identity());
  std::cout << "Testing intersection of " << p << " and " << r1 << std::endl;
  assert(WFMath::Intersect(p, r1, false));

  RotBox<2> r2(Point<2>(1, -5), Vector<2>(1, 2), RotMatrix<2>().identity());
  std::cout << "Testing intersection of " << p << " and " << r2 << std::endl;
  assert(WFMath::Intersect(p, r2, false));

  RotBox<2> r3(Point<2>(5, -1), Vector<2>(-2, -2), RotMatrix<2>().identity());
  std::cout << "Testing intersection of " << p << " and " << r3 << std::endl;
  assert(WFMath::Intersect(p, r3, false));

  RotBox<2> r4(Point<2>(1, 1), Vector<2>(1, -2), RotMatrix<2>().identity());
  std::cout << "Testing intersection of " << p << " and " << r4 << std::endl;
  assert(WFMath::Intersect(p, r4, false));


}

/**
 * Test contains between a square polygon and axis boxes at each of its four sides.
 */
void test_contains()
{

  Polygon<2> p;

  p.addCorner(0, Point<2>(0, 0));
  p.addCorner(0, Point<2>(4, 0));
  p.addCorner(0, Point<2>(4, -4));
  p.addCorner(0, Point<2>(0, -4));
  p.isValid();

  AxisBox<2> a1(Point<2>(0.1, -3.9), Point<2>(0.2, -3.8));
  std::cout << "Testing intersection of " << p << " and " << a1 << std::endl;
  assert(WFMath::Contains(p, a1, false));

  AxisBox<2> a2(Point<2>(3.8, -3.9), Point<2>(3.9, -3.8));
  std::cout << "Testing intersection of " << p << " and " << a2 << std::endl;
  assert(WFMath::Contains(p, a2, false));

  AxisBox<2> a3(Point<2>(0.1, -0.2), Point<2>(0.2, -0.1));
  std::cout << "Testing intersection of " << p << " and " << a3 << std::endl;
  assert(WFMath::Contains(p, a3, false));

  AxisBox<2> a4(Point<2>(3.8, -0.2), Point<2>(3.9, -0.1));
  std::cout << "Testing intersection of " << p << " and " << a4 << std::endl;
  assert(WFMath::Contains(p, a4, false));


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

  test_intersect();

  test_contains();

  return 0;
}
