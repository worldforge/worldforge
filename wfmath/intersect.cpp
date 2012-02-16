// intersect.cpp (Backends to intersection functions)
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
//

// Author: Ron Steinke

#include "intersect.h"

#include <cmath>

#include <cassert>

namespace WFMath {

// force a bunch of instantiations

template bool Intersect<2>(const Point<2>&, const Point<2>&, bool);
template bool Intersect<3>(const Point<3>&, const Point<3>&, bool);
template bool Contains<2>(const Point<2>&, const Point<2>&, bool);
template bool Contains<3>(const Point<3>&, const Point<3>&, bool);

template bool Intersect<Point<2>,AxisBox<2> >(const Point<2>&, const AxisBox<2>&, bool);
template bool Intersect<Point<3>,AxisBox<3> >(const Point<3>&, const AxisBox<3>&, bool);
template bool Contains<2>(const Point<2>&, const AxisBox<2>&, bool);
template bool Contains<3>(const Point<3>&, const AxisBox<3>&, bool);
template bool Intersect<2>(const AxisBox<2>&, const Point<2>&, bool);
template bool Intersect<3>(const AxisBox<3>&, const Point<3>&, bool);
template bool Contains<2>(const AxisBox<2>&, const Point<2>&, bool);
template bool Contains<3>(const AxisBox<3>&, const Point<3>&, bool);

template bool Intersect<2>(const AxisBox<2>&, const AxisBox<2>&, bool);
template bool Intersect<3>(const AxisBox<3>&, const AxisBox<3>&, bool);
template bool Contains<2>(const AxisBox<2>&, const AxisBox<2>&, bool);
template bool Contains<3>(const AxisBox<3>&, const AxisBox<3>&, bool);

template bool Intersect<Point<2>,Ball<2> >(const Point<2>&, const Ball<2>&, bool);
template bool Intersect<Point<3>,Ball<3> >(const Point<3>&, const Ball<3>&, bool);
template bool Contains<2>(const Point<2>&, const Ball<2>&, bool);
template bool Contains<3>(const Point<3>&, const Ball<3>&, bool);
template bool Intersect<2>(const Ball<2>&, const Point<2>&, bool);
template bool Intersect<3>(const Ball<3>&, const Point<3>&, bool);
template bool Contains<2>(const Ball<2>&, const Point<2>&, bool);
template bool Contains<3>(const Ball<3>&, const Point<3>&, bool);

template bool Intersect<AxisBox<2>,Ball<2> >(const AxisBox<2>&, const Ball<2>&, bool);
template bool Intersect<AxisBox<3>,Ball<3> >(const AxisBox<3>&, const Ball<3>&, bool);
template bool Contains<2>(const AxisBox<2>&, const Ball<2>&, bool);
template bool Contains<3>(const AxisBox<3>&, const Ball<3>&, bool);
template bool Intersect<2>(const Ball<2>&, const AxisBox<2>&, bool);
template bool Intersect<3>(const Ball<3>&, const AxisBox<3>&, bool);
template bool Contains<2>(const Ball<2>&, const AxisBox<2>&, bool);
template bool Contains<3>(const Ball<3>&, const AxisBox<3>&, bool);

template bool Intersect<2>(const Ball<2>&, const Ball<2>&, bool);
template bool Intersect<3>(const Ball<3>&, const Ball<3>&, bool);
template bool Contains<2>(const Ball<2>&, const Ball<2>&, bool);
template bool Contains<3>(const Ball<3>&, const Ball<3>&, bool);

template bool Intersect<Point<2>,Segment<2> >(const Point<2>&, const Segment<2>&, bool);
template bool Intersect<Point<3>,Segment<3> >(const Point<3>&, const Segment<3>&, bool);
template bool Contains<2>(const Point<2>&, const Segment<2>&, bool);
template bool Contains<3>(const Point<3>&, const Segment<3>&, bool);
template bool Intersect<2>(const Segment<2>&, const Point<2>&, bool);
template bool Intersect<3>(const Segment<3>&, const Point<3>&, bool);
template bool Contains<2>(const Segment<2>&, const Point<2>&, bool);
template bool Contains<3>(const Segment<3>&, const Point<3>&, bool);

template bool Intersect<AxisBox<2>,Segment<2> >(const AxisBox<2>&, const Segment<2>&, bool);
template bool Intersect<AxisBox<3>,Segment<3> >(const AxisBox<3>&, const Segment<3>&, bool);
template bool Contains<2>(const AxisBox<2>&, const Segment<2>&, bool);
template bool Contains<3>(const AxisBox<3>&, const Segment<3>&, bool);
template bool Intersect<2>(const Segment<2>&, const AxisBox<2>&, bool);
template bool Intersect<3>(const Segment<3>&, const AxisBox<3>&, bool);
template bool Contains<2>(const Segment<2>&, const AxisBox<2>&, bool);
template bool Contains<3>(const Segment<3>&, const AxisBox<3>&, bool);

template bool Intersect<Ball<2>,Segment<2> >(const Ball<2>&, const Segment<2>&, bool);
template bool Intersect<Ball<3>,Segment<3> >(const Ball<3>&, const Segment<3>&, bool);
template bool Contains<2>(const Ball<2>&, const Segment<2>&, bool);
template bool Contains<3>(const Ball<3>&, const Segment<3>&, bool);
template bool Intersect<2>(const Segment<2>&, const Ball<2>&, bool);
template bool Intersect<3>(const Segment<3>&, const Ball<3>&, bool);
template bool Contains<2>(const Segment<2>&, const Ball<2>&, bool);
template bool Contains<3>(const Segment<3>&, const Ball<3>&, bool);

template bool Intersect<2>(const Segment<2>&, const Segment<2>&, bool);
template bool Intersect<3>(const Segment<3>&, const Segment<3>&, bool);
template bool Contains<2>(const Segment<2>&, const Segment<2>&, bool);
template bool Contains<3>(const Segment<3>&, const Segment<3>&, bool);

template bool Intersect<Point<2>,RotBox<2> >(const Point<2>&, const RotBox<2>&, bool);
template bool Intersect<Point<3>,RotBox<3> >(const Point<3>&, const RotBox<3>&, bool);
template bool Contains<2>(const Point<2>&, const RotBox<2>&, bool);
template bool Contains<3>(const Point<3>&, const RotBox<3>&, bool);
template bool Intersect<2>(const RotBox<2>&, const Point<2>&, bool);
template bool Intersect<3>(const RotBox<3>&, const Point<3>&, bool);
template bool Contains<2>(const RotBox<2>&, const Point<2>&, bool);
template bool Contains<3>(const RotBox<3>&, const Point<3>&, bool);

template bool Intersect<AxisBox<2>,RotBox<2> >(const AxisBox<2>&, const RotBox<2>&, bool);
template bool Intersect<AxisBox<3>,RotBox<3> >(const AxisBox<3>&, const RotBox<3>&, bool);
template bool Contains<2>(const AxisBox<2>&, const RotBox<2>&, bool);
template bool Contains<3>(const AxisBox<3>&, const RotBox<3>&, bool);
template bool Contains<2>(const RotBox<2>&, const AxisBox<2>&, bool);
template bool Contains<3>(const RotBox<3>&, const AxisBox<3>&, bool);

template bool Intersect<Ball<2>,RotBox<2> >(const Ball<2>&, const RotBox<2>&, bool);
template bool Intersect<Ball<3>,RotBox<3> >(const Ball<3>&, const RotBox<3>&, bool);
template bool Contains<2>(const Ball<2>&, const RotBox<2>&, bool);
template bool Contains<3>(const Ball<3>&, const RotBox<3>&, bool);
template bool Intersect<2>(const RotBox<2>&, const Ball<2>&, bool);
template bool Intersect<3>(const RotBox<3>&, const Ball<3>&, bool);
template bool Contains<2>(const RotBox<2>&, const Ball<2>&, bool);
template bool Contains<3>(const RotBox<3>&, const Ball<3>&, bool);

template bool Intersect<Segment<2>,RotBox<2> >(const Segment<2>&, const RotBox<2>&, bool);
template bool Intersect<Segment<3>,RotBox<3> >(const Segment<3>&, const RotBox<3>&, bool);
template bool Contains<2>(const Segment<2>&, const RotBox<2>&, bool);
template bool Contains<3>(const Segment<3>&, const RotBox<3>&, bool);
template bool Intersect<2>(const RotBox<2>&, const Segment<2>&, bool);
template bool Intersect<3>(const RotBox<3>&, const Segment<3>&, bool);
template bool Contains<2>(const RotBox<2>&, const Segment<2>&, bool);
template bool Contains<3>(const RotBox<3>&, const Segment<3>&, bool);

template bool Intersect<2>(const RotBox<2>&, const RotBox<2>&, bool);
template bool Intersect<3>(const RotBox<3>&, const RotBox<3>&, bool);
template bool Contains<2>(const RotBox<2>&, const RotBox<2>&, bool);
template bool Contains<3>(const RotBox<3>&, const RotBox<3>&, bool);

// The 2d implementation was inspired as a simplification of the 3d.
// It used the fact that two not-similarly-oriented rectangles a and b
// intersect each other if and only if a's bounding box in b's coordinate
// system intersects b, and vice versa.
//
// To see this, it is only necessary to consider the bounding box intersection
// => intersection side of the assertion, since if the rectangles intersect,
// clearly their bounding boxes will as well. Let B(a) be the bounding box of
// a in b's coordinate system, and A(b) be the bounding box of b in a's coordinate
// system. Let the symbol ^ denote intersection. The thing we need to prove is:
//
// a ^ A(b) && b ^ B(a) => a ^ b
//
// I will discuss the equivalent statement
//
// a ^ A(b) && !(a ^ b) => !(b ^ B(a))
//
// If a intersects b's bounding box, but does not intersect b,
// the intersection of a and A(b) is a rectangle which lies in
// one of the four corners of the region A(b) - b (the set of all
// points which lie in the bounding box, but not in b). Without
// loss of generality, let a ^ A(b) lie in the lower left corner
// of A(b) - b. This is a triangular region, two of whose sides
// are parts of edges of A(b), the third being a side of b.
// Construct a line parallel to the side of b, passing between
// b and a ^ A(b). This line never intersects b, since b is a rectangle.
// It also never intersects a, since it passes above and to the
// right of the upper right corner of a. It also never intersects
// B(a), since the upper right side of B(a) is parallel to the
// line, but passes through the upper right corner of a. Thus,
// this line separates b from B(a), and they do not intersect. QED.

template<>
bool Intersect<2>(const RotBox<2>& r, const AxisBox<2>& b, bool proper)
{
  const AxisBox<2> b2 = r.boundingBox();
  if(!Intersect(b2, b, proper))
    return false;

  RotMatrix<2> m = r.m_orient.inverse();

  const AxisBox<2> b3 = RotBox<2>(Point<2>(b.m_low).rotate(m, r.m_corner0),
                             b.m_high - b.m_low, m).boundingBox();
  const AxisBox<2> b4(r.m_corner0, r.m_corner0 + r.m_size);
  return Intersect(b3, b4, proper);
}

// The 3d implementation is based on the following theorem:
//
// Theorem:
//
// Two convex polyhedra do not intersect if and only if there exists a separating
// plane which is either parallel to the face of one polyhedron or which is
// parallel to at least one edge of each polyhedron.
//
// Found this in the abstract to the paper:
//
// Surface-to-surface intersection based on triangular parameter domain subdivision
// Ernst Huber
// Institute of Computer Graphics
// Vienna University of Technology
// A-1040 Vienna, Karlsplatz 13/186/1, Austria
//
// online postscript version of the abstract (where I got this from):
//
// http://www.cs.ubc.ca/conferences/CCCG/elec_proc/c48.ps.gz
//
// The paper gives a reference for the theorem (probably the one to really look at
// for proof/details):
//
// S. Gottschalk
// Separating Axis Theorem
// Technical Report TR96-024
// Department of Computer Science
// UNC Chapel Hill
// 1996

template<>
bool Intersect<3>(const RotBox<3>& r, const AxisBox<3>& b, bool proper)
{
  // Checking intersection of each with the bounding box of
  // the other in the coordinate system of the first will take care
  // of the "plane parallel to face" case

  const AxisBox<3> b2 = r.boundingBox();
  if(!Intersect(b2, b, proper))
    return false;

  RotMatrix<3> minv = r.m_orient.inverse();
  Vector<3> b_size = b.m_high - b.m_low;

  const AxisBox<3> b3 = RotBox<3>(Point<3>(b.m_low).rotate(minv, r.m_corner0),
                             b_size, minv).boundingBox();
  const AxisBox<3> b4(r.m_corner0, r.m_corner0 + r.m_size);
  if(!Intersect(b3, b4, proper))
    return false;

  // Now for the "plane parallel to at least one edge of each" case

  Vector<3> sep =  b.m_low - r.m_corner0;
  const RotMatrix<3> &m = r.m_orient;

  // Generate normals to the 9 possible separating planes

  for(int i = 0; i < 3; ++i) {
    // Generate edge vectors for the RotBox, ignore size, only care about direction
// Just access m_orient directly below instead of using r_vec
//    Vector<3> r_vec = m.row(i);

    for(int j = 0; j < 3; ++j) {
      Vector<3> axis;

      // Cross product, ignore size of b since we only care about direction

      switch(j) {
        case 0:
          axis[0] = 0;
          axis[1] = -m.elem(i, 2);
          axis[2] =  m.elem(i, 1);
          break;
        case 1:
          axis[0] =  m.elem(i, 2);
          axis[1] = 0;
          axis[2] = -m.elem(i, 0);
          break;
        case 2:
          axis[0] = -m.elem(i, 1);
          axis[1] =  m.elem(i, 0);
          axis[2] = 0;
          break;
        default:
          assert(false);
      }

      if(axis.sqrMag() < WFMATH_EPSILON * WFMATH_EPSILON) {
        // Parallel edges, this reduces to the 2d case above. We've already
        // checked the bounding box intersections, so we know they intersect.
        // We don't need to scale WFMATH_EPSILON, det(m_orient) = 1
        // essentially takes care of that.
        return true;
      }

      // Project both boxes on this axis, check for separating plane.

      // We only need to project two axes per box, the one parallel
      // to the plane doesn't contribute

      const int next[] = {1, 2, 0};
      CoordType val;
      CoordType b_low, b_high, r_low, r_high, dist;
      int k;

      // AxisBox projection

      k = next[j];

      val = axis[k] * b_size[k];

      if(val > 0) {
        b_high = val;
        b_low = 0;
      }
      else {
        b_low = val;
        b_high = 0;
      }

      k = next[k];

      val = axis[k] * b_size[k];

      if(val > 0)
        b_high += val;
      else
        b_low += val;

      // RotBox projection

      k = next[i];

      val = Dot(m.row(k), axis) * r.m_size[k];

      if(val > 0) {
        r_high = val;
        r_low = 0;
      }
      else {
        r_low = val;
        r_high = 0;
      }

      k = next[k];

      val = Dot(m.row(k), axis) * r.m_size[k];

      if(val > 0)
        r_high += val;
      else
        r_low += val;

      // Distance between basepoints for boxes along this axis

      dist = Dot(sep, axis);

      if(_Greater(r_low - dist, b_high, proper)
        || _Less(r_high - dist, b_low, proper))
        return false;
    }
  }

  return true;
}

}
