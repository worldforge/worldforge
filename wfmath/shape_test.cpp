// -*-C++-*-
// shape_test.cpp (Shape<> derived classes test functions)
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

#include "vector.h"
#include "matrix.h"
#include "point.h"
#include "const.h"
#include "stream.h"
#include "shape.h"
#include "axisbox.h"
#include "ball.h"
#include "segment.h"
#include "rotbox.h"
#include <iostream>

using namespace WF::Math;

template<const int dim>
void test_shape(const Point<dim>& p1, const Point<dim>& p2)
{
  AxisBox<dim> box(p1, p2);

  cout << "Testing " << box << std::endl;

  Ball<dim> ball(p1, 1);

  cout << "Testing " << ball << std::endl;

  Segment<dim> seg(p1, p2);

  cout << "Testing " << seg << std::endl;

  RotBox<dim> rbox(p1, p2 - p1, RotMatrix<dim>().rotation(0, 1, WFMATH_CONST_PI / 6));

  cout << "Testing " << rbox << std::endl;

  // FIXME
}

int main()
{
  test_shape(Point<2>(1, -1), Point<2>().origin());
  test_shape(Point<3>(1, -1, WFMATH_CONST_SQRT2), Point<3>().origin());

  return 0;
}
