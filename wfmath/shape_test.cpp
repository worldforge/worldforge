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

using namespace WFMath;

template<const int dim>
void test_shape(const Point<dim>& p1, const Point<dim>& p2)
{
  CoordType sqr_dist = SquaredDistance(p1, p2);

  AxisBox<dim> box(p1, p2), tmp;

  cout << "Testing " << box << std::endl;

  test_general(box);
  test_shape_no_rotate(box);

  tmp = Union(box, box);
  assert(tmp == box);
  assert(Intersection(box, box, tmp));
  assert(tmp == box);
  vector<AxisBox<dim> > boxvec;
  boxvec.push_back(box);
  assert(box == BoundingBox(boxvec));

  assert(Intersect(box, p1));
  assert(!IntersectProper(box, p1));

  assert(Intersect(box, box));
  assert(IntersectProper(box, box));
  assert(Contains(box, box));
  assert(!ContainsProper(box, box));

  Ball<dim> ball(p1, 1);

  cout << "Testing " << ball << std::endl;

  test_general(ball);
  test_shape(ball);

  assert(Intersect(ball, p1));
  assert(IntersectProper(ball, p1));

  assert(Intersect(ball, box));
  assert(IntersectProper(ball, box));
  assert(Contains(ball, box) == (sqr_dist <= 1));
  assert(ContainsProper(ball, box) == (sqr_dist < 1));
  assert(!Contains(box, ball));
  assert(!ContainsProper(box, ball));

  assert(Intersect(ball, ball));
  assert(IntersectProper(ball, ball));
  assert(Contains(ball, ball));
  assert(!ContainsProper(ball, ball));

  Segment<dim> seg(p1, p2);

  cout << "Testing " << seg << std::endl;

  test_general(seg);
  test_shape(seg);

  assert(Intersect(seg, p1));
  assert(!IntersectProper(seg, p1));

  assert(Intersect(seg, box));
  assert(IntersectProper(seg, box));
  assert(!Contains(seg, box));
  assert(!ContainsProper(seg, box));
  assert(Contains(box, seg));
  assert(!ContainsProper(box, seg));

  assert(Intersect(seg, ball));
  assert(IntersectProper(seg, ball));
  assert(!Contains(seg, ball));
  assert(!ContainsProper(seg, ball));
  assert(Contains(ball, seg) == (sqr_dist <= 1));
  assert(ContainsProper(ball, seg) == (sqr_dist < 1));

  assert(Intersect(seg, seg));
  assert(IntersectProper(seg, seg));
  assert(Contains(seg, seg));
  assert(!ContainsProper(seg, seg));

  RotBox<dim> rbox(p1, p2 - p1, RotMatrix<dim>().rotation(0, 1, Pi / 6));

  cout << "Testing " << rbox << std::endl;

  test_general(rbox);
  test_shape(rbox);

  assert(Intersect(rbox, p1));
  assert(!IntersectProper(rbox, p1));

//  assert(Intersect(rbox, box));
//  assert(IntersectProper(rbox, box));
  assert(!Contains(rbox, box));
  assert(!ContainsProper(rbox, box));
  assert(!Contains(box, rbox));
  assert(!ContainsProper(box, rbox));

  assert(Intersect(rbox, ball));
  assert(IntersectProper(rbox, ball));
  assert(!Contains(rbox, ball));
  assert(!ContainsProper(rbox, ball));
  assert(Contains(ball, rbox) == (sqr_dist <= 1));
  assert(ContainsProper(ball, rbox) == (sqr_dist < 1));

  assert(Intersect(rbox, seg));
  // The next function may either succeed or fail, depending on the points passed.
  IntersectProper(rbox, seg);
  assert(!Contains(rbox, seg));
  assert(!ContainsProper(rbox, seg));
  assert(!Contains(seg, rbox));
  assert(!ContainsProper(seg, rbox));

//  assert(Intersect(rbox, rbox));
//  assert(IntersectProper(rbox, rbox));
  assert(Contains(rbox, rbox));
  assert(!ContainsProper(rbox, rbox));

  // FIXME more tests
}

int main()
{
  test_shape(Point<2>(1, -1), Point<2>().setToOrigin());
  test_shape(Point<3>(1, -1, Sqrt2), Point<3>().setToOrigin());

  return 0;
}
