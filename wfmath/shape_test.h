// shape_test.h (generic shape interface test functions)
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
// Created: 2001-1-6

#ifndef WFMATH_SHAPE_TEST_H
#define WFMATH_SHAPE_TEST_H

#include "const.h"
#include "vector.h"
#include "rotmatrix.h"
#include "point.h"
#include "axisbox.h"
#include "ball.h"
#include "intersect.h"
#include "stream.h"

namespace WFMath {

template<int dim, template<int> class Shape>
void test_shape_no_rotate(const Shape<dim>& s)
{
  Shape<dim> s2 = s;

  size_t corners = s2.numCorners();

  Point<dim> p = s2.getCenter();
  Vector<dim> v;

  v.zero();
  v[0] = 1;
  s2.shift(v);

  for(size_t i = 0; i < corners; ++i) {
    s2.moveCornerTo(p, i);
    p = s2.getCorner(i);
  }

  s2.moveCenterTo(p);

  assert(s2 == s);

  AxisBox<dim> box = s.boundingBox();
  assert(Contains(box, s, false));
  Ball<dim> ball1 = s.boundingSphere(), ball2 = s.boundingSphereSloppy();
//  cout << ball1 << std::endl << ball2 << std::endl;
  assert(Contains(ball1, s, false));
  assert(Contains(ball2, ball1, false));
}

template<int dim, template<int> class Shape>
void test_shape(const Shape<dim>& s)
{
  test_shape_no_rotate(s);

  Shape<dim> s2 = s;
  RotMatrix<dim> m;
  Point<dim> p;
  size_t corners = s2.numCorners();

  if(dim >= 2)
    m.rotation(0, 1, numeric_constants<CoordType>::pi / 6);
  else
    m.identity();

  for(size_t i = 0; i < corners; ++i) {
    s2.rotateCorner(m, i);
    p = s2.getCorner(i);
  }

  s2.rotatePoint(m, p);
  s2.rotateCenter(m);
}

} // namespace WFMath

#endif // WFMATH_SHAPE_TEST_H
