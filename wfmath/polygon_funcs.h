// polygon_funcs.h (line polygon implementation)
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

#ifndef WFMATH_POLYGON_FUNCS_H
#define WFMATH_POLYGON_FUNCS_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/polygon.h>

namespace WFMath {

template<const int dim>
_Poly2Orient<dim>& _Poly2Orient<dim>::operator=(const _Poly2Orient<dim>& a)
{
  m_origin_valid = a.m_origin_valid;
  if(m_origin_valid)
    m_origin = a.m_origin;

  for(int i = 0; i < 2; ++i) {
    m_axes_valid[i] = a.m_axes_valid[i];
    if(m_axes_valid[i])
      m_axes[i] = a.m_axes[i];
  }

  return *this;
}

template<const int dim>
bool Polygon<dim>::isEqualTo(const Polygon<dim>& p, double epsilon) const
{
  // The same polygon can be expressed in different ways in the interal
  // format, so we have to call getCorner();

  int size = m_poly.numCorners();
  if(size != p.m_poly.numCorners())
    return false;

  for(int i = 0; i < size; ++i)
    if(!Equal(getCorner(i), p.getCorner(i), epsilon))
      return false;

  return true;
}

// WARNING! This operator is for sorting only. It does not
// reflect any property of the box.
template<const int dim>
bool Polygon<dim>::operator< (const Polygon<dim>& s) const
{
  int size = m_poly.numCorners(), s_size = s.m_poly.numCorners();

  if(size != s_size)
    return size < s_size;

  for(int i = 0; i < size; ++i) {
    Point<dim> p = getCorner(i), sp = s.getCorner(i);
    if(p != sp)
      return p < sp;
  }

  return false;
}

template<const int dim>
Point<dim> _Poly2Orient<dim>::convert(const Point<2>& p) const
{
  Point<dim> out = m_origin;

  for(int j = 0; j < 2; ++j) {
    if(m_axes_valid[j])
      out += m_axes[j] * p[j];
    else
      assert(p[j] == 0);
  }

  return out;
}

template<const int dim>
bool _Poly2Orient<dim>::expand(const Point<dim>& pd, Point<2>& p2, double epsilon)
{
  p2[0] = p2[1] = 0; // initialize

  if(!m_origin_valid) { // Adding to an empty list
    m_origin = pd;
    m_origin_valid = true;
    return true;
  }

  Vector<dim> shift = pd - m_origin, start_shift = shift;

  CoordType bound = shift.sqrMag() * epsilon;

  int j = 0;

  while(true) {
    if(Dot(shift, start_shift) <= bound) // shift is effectively zero
      return true;

    if(j == 2) // Have two axes, shift doesn't lie in their plane
      return false;

    if(!m_axes_valid[j]) { // Didn't span this previously, now we do
      p2[j] = shift.mag();
      m_axes[j] = shift / p2[j];
      m_axes_valid[j] = true;
      return true;
   }

   p2[j] = Dot(shift, m_axes[j]);
   shift -= m_axes[j] * p2[j]; // shift is now perpendicular to m_axes[j]
   ++j;
  }
}

template<const int dim>
_Poly2Reorient _Poly2Orient<dim>::reduce(const Polygon<2>& poly, int skip)
{
  if(poly.numCorners() <= ((skip == 0) ? 1 : 0)) { // No corners left
    m_origin_valid = false;
    m_axes_valid[0] = false;
    m_axes_valid[1] = false;
    return _WFMATH_POLY2REORIENT_ALL;
  }

  assert(m_origin_valid);

  // Check that we still span both axes

  bool still_valid[2] = {false,}, got_ratio = false;
  CoordType ratio;

  int i = 0, end = poly.numCorners();
  if(skip == 0)
    ++i;
  assert(i != end);
  Point<2> first_point = m_poly[i];

  while(++i != end) {
    if(i == skip)
      continue;

    Vector<2> diff = m_poly[i] - first_point;
    if(diff == 0) // No addition to span
      continue;
    for(int j = 0; j < 2; ++j) {
      if(diff[j] == 0) {
        assert(diff[j ? 0 : 1] != 0); // because diff != 0
        if(still_valid[j ? 0 : 1] || got_ratio) // We span a 2D space
          return _WFMATH_POLY2REORIENT_NONE;
        still_valid[j] = true;
      }
      // The point has both elements nonzero
      if(still_valid[0] || still_valid[1]) // We span a 2D space
        return _WFMATH_POLY2REORIENT_NONE;
      CoordType new_ratio = diff[1] / diff[0];
      if(!got_ratio) {
        ratio = new_ratio;
        got_ratio = true;
        continue;
      }
      if(!Equal(ratio, new_ratio)) // We span a 2D space
        return _WFMATH_POLY2REORIENT_NONE;
    }
  }

  // Okay, we don't span both vectors. What now?

  if(still_valid[0]) {
    assert(!still_valid[1]);
    assert(!got_ratio);
    // This is easy, m_axes[1] is just invalid
    m_axes_valid[1] = false;
    if(first_point[1] != 0) { // Need to shift points
      m_origin += m_axes[1] * first_point[1];
      return _WFMATH_POLY2REORIENT_CLEAR_AXIS2;
    }
    else
      return _WFMATH_POLY2REORIENT_NONE;
  }

  if(still_valid[1]) {
    assert(!got_ratio);
    // This is a little harder, m_axes[0] is invalid, must swap axes
    if(first_point[0] != 0)
      m_origin += m_axes[0] * first_point[0];
    m_axes[0] = m_axes[1];
    m_axes_valid[1] = false;
    return _WFMATH_POLY2REORIENT_MOVE_AXIS2_TO_AXIS1;
  }

  if(!got_ratio) { // Nothing's valid, all points are equal
    bool shift_points[2] = {false,};
    for(int j = 0; j < 2; ++j) {
      if(first_point[j] != 0)
        shift_points = true;
      m_axes_valid[j] = false;
    }
    if(shift_points[0])
      return shift_points[1] ? _WFMATH_POLY2REORIENT_CLEAR_BOTH_AXES
			     : _WFMATH_POLY2REORIENT_CLEAR_AXIS1;
    else
      return shift_points[1] ? _WFMATH_POLY2REORIENT_CLEAR_AXIS2
			     : _WFMATH_POLY2REORIENT_NONE;
  }

  // All the points are colinear, along some line which is not parallel
  // to either of the original axes

  m_axes[0] += m_axes[1] * ratio;
  m_axes_valid[1] = false;
  return _Poly2Reorient(_WFMATH_POLY2REORIENT_SCALE1_CLEAR2, sqrt(1 + ratio * ratio));
}

template<const int dim>
void _Poly2Orient<dim>::rotate(const RotMatrix<dim>& m, const Point<dim>& p)
{
  if(m_origin_valid)
    m_origin.rotate(m, p);

  for(int j = 0; j < 2; ++j)
    if(m_axes_valid[j])
      m_axes[j] = Prod(m_axes[j], m);
}

template<const int dim>
void _Poly2Orient<dim>::rotate(const RotMatrix<dim>& m, const Point<2>& p)
{
  assert(m_origin_valid);

  if(!m_axes_valid[0]) {
    assert(p[0] == 0 && p[1] == 0);
    return;
  }

  Vector<dim> shift = m_axes[0] * p[0];
  m_axes[0] = Prod(m_axes[0], m);

  if(m_axes_valid[1]) {
    shift += m_axes[1] * p[1];
    m_axes[1] = Prod(m_axes[1], m);
  }
  else
    assert(p[1] == 0);

  m_origin += shift - Prod(shift, m);
}

template<const int dim>
Vector<dim> _Poly2Orient<dim>::offset(const Point<dim>& pd, Point<2>& p2) const
{
  assert(m_origin_valid); // Check for empty polygon before calling this

  Vector<dim> out = pd - m_origin;

  for(int j = 0; j < 2; ++j) {
    p2[j] = Dot(out, m_axes[j]);
    out -= p2[j] * m_axes[j];
  }

  return out;
}

template<>
bool _Poly2Orient<3>::checkIntersectPlane(const AxisBox<3>& b, Point<2>& p2) const;

template<const int dim>
bool _Poly2Orient<dim>::checkIntersect(const AxisBox<dim>& b, Point<2>& p2) const
{
  assert(m_origin_valid);

  if(!m_axes_valid[0]) {
    // Single point
    p2[0] = p2[1] = 0;
    return Intersect(b, convert(p2));
  }

  if(m_axes_valid[1]) {
    // A plane

    // I only know how to do this in 3D, so write a function which will
    // specialize to different dimensions

    return checkIntersectPlane(b, p2);
  }

  // A line

  // This is a modified version of AxisBox<>/Segment<> intersection

  CoordType min = 0, max = 0; // Initialize to avoid compiler warnings
  bool got_bounds = false;

  for(int i = 0; i < dim; ++i) {
    const CoordType dist = (m_axes[0])[i]; // const may optimize away better
    if(dist == 0) {
      if(m_origin[i] < b.lowerBound[i] || m_origin[i] > b.upperBound[i])
        return false;
    }
    else {
      CoordType low = (b.lowerBound[i] - m_origin[i]) / dist;
      CoordType high = (b.upperBound[i] - m_origin[i]) / dist;
      if(low > high) {
        CoordType tmp = high;
        high = low;
        low = tmp;
      }
      if(got_bounds) {
        if(low > min)
          min = low;
        if(high < max)
          max = high;
      }
      else {
        min = low;
        max = high;
        got_bounds = true;
      }
    }
  }

  assert(got_bounds); // We can't be parallel in _all_ dimensions

  if(min <= max) {
    p2 = m_origin + m_axes[0] * (max - min) / 2;
    return true;
  }
  else
    return false;
}

template<const int dim>
bool _Poly2Orient<dim>::checkIntersectProper(const AxisBox<dim>& b, Point<2>& p2) const
{
  assert(m_origin_valid);

  if(!m_axes_valid[0]) {
    // Single point
    p2[0] = p2[1] = 0;
    return IntersectProper(b, convert(p2));
  }

  if(m_axes_valid[1]) {
    // A plane

    // I only know how to do this in 3D, so write a function which will
    // specialize to different dimensions

    return checkIntersectPlane(b, p2) && ContainsProper(b, p2);
  }

  // A line

  // This is a modified version of AxisBox<>/Segment<> intersection

  CoordType min = 0, max = 0; // Initialize to avoid compiler warnings
  bool got_bounds = false;

  for(int i = 0; i < dim; ++i) {
    const CoordType dist = (m_axes[0])[i]; // const may optimize away better
    if(dist == 0) {
      if(m_origin[i] <= b.lowerBound[i] || m_origin[i] >= b.upperBound[i])
        return false;
    }
    else {
      CoordType low = (b.lowerBound[i] - m_origin[i]) / dist;
      CoordType high = (b.upperBound[i] - m_origin[i]) / dist;
      if(low > high) {
        CoordType tmp = high;
        high = low;
        low = tmp;
      }
      if(got_bounds) {
        if(low > min)
          min = low;
        if(high < max)
          max = high;
      }
      else {
        min = low;
        max = high;
        got_bounds = true;
      }
    }
  }

  assert(got_bounds); // We can't be parallel in _all_ dimensions

  if(min < max) {
    p2 = m_origin + m_axes[0] * (max - min) / 2;
    return true;
  }
  else
    return false;
}

template<const int dim>
bool Polygon<dim>::addCorner(int i, const Point<dim>& p, double epsilon)
{
  Point<2> p2;
  bool succ = m_orient.expand(p, p2, epsilon);
  if(succ)
    m_poly.addCorner(i, p2, epsilon);
  return succ;
}

template<const int dim>
void Polygon<dim>::removeCorner(int i)
{
  m_poly.removeCorner(i);
  _Poly2Reorient r = m_orient.reduce(m_poly);
  r.reorient(m_poly);
}

template<const int dim>
bool Polygon<dim>::moveCorner(int i, const Point<dim>& p, double epsilon)
{
  _Poly2Orient<dim> try_orient = m_orient;
  _Poly2Reorient r = try_orient.reduce(m_poly, i);
  Point<2> p2;

  if(!try_orient.expand(p, p2, epsilon))
    return false;

  r.reorient(m_poly, i);
  m_poly[i] = p2;
  m_orient = try_orient;

  return true;
}

template<const int dim>
AxisBox<dim> Polygon<dim>::boundingBox() const
{
  assert(m_poly.numCorners() > 0);

  Point<dim> min = m_orient.convert(m_poly[0]), max = min;

  for(int i = 1; i != m_poly.numCorners(); ++i) {
    Point<dim> p = m_orient.convert(m_poly[i]);
    for(int j = 0; j < dim; ++j) {
      if(p[j] < min[j])
        min[j] = p[j];
      if(p[j] > max[j])
        max[j] = p[j];
    }
  }

  return AxisBox<dim>(min, max, true);
}

template<const int dim>
Ball<dim> Polygon<dim>::boundingSphere() const
{
  Ball<2> b = m_poly.boundingSphere();

  return Ball<dim>(m_orient.convert(b.center()), b.radius());
}

template<const int dim>
Ball<dim> Polygon<dim>::boundingSphereSloppy() const
{
  Ball<2> b = m_poly.boundingSphereSloppy();

  return Ball<dim>(m_orient.convert(b.center()), b.radius());
}

template<const int dim>
bool Intersect(const Polygon<dim>& r, const Point<dim>& p)
{
  Point<2> p2;

  return m_poly.numCorners() > 0 && m_orient.offset(p, p2).sqrMag() == 0
	 && Intersect(m_poly, p2);
}

template<const int dim>
bool IntersectProper(const Polygon<dim>& r, const Point<dim>& p)
{
  Point<2> p2;

  return m_poly.numCorners() > 0 && m_orient.offset(p, p2).sqrMag() == 0
	 && IntersectProper(m_poly, p2);
}

template<const int dim>
bool Contains(const Point<dim>& p, const Polygon<dim>& r)
{
  if(m_poly.numCorners() == 0)
    return true;

  for(int i = 1; i < m_poly.numCorners(); ++i)
    if(m_poly[i] != m_poly[0])
      return false;

  Point<2> p2;

  return m_orient.offset(p, p2).sqrMag() == 0 && p2 = m_poly[0];
}

template<const int dim>
bool ContainsProper(const Point<dim>& p, const Polygon<dim>& r)
{
  // Weird degenerate case, also works for dim == 2

  return r.numCorners() == 0;
}

template<const int dim>
bool Intersect(const Polygon<dim>& p, const AxisBox<dim>& b)
{
  if(p.m_points.size() == 0)
    return false;

  Point<2> p2;

  if(!p.m_orient.checkIntersect(b, p2))
    return false;

  Segment<dim> s;
  s.endpoint(0) = p.m_orient.convert(p.m_points.last());
  int next_end = 1;

  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i) {
    s.endpoint(next_end) = p.m_orient.convert(*i);
    if(Intersect(b, s))
      return true;
    next_end = next_end ? 0 : 1;
  }

  return Contains(p, p2);
}

template<const int dim>
bool IntersectProper(const Polygon<dim>& p, const AxisBox<dim>& b)
{
  if(p.m_points.size() == 0)
    return false;

  Point<2> p2;

  if(!p.m_orient.checkIntersectProper(b, p2))
    return false;

  Segment<dim> s;
  s.endpoint(0) = p.m_orient.convert(p.m_points.last());
  int next_end = 1;

  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i) {
    s.endpoint(next_end) = p.m_orient.convert(*i);
    if(IntersectProper(b, s))
      return true;
    next_end = next_end ? 0 : 1;
  }

  return ContainsProper(p, p2);
}

template<const int dim>
bool Contains(const Polygon<dim>& p, const AxisBox<dim>& b);
template<const int dim>
bool ContainsProper(const Polygon<dim>& p, const AxisBox<dim>& b);

template<const int dim>
bool Contains(const AxisBox<dim>& b, const Polygon<dim>& p)
{
  for(int i = 0; i < p.m_poly.numCorners(); ++i)
    if(!Contains(b, p.getCorner(i)))
      return false;

  return true;
}

template<const int dim>
bool ContainsProper(const AxisBox<dim>& b, const Polygon<dim>& p)
{
  for(int i = 0; i < p.m_poly.numCorners(); ++i)
    if(!ContainsProper(b, p.getCorner(i)))
      return false;

  return true;
}

template<const int dim>
bool Intersect(const Polygon<dim>& p, const Ball<dim>& b)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Point<2> c2;
  CoordType dist;

  dist = p.m_orient.offset(b.m_center, c2).sqrMag();
  dist -= b.m_radius * b.m_radius;

  if(dist < 0)
    return false;

  // FIXME get rid of sqrt()?
  return Intersect(p.m_poly, Ball<2>(c2, sqrt(dist)));
}

template<const int dim>
bool IntersectProper(const Polygon<dim>& p, const Ball<dim>& b)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Point<2> c2;
  CoordType dist;

  dist = p.m_orient.offset(b.m_center, c2).sqrMag();
  dist -= b.m_radius * b.m_radius;

  if(dist <= 0)
    return false;

  // FIXME get rid of sqrt()?
  return IntersectProper(p.m_poly, Ball<2>(c2, sqrt(dist)));
}

template<const int dim>
bool Contains(const Polygon<dim>& p, const Ball<dim>& b)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Point<2> c2;

  if(p.m_orient.offset(b.m_center, c2).sqrMag() != 0)
    return false;

  return Contains(p.m_poly, c2);
}

template<const int dim>
bool ContainsProper(const Polygon<dim>& p, const Ball<dim>& b)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Point<2> c2;

  if(p.m_orient.offset(b.m_center, c2).sqrMag() != 0)
    return false;

  return ContainsProper(p.m_poly, c2);
}

template<const int dim>
bool Contains(const Ball<dim>& b, const Polygon<dim>& p)
{
  if(p.m_poly.numCorners() == 0)
    return true;

  Point<2> c2;
  CoordType dist;

  dist = p.m_orient.offset(b.m_center, c2).sqrMag();

  dist = b.m_radius * b.m_radius - dist;

  if(dist < 0)
    return false;


  for(int i = 0; i != p.m_poly.numCorners(); ++i)
    if(dist < SquaredDistance(c2, p.m_poly[i]))
      return false;

  return true;
}

template<const int dim>
bool ContainsProper(const Ball<dim>& b, const Polygon<dim>& p)
{
  if(p.m_poly.numCorners() == 0)
    return true;

  Point<2> c2;
  CoordType dist;

  dist = p.m_orient.offset(b.m_center, c2).sqrMag();

  dist -= b.m_radius * b.m_radius;

  if(dist <= 0)
    return false;

  for(int i = 0; i != p.m_poly.numCorners(); ++i)
    if(dist <= SquaredDistance(c2, m_poly[i]))
      return false;

  return true;
}

template<const int dim>
bool Intersect(const Polygon<dim>& p, const Segment<dim>& s)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Point<2> p1, p2;
  CoordType d1, d2;
  Vector<dim> v1, v2;

  v1 = p.m_orient.offset(s.m_p1, p1);
  v2 = p.m_orient.offset(s.m_p2, p2);

  d1 = v1.sqrMag();
  d2 = v2.sqrMag();

  if(d1 == 0) {
    if(d2 == 0)
      return Intersect(p.m_poly, Segment<2>(p1, p2));
    else
      return Intersect(p.m_poly, p1);
  }
  else if(d2 == 0)
    return Intersect(p.m_poly, p2);

  if(Dot(v1, v2) > 0) // Both points on same side of sheet
    return false;

  d1 = sqrt(d1);
  d2 = sqrt(d2);
  Point<2> p_intersect;

  for(int i = 0; i < 2; ++i)
    p_intersect[i] = (p1[i] * d2 + p2[i] * d1) / (d1 + d2);

  return Intersect(p.m_poly, p_intersect);
}

template<const int dim>
bool IntersectProper(const Polygon<dim>& p, const Segment<dim>& s)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Point<2> p1, p2;
  CoordType d1, d2;
  Vector<dim> v1, v2;

  v1 = p.m_orient.offset(s.m_p1, p1);
  v2 = p.m_orient.offset(s.m_p2, p2);

  d1 = v1.sqrMag();
  d2 = v2.sqrMag();

  if(d1 == 0) {
    if(d2 == 0)
      return IntersectProper(p.m_poly, Segment<2>(p1, p2));
    else
      return IntersectProper(p.m_poly, p1);
  }
  else if(d2 == 0)
    return IntersectProper(p.m_poly, p2);

  if(Dot(v1, v2) > 0) // Both points on same side of sheet
    return false;

  d1 = sqrt(d1);
  d2 = sqrt(d2);
  Point<2> p_intersect;

  for(int i = 0; i < 2; ++i)
    p_intersect[i] = (p1[i] * d2 + p2[i] * d1) / (d1 + d2);

  return IntersectProper(p.m_poly, p_intersect);
}

template<const int dim>
bool Contains(const Polygon<dim>& p, const Segment<dim>& s)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Segment<2> s2;
  Vector<dim> v1, v2, v_zero;
  v_zero.zero;

  v1 = p.m_orient.offset(s.m_p1, s2.endpoint(0));
  if(v1 != v_zero)
    return false;
  v2 = p.m_orient.offset(s.m_p2, s2.endpoint(1));
  if(v2 != v_zero)
    return false;

  return Contains(p.m_poly, Segment<2>(p1, p2));
}

template<const int dim>
bool ContainsProper(const Polygon<dim>& p, const Segment<dim>& s)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Segment<2> s2;
  Vector<dim> v1, v2, v_zero;
  v_zero.zero();

  v1 = p.m_orient.offset(s.m_p1, s2.endpoint(0));
  if(v1 != v_zero)
    return false;
  v2 = p.m_orient.offset(s.m_p2, s2.endpoint(1));
  if(v2 != v_zero)
    return false;

  return ContainsProper(p.m_poly, s2);
}

template<const int dim>
bool Contains(const Segment<dim>& s, const Polygon<dim>& p)
{
  if(p.m_poly.numCorners() == 0)
    return true;

  // Expand the basis to include the segment, this deals well with
  // degenerate polygons

  Segment<2> s2;
  _Poly2Orient<dim> orient(p.m_orient);

  for(int i = 0; i < 2; ++i)
    if(!orient.expand(s.endpoint(i), s2.endpoint(i)))
      return false;

  return Contains(s2, p.m_poly);
}

template<const int dim>
bool ContainsProper(const Segment<dim>& s, const Polygon<dim>& p)
{
  if(p.m_poly.numCorners() == 0)
    return true;

  // Expand the basis to include the segment, this deals well with
  // degenerate polygons

  Segment<2> s2;
  _Poly2Orient<dim> orient(p.m_orient);

  for(int i = 0; i < 2; ++i)
    if(!orient.expand(s.endpoint(i), s2.endpoint(i)))
      return false;

  return ContainsProper(s2, p.m_poly);
}

template<const int dim>
bool Intersect(const Polygon<dim>& p, const RotBox<dim>& r)
{
  if(p.m_points.size() == 0)
    return false;

  _Poly2Orient<dim> orient(m_orient);
  // FIXME rotateInverse()
  orient.rotate(r.m_orient.inverse(), r.m_corner0);

  AxisBox<dim> b(r.m_corner0, r.m_corner0 + r.m_size);

  Point<2> p2;

  if(!orient.checkIntersect(b, p2))
    return false;

  Segment<dim> s;
  s.endpoint(0) = orient.convert(p.m_points.last());
  int next_end = 1;

  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i) {
    s.endpoint(next_end) = orient.convert(*i);
    if(Intersect(b, s))
      return true;
    next_end = next_end ? 0 : 1;
  }

  return Contains(p, p2);
}

template<const int dim>
bool IntersectProper(const Polygon<dim>& p, const RotBox<dim>& r)
{
  if(p.m_points.size() == 0)
    return false;

  _Poly2Orient<dim> orient(m_orient);
  // FIXME rotateInverse()
  orient.rotate(r.m_orient.inverse(), r.m_corner0);

  AxisBox<dim> b(r.m_corner0, r.m_corner0 + r.m_size);

  Point<2> p2;

  if(!orient.checkIntersectProper(b, p2))
    return false;

  Segment<dim> s;
  s.endpoint(0) = orient.convert(p.m_points.last());
  int next_end = 1;

  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i) {
    s.endpoint(next_end) = orient.convert(*i);
    if(IntersectProper(b, s))
      return true;
    next_end = next_end ? 0 : 1;
  }

  return ContainsProper(p, p2);
}

template<const int dim>
bool Contains(const Polygon<dim>& p, const RotBox<dim>& r);
template<const int dim>
bool ContainsProper(const Polygon<dim>& p, const RotBox<dim>& r);

template<const int dim>
bool Contains(const RotBox<dim>& r, const Polygon<dim>& p)
{
  if(p.m_poly.numCorners() == 0)
    return true;

  AxisBox<dim> b(r.m_corner0, r.m_corner0 + r.m_size);

  _Poly2Orient<dim> orient(p.m_orient);
  orient.rotate(r.m_orient.inverse(), r.m_corner0);

  for(int i = 0; i < p.m_poly.numCorners(); ++i)
    if(!Contains(b, orient.convert(p.m_poly[i])))
      return false;

  return true;
}

template<const int dim>
bool ContainsProper(const RotBox<dim>& r, const Polygon<dim>& p)
{
  if(p.m_poly.numCorners() == 0)
    return true;

  AxisBox<dim> b(r.m_corner0, r.m_corner0 + r.m_size);

  _Poly2Orient<dim> orient(p.m_orient);
  orient.rotate(r.m_orient.inverse(), r.m_corner0);

  for(int i = 0; i < p.m_poly.numCorners(); ++i)
    if(!ContainsProper(b, orient.convert(p.m_poly[i])))
      return false;

  return true;
}

template<const int dim>
bool Intersect(const Polygon<dim>& p1, const Polygon<dim>& p2);
template<const int dim>
bool IntersectProper(const Polygon<dim>& p1, const Polygon<dim>& p2);
template<const int dim>
bool Contains(const Polygon<dim>& outer, const Polygon<dim>& inner);
template<const int dim>
bool ContainsProper(const Polygon<dim>& outer, const Polygon<dim>& inner);

template<>
bool Intersect(const Polygon<2>& r, const Point<2>& p);
template<>
bool IntersectProper(const Polygon<2>& r, const Point<2>& p);
template<>
bool Contains(const Point<2>& p, const Polygon<2>& r);

template<>
bool Intersect(const Polygon<2>& p, const AxisBox<2>& b);
template<>
bool IntersectProper(const Polygon<2>& p, const AxisBox<2>& b);
template<>
bool Contains(const Polygon<2>& p, const AxisBox<2>& b);
template<>
bool ContainsProper(const Polygon<2>& p, const AxisBox<2>& b);
template<>
bool Contains(const AxisBox<2>& b, const Polygon<2>& p);
template<>
bool ContainsProper(const AxisBox<2>& b, const Polygon<2>& p);

template<>
bool Intersect(const Polygon<2>& p, const Ball<2>& b);
template<>
bool IntersectProper(const Polygon<2>& p, const Ball<2>& b);
template<>
bool Contains(const Polygon<2>& p, const Ball<2>& b);
template<>
bool ContainsProper(const Polygon<2>& p, const Ball<2>& b);
template<>
bool Contains(const Ball<2>& b, const Polygon<2>& p);
template<>
bool ContainsProper(const Ball<2>& b, const Polygon<2>& p);

template<>
bool Intersect(const Polygon<2>& r, const Segment<2>& s);
template<>
bool IntersectProper(const Polygon<2>& p, const Segment<2>& s);
template<>
bool Contains(const Polygon<2>& p, const Segment<2>& s);
template<>
bool ContainsProper(const Polygon<2>& p, const Segment<2>& s);
template<>
bool Contains(const Segment<2>& s, const Polygon<2>& p);
template<>
bool ContainsProper(const Segment<2>& s, const Polygon<2>& p);

template<>
bool Intersect(const Polygon<2>& p, const RotBox<2>& r);
template<>
bool IntersectProper(const Polygon<2>& p, const RotBox<2>& r);
template<>
bool Contains(const Polygon<2>& p, const RotBox<2>& r);
template<>
bool ContainsProper(const Polygon<2>& p, const RotBox<2>& r);
template<>
bool Contains(const RotBox<2>& r, const Polygon<2>& p);
template<>
bool ContainsProper(const RotBox<2>& r, const Polygon<2>& p);

template<>
bool Intersect(const Polygon<2>& p1, const Polygon<2>& p2);
template<>
bool IntersectProper(const Polygon<2>& p1, const Polygon<2>& p2);
template<>
bool Contains(const Polygon<2>& outer, const Polygon<2>& inner);
template<>
bool ContainsProper(const Polygon<2>& outer, const Polygon<2>& inner);

} // namespace WFMath

#endif  // WFMATH_POLYGON_FUNCS_H
