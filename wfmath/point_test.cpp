// point_test.cpp (Point<> test functions)
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
// Created: 2001-12-12

#include "const.h"
#include "vector.h"
#include "point.h"
#include "axisbox.h"
#include "ball.h"
#include "stream.h"

#include "general_test.h"
#include "shape_test.h"

#include <vector>
#include <list>

using namespace WFMath;

template<const int dim>
void test_point(const Point<dim>& p)
{
  std::cout << "Testing point: " << p << std::endl;

  test_general(p);
  test_shape(p);

  std::vector<Point<dim> > pvec;
  std::list<CoordType> clist;

  // assert(!Barycenter(pvec).isValid());

  assert(!Barycenter(pvec, clist).isValid());

  pvec.push_back(p);
  assert(p == Barycenter(pvec));
  clist.push_back(5);
  assert(p == Barycenter(pvec, clist));

  assert(p == p + (p - p));

  // FIXME more tests
}

int main()
{
  test_point(Point<2>(1, -1));
  test_point(Point<3>(1, -1, Sqrt2));

  Point<2> zero2 = Point<2>::ZERO();
  assert(zero2.x() == 0 && zero2.y() == 0);
  Point<3> zero3 = Point<3>::ZERO();
  assert(zero3.x() == 0 && zero3.y() == 0 && zero3.z() == 0);


  return 0;
}
