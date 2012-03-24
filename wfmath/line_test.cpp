// line_test.cpp (Line<> test functions)
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
// Created: 2012-01-26

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "const.h"
#include "vector.h"
#include "point.h"
#include "axisbox.h"
#include "ball.h"
#include "stream.h"
#include "line.h"

#include "general_test.h"
#include "shape_test.h"

#include <vector>
#include <list>

using namespace WFMath;

template<int dim>
void test_line(const Line<dim>& p)
{
  std::cout << "Testing line: " << p << std::endl;

  // test_general(p);
  // test_shape(p);

  assert(p == p);

  // FIXME more tests
}

void test_modify()
{
  Line<2> line;
  line.addCorner(0, Point<2>(2, 2));
  line.addCorner(0, Point<2>(0, 0));
  
  assert(line.getCorner(0) == Point<2>(0, 0));

  line.moveCorner(0, Point<2>(1, 1));
  assert(line.getCorner(0) == Point<2>(1, 1));
}

int main()
{
  Line<2> line2_1;
  assert(!line2_1.isValid());
  line2_1.addCorner(0, Point<2>(0, 0));
  line2_1.addCorner(0, Point<2>(4, 0));
  line2_1.addCorner(0, Point<2>(4, -4));
  line2_1.addCorner(0, Point<2>(0, -4));
  assert(line2_1.isValid());

  test_line(line2_1);

  assert(line2_1 == line2_1);

  Line<2> line2_2;
  assert(!line2_2.isValid());
  line2_2.addCorner(0, Point<2>(0, 0));
  line2_2.addCorner(0, Point<2>(4, -4));
  line2_2.addCorner(0, Point<2>(4, 0));
  line2_2.addCorner(0, Point<2>(0, -4));
  assert(line2_2.isValid());

  // Check in-equality
  assert(line2_1 != line2_2);

  // Check assignment
  line2_2 = line2_1;

  // Check equality
  assert(line2_1 == line2_2);

  Line<2> line2_3;
  assert(!line2_3.isValid());
  assert(line2_3 != line2_1);
  line2_3 = line2_1;
  assert(line2_3 == line2_1);

  // Check bounding box calculation
  assert(Equal(line2_1.boundingBox(),
               AxisBox<2>(Point<2>(0, -4), Point<2>(4, 0))));

  Line<2> line2_4(line2_1);

  assert(line2_1 == line2_4);

  Line<3> line3;

  test_line(line3);

  test_modify();

  return 0;
}
