// polygon_funcs.h (Polygon<> intersection functions)
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
// Created: 2002-2-20

#ifndef WFMATH_POLYGON_INTERSECT_H
#define WFMATH_POLYGON_INTERSECT_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/polygon.h>

#warning "The Intersect() and Contains() functions involving WFMath::Polygon<>"
#warning "are still under development, and probably shouldn't be used yet."

namespace WFMath {

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

#endif  // WFMATH_POLYGON_INTERSECT_H
