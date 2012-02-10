// shape_test.cpp (basic shape test functions)
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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "const.h"
#include "vector.h"
#include "rotmatrix.h"
#include "point.h"
#include "axisbox.h"
#include "ball.h"
#include "segment.h"
#include "rotbox.h"
#include "intersect.h"
#include "stream.h"
#include <vector>

#include "general_test.h"
#include "shape_test.h"

#include <cmath>

using namespace WFMath;

template<int dim>
void test_shape(const Point<dim>& p1, const Point<dim>& p2)
{
  CoordType sqr_dist = SquaredDistance(p1, p2);

  AxisBox<dim> box(p1, p2), tmp;

  std::cout << "Testing " << box << std::endl;

  test_general(box);
  test_shape_no_rotate(box);

  tmp = Union(box, box);
  assert(tmp == box);
  assert(Intersection(box, box, tmp));
  assert(tmp == box);
  std::vector<AxisBox<dim> > boxvec;
  boxvec.push_back(box);
  assert(box == BoundingBox(boxvec));

  assert(Intersect(box, p1, false));
  assert(!Intersect(box, p1, true));

  assert(Intersect(box, box, false));
  assert(Intersect(box, box, true));
  assert(Contains(box, box, false));
  assert(!Contains(box, box, true));

  Ball<dim> ball(p1, 1);

  std::cout << "Testing " << ball << std::endl;

  test_general(ball);
  test_shape(ball);

  assert(Intersect(ball, p1, false));
  assert(Intersect(ball, p1, true));

  assert(Intersect(ball, box, false));
  assert(Intersect(ball, box, true));
  assert(Contains(ball, box, false) == (sqr_dist <= 1));
  assert(Contains(ball, box, true) == (sqr_dist < 1));
  assert(!Contains(box, ball, false));
  assert(!Contains(box, ball, true));

  assert(Intersect(ball, ball, false));
  assert(Intersect(ball, ball, true));
  assert(Contains(ball, ball, false));
  assert(!Contains(ball, ball, true));

  Segment<dim> seg(p1, p2);

  std::cout << "Testing " << seg << std::endl;

  test_general(seg);
  test_shape(seg);

  assert(Intersect(seg, p1, false));
  assert(!Intersect(seg, p1, true));

  assert(Intersect(seg, box, false));
  assert(Intersect(seg, box, true));
  assert(!Contains(seg, box, false));
  assert(!Contains(seg, box, true));
  assert(Contains(box, seg, false));
  assert(!Contains(box, seg, true));

  assert(Intersect(seg, ball, false));
  assert(Intersect(seg, ball, true));
  assert(!Contains(seg, ball, false));
  assert(!Contains(seg, ball, true));
  assert(Contains(ball, seg, false) == (sqr_dist <= 1));
  assert(Contains(ball, seg, true) == (sqr_dist < 1));

  assert(Intersect(seg, seg, false));
  assert(Intersect(seg, seg, true));
  assert(Contains(seg, seg, false));
  assert(!Contains(seg, seg, true));

  RotBox<dim> rbox(
      p1,
      p2 - p1,
      RotMatrix<dim>().rotation(0, 1, numeric_constants<CoordType>::pi() / 6));

  std::cout << "Testing " << rbox << std::endl;

  test_general(rbox);
  test_shape(rbox);

  assert(Intersect(rbox, p1, false));
  assert(!Intersect(rbox, p1, true));

  assert(Intersect(rbox, box, false));
  assert(Intersect(rbox, box, true));
  assert(!Contains(rbox, box, false));
  assert(!Contains(rbox, box, true));
  assert(!Contains(box, rbox, false));
  assert(!Contains(box, rbox, true));

  assert(Intersect(rbox, ball, false));
  assert(Intersect(rbox, ball, true));
  assert(!Contains(rbox, ball, false));
  assert(!Contains(rbox, ball, true));
  assert(Contains(ball, rbox, false) == (sqr_dist <= 1));
  assert(Contains(ball, rbox, true) == (sqr_dist < 1));

  assert(Intersect(rbox, seg, false));
  // The next function may either succeed or fail, depending on the points passed.
  Intersect(rbox, seg, true);
  assert(!Contains(rbox, seg, false));
  assert(!Contains(rbox, seg, true));
  assert(!Contains(seg, rbox, false));
  assert(!Contains(seg, rbox, true));

  assert(Intersect(rbox, rbox, false));
  assert(Intersect(rbox, rbox, true));
  assert(Contains(rbox, rbox, false));
  assert(!Contains(rbox, rbox, true));

  // FIXME more tests
}

int main()
{
  test_shape(Point<2>(1, -1),
             Point<2>().setToOrigin());
  test_shape(Point<3>(1, -1, numeric_constants<CoordType>::sqrt2()),
             Point<3>().setToOrigin());

  return 0;
}
