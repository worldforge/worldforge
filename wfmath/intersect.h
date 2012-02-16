// intersect.h (Shape intersection functions)
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

#ifndef WFMATH_INTERSECT_H
#define WFMATH_INTERSECT_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/intersect_decls.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/segment.h>
#include <wfmath/rotbox.h>

#include <cmath>

namespace WFMath {

// Get the reversed order intersect functions (is this safe? FIXME)
// No it's not. In the case of an unknown intersection we end up in
// a stack crash loop.

template<class S1, class S2>
inline bool Intersect(const S1& s1, const S2& s2, bool proper)
{
  return Intersect(s2, s1, proper);
}

// Point<>

template<int dim>
inline bool Intersect(const Point<dim>& p1, const Point<dim>& p2, bool proper)
{
  return !proper && p1 == p2;
}

template<int dim, class S>
inline bool Contains(const S& s, const Point<dim>& p, bool proper)
{
  return Intersect(p, s, proper);
}

template<int dim>
inline bool Contains(const Point<dim>& p1, const Point<dim>& p2, bool proper)
{
  return !proper && p1 == p2;
}

// AxisBox<>

template<int dim>
inline bool Intersect(const AxisBox<dim>& b, const Point<dim>& p, bool proper)
{
  for(int i = 0; i < dim; ++i)
    if(_Greater(b.m_low[i], p[i], proper) || _Less(b.m_high[i], p[i], proper))
      return false;

  return true;
}

template<int dim>
inline bool Contains(const Point<dim>& p, const AxisBox<dim>& b, bool proper)
{
  return !proper && p == b.m_low && p == b.m_high;
}

template<int dim>
inline bool Intersect(const AxisBox<dim>& b1, const AxisBox<dim>& b2, bool proper)
{
  for(int i = 0; i < dim; ++i)
    if(_Greater(b1.m_low[i], b2.m_high[i], proper)
      || _Less(b1.m_high[i], b2.m_low[i], proper))
      return false;

  return true;
}

template<int dim>
inline bool Contains(const AxisBox<dim>& outer, const AxisBox<dim>& inner, bool proper)
{
  for(int i = 0; i < dim; ++i)
    if(_Less(inner.m_low[i], outer.m_low[i], proper)
      || _Greater(inner.m_high[i], outer.m_high[i], proper))
      return false;

  return true;
}

// Ball<>

template<int dim>
inline bool Intersect(const Ball<dim>& b, const Point<dim>& p, bool proper)
{
  return _LessEq(SquaredDistance(b.m_center, p), b.m_radius * b.m_radius
					   * (1 + WFMATH_EPSILON), proper);
}

template<int dim>
inline bool Contains(const Point<dim>& p, const Ball<dim>& b, bool proper)
{
  return !proper && b.m_radius == 0 && p == b.m_center;
}

template<int dim>
inline bool Intersect(const Ball<dim>& b, const AxisBox<dim>& a, bool proper)
{
  CoordType dist = 0;

  for(int i = 0; i < dim; ++i) {
    CoordType dist_i;
    if(b.m_center[i] < a.m_low[i])
      dist_i = b.m_center[i] - a.m_low[i];
    else if(b.m_center[i] > a.m_high[i])
      dist_i = b.m_center[i] - a.m_high[i];
    else
      continue;
    dist+= dist_i * dist_i;
  }

  return _LessEq(dist, b.m_radius * b.m_radius, proper);
}

template<int dim>
inline bool Contains(const Ball<dim>& b, const AxisBox<dim>& a, bool proper)
{
  CoordType sqr_dist = 0;

  for(int i = 0; i < dim; ++i) {
    CoordType furthest = FloatMax(std::fabs(b.m_center[i] - a.m_low[i]),
                                  std::fabs(b.m_center[i] - a.m_high[i]));
    sqr_dist += furthest * furthest;
  }

  return _LessEq(sqr_dist, b.m_radius * b.m_radius * (1 + WFMATH_EPSILON), proper);
}

template<int dim>
inline bool Contains(const AxisBox<dim>& a, const Ball<dim>& b, bool proper)
{
  for(int i = 0; i < dim; ++i)
    if(_Less(b.m_center[i] - b.m_radius, a.lowerBound(i), proper)
       || _Greater(b.m_center[i] + b.m_radius, a.upperBound(i), proper))
      return false;

  return true;
}

template<int dim>
inline bool Intersect(const Ball<dim>& b1, const Ball<dim>& b2, bool proper)
{
  CoordType sqr_dist = SquaredDistance(b1.m_center, b2.m_center);
  CoordType rad_sum = b1.m_radius + b2.m_radius;

  return _LessEq(sqr_dist, rad_sum * rad_sum, proper);
}

template<int dim>
inline bool Contains(const Ball<dim>& outer, const Ball<dim>& inner, bool proper)
{
  CoordType rad_diff = outer.m_radius - inner.m_radius;

  if(_Less(rad_diff, 0, proper))
    return false;

  CoordType sqr_dist = SquaredDistance(outer.m_center, inner.m_center);

  return _LessEq(sqr_dist, rad_diff * rad_diff, proper);
}

// Segment<>

template<int dim>
inline bool Intersect(const Segment<dim>& s, const Point<dim>& p, bool proper)
{
  // This is only true if p lies on the line between m_p1 and m_p2

  Vector<dim> v1 = s.m_p1 - p, v2 = s.m_p2 - p;

  CoordType proj = Dot(v1, v2);

  if(_Greater(proj, 0, proper)) // p is on the same side of both ends, not between them
    return false;

  // Check for colinearity
  return Equal(proj * proj, v1.sqrMag() * v2.sqrMag());
}

template<int dim>
inline bool Contains(const Point<dim>& p, const Segment<dim>& s, bool proper)
{
  return !proper && p == s.m_p1 && p == s.m_p2;
}

template<int dim>
bool Intersect(const Segment<dim>& s, const AxisBox<dim>& b, bool proper)
{
  // Use parametric coordinates on the line, where 0 is the location
  // of m_p1 and 1 is the location of m_p2

  // Find the parametric coordinates of the portion of the line
  // which lies betweens b.lowerBound(i) and b.upperBound(i) for
  // each i. Find the intersection of those segments and the
  // segment (0, 1), and check if it's nonzero.

  CoordType min = 0, max = 1;

  for(int i = 0; i < dim; ++i) {
    CoordType dist = s.m_p2[i] - s.m_p1[i];
    if(dist == 0) {
      if(_Less(s.m_p1[i], b.m_low[i], proper)
        || _Greater(s.m_p1[i], b.m_high[i], proper))
        return false;
    }
    else {
      CoordType low = (b.m_low[i] - s.m_p1[i]) / dist;
      CoordType high = (b.m_high[i] - s.m_p1[i]) / dist;
      if(low > high) {
        CoordType tmp = high;
        high = low;
        low = tmp;
      }
      if(low > min)
        min = low;
      if(high < max)
        max = high;
    }
  }

  return _LessEq(min, max, proper);
}

template<int dim>
inline bool Contains(const Segment<dim>& s, const AxisBox<dim>& b, bool proper)
{
  // This is only possible for zero width or zero height box,
  // in which case we check for containment of the endpoints.

  bool got_difference = false;

  for(int i = 0; i < dim; ++i) {
    if(b.m_low[i] == b.m_high[i])
      continue;
    if(got_difference)
      return false;
    else // It's okay to be different on one axis
      got_difference = true;
  }

  return Contains(s, b.m_low, proper) &&
        (got_difference ? Contains(s, b.m_high, proper) : true);
}

template<int dim>
inline bool Contains(const AxisBox<dim>& b, const Segment<dim>& s, bool proper)
{
  return Contains(b, s.m_p1, proper) && Contains(b, s.m_p2, proper);
}

template<int dim>
bool Intersect(const Segment<dim>& s, const Ball<dim>& b, bool proper)
{
  Vector<dim> line = s.m_p2 - s.m_p1, offset = b.m_center - s.m_p1;

  // First, see if the closest point on the line to the center of
  // the ball lies inside the segment

  CoordType proj = Dot(line, offset);

  // If the nearest point on the line is outside the segment,
  // intersection reduces to checking the nearest endpoint.

  if(proj <= 0)
    return Intersect(b, s.m_p1, proper);

  CoordType lineSqrMag = line.sqrMag();

  if (proj >= lineSqrMag)
    return Intersect(b, s.m_p2, proper);

  Vector<dim> perp_part = offset - line * (proj / lineSqrMag);

  return _LessEq(perp_part.sqrMag(), b.m_radius * b.m_radius, proper);
}

template<int dim>
inline bool Contains(const Ball<dim>& b, const Segment<dim>& s, bool proper)
{
  return Contains(b, s.m_p1, proper) && Contains(b, s.m_p2, proper);
}

template<int dim>
inline bool Contains(const Segment<dim>& s, const Ball<dim>& b, bool proper)
{
  return b.m_radius == 0 && Contains(s, b.m_center, proper);
}

template<int dim>
bool Intersect(const Segment<dim>& s1, const Segment<dim>& s2, bool proper)
{
  // Check that the lines that contain the segments intersect, and then check
  // that the intersection point lies within the segments

  Vector<dim> v1 = s1.m_p2 - s1.m_p1, v2 = s2.m_p2 - s2.m_p1,
	      deltav = s2.m_p1 - s1.m_p1;

  CoordType v1sqr = v1.sqrMag(), v2sqr = v2.sqrMag();
  CoordType proj12 = Dot(v1, v2), proj1delta = Dot(v1, deltav),
	    proj2delta = Dot(v2, deltav);

  CoordType denom = v1sqr * v2sqr - proj12 * proj12;

  if(dim > 2 && !Equal(v2sqr * proj1delta * proj1delta +
		         v1sqr * proj2delta * proj2delta,
		       2 * proj12 * proj1delta * proj2delta +
		         deltav.sqrMag() * denom))
    return false; // Skew lines; don't intersect

  if(denom > 0) {
    // Find the location of the intersection point in parametric coordinates,
    // where one end of the segment is at zero and the other at one

    CoordType coord1 = (v2sqr * proj1delta - proj12 * proj2delta) / denom;
    CoordType coord2 = -(v1sqr * proj2delta - proj12 * proj1delta) / denom;

    return _LessEq(coord1, 0, proper) && _LessEq(coord1, 1, proper)
           && _GreaterEq(coord2, 0, proper) && _GreaterEq(coord2, 1, proper);
  }
  else {
    // Parallel segments, see if one contains an endpoint of the other
    return Contains(s1, s2.m_p1, proper) || Contains(s1, s2.m_p2, proper)
	|| Contains(s2, s1.m_p1, proper) || Contains(s2, s1.m_p2, proper)
	// Degenerate case (identical segments), nonzero length
	|| ((proper && s1.m_p1 != s1.m_p2)
          && ((s1.m_p1 == s2.m_p1 && s1.m_p2 == s2.m_p2)
              || (s1.m_p1 == s2.m_p2 && s1.m_p2 == s2.m_p1)));
  }
}

template<int dim>
inline bool Contains(const Segment<dim>& s1, const Segment<dim>& s2, bool proper)
{
  return Contains(s1, s2.m_p1, proper) && Contains(s1, s2.m_p2, proper);
}

// RotBox<>

template<int dim>
inline bool Intersect(const RotBox<dim>& r, const Point<dim>& p, bool proper)
{
  // Rotate the point into the internal coordinate system of the box

  Vector<dim> shift = ProdInv(p - r.m_corner0, r.m_orient);

  for(int i = 0; i < dim; ++i) {
    if(r.m_size[i] < 0) {
      if(_Less(shift[i], r.m_size[i], proper) || _Greater(shift[i], 0, proper))
        return false;
    }
    else {
      if(_Greater(shift[i], r.m_size[i], proper) || _Less(shift[i], 0, proper))
        return false;
    }
  }

  return true;
}

template<int dim>
inline bool Contains(const Point<dim>& p, const RotBox<dim>& r, bool proper)
{
  if(proper)
    return false;

  for(int i = 0; i < dim; ++i)
    if(r.m_size[i] != 0)
      return false;

  return p == r.m_corner0;
}

template<int dim>
bool Intersect(const RotBox<dim>& r, const AxisBox<dim>& b, bool proper);

template<int dim>
inline bool Contains(const RotBox<dim>& r, const AxisBox<dim>& b, bool proper)
{
  RotMatrix<dim> m = r.m_orient.inverse();

  return Contains(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
		  RotBox<dim>(Point<dim>(b.m_low).rotate(m, r.m_corner0),
			      b.m_high - b.m_low, m), proper);
}

template<int dim>
inline bool Contains(const AxisBox<dim>& b, const RotBox<dim>& r, bool proper)
{
  return Contains(b, r.boundingBox(), proper);
}

template<int dim>
inline bool Intersect(const RotBox<dim>& r, const Ball<dim>& b, bool proper)
{
  return Intersect(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
		  Ball<dim>(r.m_corner0 + ProdInv(b.m_center - r.m_corner0,
			    r.m_orient), b.m_radius), proper);
}

template<int dim>
inline bool Contains(const RotBox<dim>& r, const Ball<dim>& b, bool proper)
{
  return Contains(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
		  Ball<dim>(r.m_corner0 + ProdInv(b.m_center - r.m_corner0,
			    r.m_orient), b.m_radius), proper);
}

template<int dim>
inline bool Contains(const Ball<dim>& b, const RotBox<dim>& r, bool proper)
{
  return Contains(Ball<dim>(r.m_corner0 + ProdInv(b.m_center - r.m_corner0,
			    r.m_orient), b.m_radius),
		  AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size), proper);
}

template<int dim>
inline bool Intersect(const RotBox<dim>& r, const Segment<dim>& s, bool proper)
{
  Point<dim> p1 = r.m_corner0 + ProdInv(s.m_p1 - r.m_corner0, r.m_orient);
  Point<dim> p2 = r.m_corner0 + ProdInv(s.m_p2 - r.m_corner0, r.m_orient);

  return Intersect(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
		   Segment<dim>(p1, p2), proper);
}

template<int dim>
inline bool Contains(const RotBox<dim>& r, const Segment<dim>& s, bool proper)
{
  Point<dim> p1 = r.m_corner0 + ProdInv(s.m_p1 - r.m_corner0, r.m_orient);
  Point<dim> p2 = r.m_corner0 + ProdInv(s.m_p2 - r.m_corner0, r.m_orient);

  return Contains(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
		  Segment<dim>(p1, p2), proper);
}

template<int dim>
inline bool Contains(const Segment<dim>& s, const RotBox<dim>& r, bool proper)
{
  Point<dim> p1 = r.m_corner0 + ProdInv(s.m_p1 - r.m_corner0, r.m_orient);
  Point<dim> p2 = r.m_corner0 + ProdInv(s.m_p2 - r.m_corner0, r.m_orient);

  return Contains(Segment<dim>(p1, p2),
		  AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size), proper);
}

template<int dim>
inline bool Intersect(const RotBox<dim>& r1, const RotBox<dim>& r2, bool proper)
{
  return Intersect(RotBox<dim>(r1).rotatePoint(r2.m_orient.inverse(),
					       r2.m_corner0),
		   AxisBox<dim>(r2.m_corner0, r2.m_corner0 + r2.m_size), proper);
}

template<int dim>
inline bool Contains(const RotBox<dim>& outer, const RotBox<dim>& inner, bool proper)
{
  return Contains(AxisBox<dim>(outer.m_corner0, outer.m_corner0 + outer.m_size),
		  RotBox<dim>(inner).rotatePoint(outer.m_orient.inverse(),
						 outer.m_corner0), proper);
}

// Polygon<> intersection functions are in polygon_funcs.h, to avoid
// unnecessary inclusion of <vector>

} // namespace WFMath

#endif  // WFMATH_INTERSECT_H
