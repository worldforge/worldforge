// -*-C++-*-
// intersect.h (Shape intersection functions)
//
//  The WorldForge Project
//  Copyright (C) 2000, 2001  The WorldForge Project
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

namespace WF { namespace Math {

// Get the reversed order intersect functions (is this safe? FIXME)

template<class S1, class S2>
inline bool Intersect(const S1& s1, const S2& s2)
{
  return Intersect(s2, s1);
}

template<class S1, class S2>
inline bool IntersectProper(const S1& s1, const S2& s2)
{
  return IntersectProper(s2, s1);
}

// Point<>

template<const int dim>
inline bool Intersect(const Point<dim>& p1, const Point<dim>& p2)
{
  return p1 == p2;
}

template<const int dim>
inline bool IntersectProper(const Point<dim>& p1, const Point<dim>& p2)
{
  return false;
}

template<const int dim, class S>
inline bool Contains(const S& s, const Point<dim>& p)
{
  return Intersect(p, s);
}

template<const int dim>
inline bool Contains(const Point<dim>& p1, const Point<dim>& p2)
{
  return p1 == p2;
}

template<const int dim, class S>
inline bool ContainsProper(const Point<dim>& p, const S& s)
{
  return false;
}

template<const int dim, class S>
inline bool ContainsProper(const S& s, const Point<dim>& p)
{
  return IntersectProper(p, s);
}

template<const int dim>
inline bool ContainsProper(const Point<dim>& p1, const Point<dim>& p2)
{
  return false;
}

// AxisBox<>

template<const int dim>
bool Intersect(const AxisBox<dim>& b, const Point<dim>& p)
{
  for(int i = 0; i < dim; ++i)
    if(b.m_low[i] > p[i] || b.m_high[i] < p[i])
      return false;

  return true;
}

template<const int dim>
bool IntersectProper(const AxisBox<dim>& b, const Point<dim>& p)
{
  for(int i = 0; i < dim; ++i)
    if(b.m_low[i] >= p[i] || b.m_high[i] <= p[i])
      return false;

  return true;
}

template<const int dim>
inline bool Contains(const Point<dim>& p, const AxisBox<dim>& b)
{
  return p == b.m_low && p == b.m_high;
}

template<const int dim>
bool Intersect(const AxisBox<dim>& b1, const AxisBox<dim>& b2)
{
  for(int i = 0; i < dim; ++i)
    if(b1.m_low[i] > b2.m_high[i] || b1.m_high[i] < b2.m_low[i])
      return false;

  return true;
}

template<const int dim>
bool IntersectProper(const AxisBox<dim>& b1, const AxisBox<dim>& b2)
{
  for(int i = 0; i < dim; ++i)
    if(b1.m_low[i] >= b2.m_high[i] || b1.m_high[i] <= b2.m_low[i])
      return false;

  return true;
}

template<const int dim>
bool Contains(const AxisBox<dim>& outer, const AxisBox<dim>& inner)
{
  for(int i = 0; i < dim; ++i)
    if(inner.m_low[i] < outer.m_low[i] || inner.m_high[i] > outer.m_high[i])
      return false;

  return true;
}

template<const int dim>
bool ContainsProper(const AxisBox<dim>& outer, const AxisBox<dim>& inner)
{
  for(int i = 0; i < dim; ++i)
    if(inner.m_low[i] <= outer.m_low[i] || inner.m_high[i] >= outer.m_high[i])
      return false;

  return true;
}

// Ball<>

template<const int dim>
bool Intersect(const Ball<dim>& b, const Point<dim>& p)
{
  return SquaredDistance(b.m_center, p) <= b.m_radius * b.m_radius
					   * (1 + WFMATH_EPSILON);
}

template<const int dim>
bool IntersectProper(const Ball<dim>& b, const Point<dim>& p)
{
  return SquaredDistance(b.m_center, p) < b.m_radius * b.m_radius;
}

template<const int dim>
inline bool Contains(const Point<dim>& p, const Ball<dim>& b)
{
  return b.m_radius == 0 && p == b.m_center;
}

template<const int dim>
bool Intersect(const Ball<dim>& b, const AxisBox<dim>& a)
{
  CoordType dist = 0;

  // Don't use FloatAdd(), only need values for comparison

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

  return dist <= b.m_radius * b.m_radius;
}

template<const int dim>
bool IntersectProper(const Ball<dim>& b, const AxisBox<dim>& a)
{
  CoordType dist = 0;

  // Don't use FloatAdd(), only need values for comparison

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

  return dist < b.m_radius * b.m_radius;
}

template<const int dim>
bool Contains(const Ball<dim>& b, const AxisBox<dim>& a)
{
  CoordType sqr_dist = 0;

  for(int i = 0; i < dim; ++i) {
    CoordType furthest = FloatMax(fabs(b.m_center[i] - a.m_low[i]),
			       fabs(b.m_center[i] - a.m_high[i]));
    sqr_dist += furthest * furthest;
  }

  return sqr_dist <= b.m_radius * b.m_radius * (1 + WFMATH_EPSILON);
}

template<const int dim>
bool ContainsProper(const Ball<dim>& b, const AxisBox<dim>& a)
{
  CoordType sqr_dist = 0;

  for(int i = 0; i < dim; ++i) {
    CoordType furthest = FloatMax(fabs(b.m_center[i] - a.m_low[i]),
			       fabs(b.m_center[i] - a.m_high[i]));
    sqr_dist += furthest * furthest;
  }

  return sqr_dist < b.m_radius * b.m_radius;
}

template<const int dim>
bool Contains(const AxisBox<dim>& a, const Ball<dim>& b)
{
  // Don't use FloatAdd(), only need values for comparison

  for(int i = 0; i < dim; ++i)
    if(b.m_center[i] - b.m_radius < a.lowerBound(i)
       || b.m_center[i] + b.m_radius > a.upperBound(i))
      return false;

  return true;
}

template<const int dim>
bool ContainsProper(const AxisBox<dim>& a, const Ball<dim>& b)
{
  // Don't use FloatAdd(), only need values for comparison

  for(int i = 0; i < dim; ++i)
    if(b.m_center[i] - b.m_radius <= a.lowerBound(i)
       || b.m_center[i] + b.m_radius >= a.upperBound(i))
      return false;

  return true;
}

template<const int dim>
bool Intersect(const Ball<dim>& b1, const Ball<dim>& b2)
{
  CoordType sqr_dist = SquaredDistance(b1.m_center, b2.m_center);
  CoordType rad_sum = b1.m_radius + b2.m_radius;

  return sqr_dist <= rad_sum * rad_sum;
}

template<const int dim>
bool IntersectProper(const Ball<dim>& b1, const Ball<dim>& b2)
{
  CoordType sqr_dist = SquaredDistance(b1.m_center, b2.m_center);
  CoordType rad_sum = b1.m_radius + b2.m_radius;

  return sqr_dist < rad_sum * rad_sum;
}

template<const int dim>
bool Contains(const Ball<dim>& outer, const Ball<dim>& inner)
{
  // Need FloatSubtract() so a ball will contain itself
  CoordType rad_diff = FloatSubtract(outer.m_radius, inner.m_radius);

  if(rad_diff < 0)
    return false;

  CoordType sqr_dist = SquaredDistance(outer.m_center, inner.m_center);

  return sqr_dist <= rad_diff * rad_diff;
}

template<const int dim>
bool ContainsProper(const Ball<dim>& outer, const Ball<dim>& inner)
{
  // Need FloatSubtract() so a ball will not contain itself properly
  CoordType rad_diff = FloatSubtract(outer.m_radius, inner.m_radius);

  if(rad_diff <= 0)
    return false;

  CoordType sqr_dist = SquaredDistance(outer.m_center, inner.m_center);

  return sqr_dist < rad_diff * rad_diff;
}

// Segment<>

template<const int dim>
bool Intersect(const Segment<dim>& s, const Point<dim>& p)
{
  // This is only true if p lies on the line between m_p1 and m_p2

  Vector<dim> v1 = s.m_p1 - p, v2 = s.m_p2 - p;

  CoordType proj = Dot(v1, v2);

  if(proj > 0) // p is on the same side of both ends, not between them
    return false;

  // Check for colinearity
  return IsFloatEqual(proj * proj, v1.sqrMag() * v2.sqrMag());
}

template<const int dim>
bool IntersectProper(const Segment<dim>& s, const Point<dim>& p)
{
  // This is only true if p lies on the line between m_p1 and m_p2

  Vector<dim> v1 = s.m_p1 - p, v2 = s.m_p2 - p;

  CoordType proj = Dot(v1, v2);

  if(proj >= 0) // p is on the same side of both ends, not between them
    return false;

  // Check for colinearity
  return IsFloatEqual(proj * proj, v1.sqrMag() * v2.sqrMag());
}

template<const int dim>
inline bool Contains(const Point<dim>& p, const Segment<dim>& s)
{
  return p == s.m_p1 && p == s.m_p2;
}

template<const int dim>
bool Intersect(const Segment<dim>& s, const AxisBox<dim>& b)
{
  // Use parametric coordinates on the line, where 0 is the location
  // of m_p1 and 1 is the location of m_p2

  // Find the parametric coordinates of the portion of the line
  // which lies betweens b.lowerBound(i) and b.upperBound(i) for
  // each i. Find the intersection of those segments and the
  // segment (0, 1), and check if it's nonzero.

  CoordType min = 0, max = 1, low, high, small, big;

  for(int i = 0; i < dim; ++i) {
    CoordType dist = FloatSubtract(s.m_p2[i], s.m_p1[i]);
    if(dist == 0) {
      if(s.m_p1[i] < b.m_low[i] || s.m_p1[i] > b.m_high[i])
        return false;
    }
    else {
      low = (b.m_low[i] - s.m_p1[i]) / dist;
      high = (b.m_high[i] - s.m_p1[i]) / dist;
      if(low < high) {
        small = low;
        big = high;
      }
      else {
        big = low;
        small = high;
      }
      if(small > min)
        min = small;
      if(big < max)
        max = big;
    }
  }

  return min <= max;
}

template<const int dim>
bool IntersectProper(const Segment<dim>& s, const AxisBox<dim>& b)
{
  // Use parametric coordinates on the line, where 0 is the location
  // of m_p1 and 1 is the location of m_p2

  // Find the parametric coordinates of the portion of the line
  // which lies betweens b.lowerBound(i) and b.upperBound(i) for
  // each i. Find the intersection of those segments and the
  // segment (0, 1), and check if it's nonzero.

  CoordType min = 0, max = 1, low, high, small, big;

  for(int i = 0; i < dim; ++i) {
    CoordType dist = FloatSubtract(s.m_p2[i], s.m_p1[i]);
    if(dist == 0) {
      if(s.m_p1[i] <= b.m_low[i] || s.m_p1[i] >= b.m_high[i])
        return false;
    }
    else {
      low = (b.m_low[i] - s.m_p1[i]) / dist;
      high = (b.m_high[i] - s.m_p1[i]) / dist;
      if(low < high) {
        small = low;
        big = high;
      }
      else {
        big = low;
        small = high;
      }
      if(small > min)
        min = small;
      if(big < max)
        max = big;
    }
  }

  return min < max;
}

template<const int dim>
bool Contains(const Segment<dim>& s, const AxisBox<dim>& b)
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

  return Contains(s, b.m_low) && (got_difference ? Contains(s, b.m_high) : true);
}

template<const int dim>
bool ContainsProper(const Segment<dim>& s, const AxisBox<dim>& b)
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

  return ContainsProper(s, b.m_low) && (got_difference ?
	 ContainsProper(s, b.m_high) : true);
}

template<const int dim>
inline bool Contains(const AxisBox<dim>& b, const Segment<dim>& s)
{
  return Contains(b, s.m_p1) && Contains(b, s.m_p2);
}

template<const int dim>
inline bool ContainsProper(const AxisBox<dim>& b, const Segment<dim>& s)
{
  return ContainsProper(b, s.m_p1) && ContainsProper(b, s.m_p2);
}

template<const int dim>
bool Intersect(const Segment<dim>& s, const Ball<dim>& b)
{
  Vector<dim> line = s.m_p2 - s.m_p1, offset = b.m_center - s.m_p1;

  // First, see if the closest point on the line to the center of
  // the ball lies inside the segment

  CoordType proj = Dot(line, offset), lineSqrMag = line.sqrMag();

  if(proj < 0 || proj > lineSqrMag) {
    // The nearest point on the line is outside the segment. Intersection reduces
    // to checking the endpoints.

    return Intersect(b, s.m_p1) || Intersect(b, s.m_p2);
  }

  Vector<dim> perp_part = offset - line * proj / lineSqrMag;

  return perp_part.sqrMag() <= b.m_radius * b.m_radius;
}

template<const int dim>
bool IntersectProper(const Segment<dim>& s, const Ball<dim>& b)
{
  Vector<dim> line = s.m_p2 - s.m_p1, offset = b.m_center - s.m_p1;

  // First, see if the closest point on the line to the center of
  // the ball lies inside the segment

  CoordType proj = Dot(line, offset), lineSqrMag = line.sqrMag();

  if(proj < 0 || proj > lineSqrMag) {
    // The nearest point on the line is outside the segment. Intersection reduces
    // to checking the endpoints.

    return IntersectProper(b, s.m_p1) || IntersectProper(b, s.m_p2);
  }

  Vector<dim> perp_part = offset - line * proj / lineSqrMag;

  return perp_part.sqrMag() < b.m_radius * b.m_radius;
}

template<const int dim>
inline bool Contains(const Ball<dim>& b, const Segment<dim>& s)
{
  return Contains(b, s.m_p1) && Contains(b, s.m_p2);
}

template<const int dim>
inline bool ContainsProper(const Ball<dim>& b, const Segment<dim>& s)
{
  return ContainsProper(b, s.m_p1) && ContainsProper(b, s.m_p2);
}

template<const int dim>
inline bool Contains(const Segment<dim>& s, const Ball<dim>& b)
{
  return b.m_radius == 0 && Contains(s, b.m_center);
}

template<const int dim>
inline bool ContainsProper(const Segment<dim>& s, const Ball<dim>& b)
{
  return b.m_radius == 0 && ContainsProper(s, b.m_center);
}

template<const int dim>
bool Intersect(const Segment<dim>& s1, const Segment<dim>& s2)
{
  // Check that the lines that contain the segments intersect, and then check
  // that the intersection point lies within the segments

  Vector<dim> v1 = s1.m_p2 - s1.m_p1, v2 = s2.m_p2 - s2.m_p1,
	      deltav = s2.m_p1 - s1.m_p1;

  CoordType v1sqr = v1.sqrMag(), v2sqr = v2.sqrMag();
  CoordType proj12 = Dot(v1, v2), proj1delta = Dot(v1, deltav),
	    proj2delta = Dot(v2, deltav);

  CoordType denom = FloatSubtract(v1sqr * v2sqr, proj12 * proj12);

  if(dim > 2 && !IsFloatEqual(FloatAdd(v2sqr * proj1delta * proj1delta,
				       v1sqr * proj2delta * proj2delta),
			      FloatAdd(2 * proj12 * proj1delta * proj2delta,
				       deltav.sqrMag() * denom)))
    return false; // Skew lines; don't intersect

  if(denom > 0) {
    // Find the location of the intersection point in parametric coordinates,
    // where one end of the segment is at zero and the other at one

    CoordType coord1 = FloatSubtract(v2sqr * proj1delta, proj12 * proj2delta) / denom;
    CoordType coord2 = -FloatSubtract(v1sqr * proj2delta, proj12 * proj1delta) / denom;

    return coord1 >= 0 && coord1 <= 1 && coord2 >= 0 && coord2 <= 1;
  }
  else {
    // Parallel segments, see if one contains an endpoint of the other
    return Contains(s1, s2.m_p1) || Contains(s1, s2.m_p2)
	|| Contains(s2, s1.m_p1) || Contains(s2, s1.m_p2);
  }
}

template<const int dim>
bool IntersectProper(const Segment<dim>& s1, const Segment<dim>& s2)
{
  // Check that the lines that contain the segments intersect, and then check
  // that the intersection point lies within the segments

  Vector<dim> v1 = s1.m_p2 - s1.m_p1, v2 = s2.m_p2 - s2.m_p1,
	      deltav = s2.m_p1 - s1.m_p1;

  CoordType v1sqr = v1.sqrMag(), v2sqr = v2.sqrMag();
  CoordType proj12 = Dot(v1, v2), proj1delta = Dot(v1, deltav),
	    proj2delta = Dot(v2, deltav);

  CoordType denom = FloatSubtract(v1sqr * v2sqr, proj12 * proj12);

  if(dim > 2 && !IsFloatEqual(FloatAdd(v2sqr * proj1delta * proj1delta,
				       v1sqr * proj2delta * proj2delta),
			      FloatAdd(2 * proj12 * proj1delta * proj2delta,
				       deltav.sqrMag() * denom)))
    return false; // Skew lines; don't intersect

  if(denom > 0) {
    // Find the location of the intersection point in parametric coordinates,
    // where one end of the segment is at zero and the other at one

    CoordType coord1 = FloatSubtract(v2sqr * proj1delta, proj12 * proj2delta) / denom;
    CoordType coord2 = -FloatSubtract(v1sqr * proj2delta, proj12 * proj1delta) / denom;

    return coord1 > 0 && coord1 < 1 && coord2 > 0 && coord2 < 1;
  }
  else {
    // Parallel segments, see if one contains an endpoint of the other
    return ContainsProper(s1, s2.m_p1) || ContainsProper(s1, s2.m_p2)
	|| ContainsProper(s2, s1.m_p1) || ContainsProper(s2, s1.m_p2)
	// Degenerate case, nonzero length
	|| (s1.m_p1 != s1.m_p2) && ((s1.m_p1 == s2.m_p1 && s1.m_p2 == s2.m_p2)
	|| (s1.m_p1 == s2.m_p2 && s1.m_p2 == s2.m_p1));
  }
}

template<const int dim>
inline bool Contains(const Segment<dim>& s1, const Segment<dim>& s2)
{
  return Contains(s1, s2.m_p1) && Contains(s1, s2.m_p2);
}

template<const int dim>
inline bool ContainsProper(const Segment<dim>& s1, const Segment<dim>& s2)
{
  return ContainsProper(s1, s2.m_p1) && ContainsProper(s1, s2.m_p2);
}

// RotBox<>

template<const int dim>
bool Intersect(const RotBox<dim>& r, const Point<dim>& p)
{
  // Rotate the point into the internal coordinate system of the box

  Vector<dim> shift = InvProd(r.m_orient, p - r.m_corner0);

  for(int i = 0; i < dim; ++i) {
    if(r.m_size[i] < 0) {
      if(shift[i] < r.m_size[i] || shift[i] > 0)
        return false;
    }
    else {
      if(shift[i] > r.m_size[i] || shift[i] < 0)
        return false;
    }
  }

  return true;
}

template<const int dim>
bool IntersectProper(const RotBox<dim>& r, const Point<dim>& p)
{
  // Rotate the point into the internal coordinate system of the box

  Vector<dim> shift = InvProd(r.m_orient, p - r.m_corner0);

  for(int i = 0; i < dim; ++i) {
    if(r.m_size[i] < 0) {
      if(shift[i] <= r.m_size[i] || shift[i] >= 0)
        return false;
    }
    else {
      if(shift[i] >= r.m_size[i] || shift[i] <= 0)
        return false;
    }
  }

  return true;
}

template<const int dim>
bool Contains(const Point<dim>& p, const RotBox<dim>& r)
{
  for(int i = 0; i < dim; ++i)
    if(r.m_size != 0)
      return false;

  return p == r.m_corner0;
}

template<const int dim>
bool Intersect(const RotBox<dim>& r, const AxisBox<dim>& b);
template<const int dim>
bool IntersectProper(const RotBox<dim>& r, const AxisBox<dim>& b);

/*
// This does row reduction to solve a system of linear equations
// for the next two functions.
void  _RotBoxAxisBoxIntersectImpl(int dim, int params, CoordType* matrix,
				  CoordType* low, CoordType* high);

template<const int dim>
bool Intersect(const RotBox<dim>& r, const AxisBox<dim>& b)
{
  // This simultaneously solves the equations
  //
  // b.m_low[i] <= r.m_corner0[i] + \sum_j r.m_orient.elem(i, j)
  //               * r.m_size[j] * \lambda_j <= b.m_high[i]
  //
  // where the \lambda_j are the independent variables whose values
  // are between zero and one (sorry for the mix of C and LaTeX).
  // These equations give the condition for at least one point in r
  // to lie inside b. The solution is found by inverting the matrix
  // whose elements are r.m_orient.elem(i, j) * r.m_size[j]. Since this
  // is _not_ a RotMatrix<>, we have to do things the hard way.

  int params = 0; // The number of non-degenerate equations

  for(int i = 0; i < dim; ++i)
    if(r.m_size[i] != 0)
      ++params;

  CoordType matrix[dim*dim], low[dim], high[dim];

  for(int i = 0, num_param = 0; i < dim; ++i) {
    low[i] = FloatSubtract(b.m_low[i], r.m_corner0[i]);
    high[i] = FloatSubtract(b.m_high[i], r.m_corner0[i]);
    if(r.m_size[i] == 0)
      continue;
    for(int j = 0; j < dim; ++j)
      matrix[j*params+num_param] = r.m_orient.elem(j, i) * r.m_size[i];
    ++num_param;
  }

  // FIXME I don't think this algorithm is correct

  _RotBoxAxisBoxIntersectImpl(dim, params, matrix, low, high);

  for(int i = 0; i < dim; ++i) {
    if(low[i] > high[i])
      return false;
    // If i < params, \lambda_i can vary between 0 and 1,
    // otherwise you have a straight constraint where the
    // number in the middle is 0.
    if(low[i] > ((i < params) ? 1 : 0))
      return false;
    if(high[i] < 0)
      return false;
  }

  return true;
}

template<const int dim>
bool IntersectProper(const RotBox<dim>& r, const AxisBox<dim>& b)
{
  // This simultaneously solves the equations
  //
  // b.m_low[i] < r.m_corner0[i] + \sum_j r.m_orient.elem(i, j)
  //              * r.m_size[j] * \lambda_j < b.m_high[i]
  //
  // where the \lambda_j are the independent variables whose values
  // are between zero and one (sorry for the mix of C and LaTeX).
  // These equations give the condition for at least one point in r
  // to lie inside b. The solution is found by inverting the matrix
  // whose elements are r.m_orient.elem(i, j) * r.m_size[j]. Since this
  // is _not_ a RotMatrix<>, we have to do things the hard way.

  for(int i = 0; i < dim; ++i)
    if(r.m_size[i] == 0)
      return false; // Can't have proper intersection with a zero volume box

  CoordType matrix[dim*dim], low[dim], high[dim];

  for(int i = 0; i < dim; ++i) {
    low[i] = FloatSubtract(b.m_low[i], r.m_corner0[i]);
    high[i] = FloatSubtract(b.m_high[i], r.m_corner0[i]);
    for(int j = 0; j < dim; ++j)
      matrix[j*dim+i] = r.m_orient.elem(j, i) * r.m_size[i];
  }

  _RotBoxAxisBoxIntersectImpl(dim, dim, matrix, low, high);

  for(int i = 0; i < dim; ++i) {
    if(low[i] >= high[i])
      return false;
    // \lambda_i can vary between 0 and 1,
    if(low[i] >= 1)
      return false;
    if(high[i] <= 0)
      return false;
  }

  return true;
}
*/

template<const int dim>
bool Contains(const RotBox<dim>& r, const AxisBox<dim>& b)
{
  return Contains(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
		  RotBox<dim>(b.m_low, b.m_high - b.m_low,
		  r.m_orient.inverse()));
}

template<const int dim>
bool ContainsProper(const RotBox<dim>& r, const AxisBox<dim>& b)
{
  return ContainsProper(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
			RotBox<dim>(b.m_low, b.m_high - b.m_low,
			r.m_orient.inverse()));
}

template<const int dim>
bool Contains(const AxisBox<dim>& b, const RotBox<dim>& r)
{
  return Contains(b, r.boundingBox());
}

template<const int dim>
bool ContainsProper(const AxisBox<dim>& b, const RotBox<dim>& r)
{
  return ContainsProper(b, r.boundingBox());
}

template<const int dim>
bool Intersect(const RotBox<dim>& r, const Ball<dim>& b)
{
  return Intersect(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
		  Ball<dim>(r.m_corner0 + InvProd(r.m_orient, b.m_center
		       - r.m_corner0), b.m_radius));
}

template<const int dim>
bool IntersectProper(const RotBox<dim>& r, const Ball<dim>& b)
{
  return IntersectProper(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
			Ball<dim>(r.m_corner0 + InvProd(r.m_orient, b.m_center
			     - r.m_corner0), b.m_radius));
}

template<const int dim>
bool Contains(const RotBox<dim>& r, const Ball<dim>& b)
{
  return Contains(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
		  Ball<dim>(r.m_corner0 + InvProd(r.m_orient, b.m_center
		       - r.m_corner0), b.m_radius));
}

template<const int dim>
bool ContainsProper(const RotBox<dim>& r, const Ball<dim>& b)
{
  return ContainsProper(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
			Ball<dim>(r.m_corner0 + InvProd(r.m_orient, b.m_center
			     - r.m_corner0), b.m_radius));
}

template<const int dim>
bool Contains(const Ball<dim>& b, const RotBox<dim>& r)
{
  return Contains(Ball<dim>(r.m_corner0 + InvProd(r.m_orient, b.m_center
		       - r.m_corner0), b.m_radius),
		  AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size));
}

template<const int dim>
bool ContainsProper(const Ball<dim>& b, const RotBox<dim>& r)
{
  return ContainsProper(Ball<dim>(r.m_corner0 + InvProd(r.m_orient, b.m_center
			     - r.m_corner0), b.m_radius),
			AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size));
}

template<const int dim>
bool Intersect(const RotBox<dim>& r, const Segment<dim>& s)
{
  Point<dim> p1 = r.m_corner0 + InvProd(r.m_orient, s.m_p1 - r.m_corner0);
  Point<dim> p2 = r.m_corner0 + InvProd(r.m_orient, s.m_p2 - r.m_corner0);

  return Intersect(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
		   Segment<dim>(p1, p2));
}

template<const int dim>
bool IntersectProper(const RotBox<dim>& r, const Segment<dim>& s)
{
  Point<dim> p1 = r.m_corner0 + InvProd(r.m_orient, s.m_p1 - r.m_corner0);
  Point<dim> p2 = r.m_corner0 + InvProd(r.m_orient, s.m_p2 - r.m_corner0);

  return IntersectProper(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
			 Segment<dim>(p1, p2));
}

template<const int dim>
bool Contains(const RotBox<dim>& r, const Segment<dim>& s)
{
  Point<dim> p1 = r.m_corner0 + InvProd(r.m_orient, s.m_p1 - r.m_corner0);
  Point<dim> p2 = r.m_corner0 + InvProd(r.m_orient, s.m_p2 - r.m_corner0);

  return Contains(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
		  Segment<dim>(p1, p2));
}

template<const int dim>
bool ContainsProper(const RotBox<dim>& r, const Segment<dim>& s)
{
  Point<dim> p1 = r.m_corner0 + InvProd(r.m_orient, s.m_p1 - r.m_corner0);
  Point<dim> p2 = r.m_corner0 + InvProd(r.m_orient, s.m_p2 - r.m_corner0);

  return ContainsProper(AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size),
			Segment<dim>(p1, p2));
}

template<const int dim>
bool Contains(const Segment<dim>& s, const RotBox<dim>& r)
{
  Point<dim> p1 = r.m_corner0 + InvProd(r.m_orient, s.m_p1 - r.m_corner0);
  Point<dim> p2 = r.m_corner0 + InvProd(r.m_orient, s.m_p2 - r.m_corner0);

  return Contains(Segment<dim>(p1, p2),
		  AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size));
}

template<const int dim>
bool ContainsProper(const Segment<dim>& s, const RotBox<dim>& r)
{
  Point<dim> p1 = r.m_corner0 + InvProd(r.m_orient, s.m_p1 - r.m_corner0);
  Point<dim> p2 = r.m_corner0 + InvProd(r.m_orient, s.m_p2 - r.m_corner0);

  return ContainsProper(Segment<dim>(p1, p2),
			AxisBox<dim>(r.m_corner0, r.m_corner0 + r.m_size));
}

template<const int dim>
bool Intersect(const RotBox<dim>& r1, const RotBox<dim>& r2)
{
  return Intersect(RotBox<dim>(r1.m_corner0, r1.m_size,
			       InvProd(r2.m_orient, r1.m_orient)),
		   AxisBox<dim>(r2.m_corner0, r2.m_corner0 + r2.m_size));
}

template<const int dim>
bool IntersectProper(const RotBox<dim>& r1, const RotBox<dim>& r2)
{
  return IntersectProper(RotBox<dim>(r1.m_corner0, r1.m_size,
				     InvProd(r2.m_orient, r1.m_orient)),
			 AxisBox<dim>(r2.m_corner0, r2.m_corner0 + r2.m_size));
}

template<const int dim>
bool Contains(const RotBox<dim>& outer, const RotBox<dim>& inner)
{
  return Contains(AxisBox<dim>(outer.m_corner0, outer.m_corner0 + outer.m_size),
		  RotBox<dim>(inner.m_corner0, inner.m_size,
			      InvProd(outer.m_orient, inner.m_orient)));
}

template<const int dim>
bool ContainsProper(const RotBox<dim>& outer, const RotBox<dim>& inner)
{
  return ContainsProper(AxisBox<dim>(outer.m_corner0, outer.m_corner0 + outer.m_size),
			RotBox<dim>(inner.m_corner0, inner.m_size,
				    InvProd(outer.m_orient, inner.m_orient)));
}

// Polygon<> intersection functions are in polygon_funcs.h, to avoid
// unnecessary inclusion of <vector>

}} // namespace WF::Math

#endif  // WFMATH_INTERSECT_H
