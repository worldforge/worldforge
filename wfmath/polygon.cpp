// -*-C++-*-
// vector.cpp (Vector<> implementation)
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
// Created: 2002-1-4

#include "const.h"
#include "basis.h"
#include "vector.h"
#include "point.h"
#include "axisbox.h"
#include "ball.h"
#include "segment.h"
#include "rotbox.h"
#include "polygon.h"
#include "intersect.h"

using namespace WF::Math;

void _Poly2Reorient::reorient(Polygon<2>& poly, int skip = -1) const
{
  int end = poly.numCorners();

  switch(m_type) {
    case _WFMATH_POLY2REORIENT_NONE:
      return;
    case _WFMATH_POLY2REORIENT_CLEAR_AXIS1:
      for(int i = 0; i != end; ++i) {
        if(i == skip)
          continue;
        (poly[i])[0] = 0;
      }
      return;
    case _WFMATH_POLY2REORIENT_CLEAR_AXIS2:
      for(int i = 0; i != end; ++i) {
        if(i == skip)
          continue;
        (poly[i])[1] = 0;
      }
      return;
    case _WFMATH_POLY2REORIENT_CLEAR_BOTH_AXES:
      for(int i = 0; i != end; ++i) {
        if(i == skip)
          continue;
        (poly[i])[0] = 0;
        (poly[i])[1] = 0;
      }
      return;
    case _WFMATH_POLY2REORIENT_MOVE_AXIS2_TO_AXIS1:
      for(int i = 0; i != end; ++i) {
        if(i == skip)
           continue;
        (poly[i])[0] = (poly[i])[1];
        (poly[i])[1] = 0;
      }
      return;
    case _WFMATH_POLY2REORIENT_SCALE1_CLEAR2:
      for(int i = 0; i != end; ++i) {
        if(i == skip)
          continue;
        (poly[i])[0] *= m_scale;
        (poly[i])[1] = 0;
      }
      return;
    default:
      assert(false);
      return;
  }
}

//template<>
bool Polygon<2>::isEqualTo(const Polygon& p, double tolerance) const
{
  if(m_points.size() != p.m_points.size())
    return false;

  theConstIter i1 = m_points.begin(), i2 = p.m_points.begin(), end = m_points.end();

  while(i1 != end) {
    if(!i1->isEqualTo(*i2, tolerance))
      return false;
    ++i1;
    ++i2;
  }

  return true;
}

//template<>
bool Polygon<2>::operator< (const Polygon& p) const
{
  if(m_points.size() < p.m_points.size())
    return true;
  if(p.m_points.size() < m_points.size())
    return false;

  theConstIter i1 = m_points.begin(), i2 = p.m_points.begin(), end = m_points.end();

  while(i1 != end) {
    if(*i1 < *i2)
      return true;
    if(*i2 < *i1)
      return false;
    ++i1;
    ++i2;
  }

  return false;
}

//template<>
Polygon<2>& Polygon<2>::shift(const Vector<2>& v)
{
  for(theIter i = m_points.begin(); i != m_points.end(); ++i)
    *i += v;

  return *this;
}

//template<>
Polygon<2>& Polygon<2>::rotatePoint(const RotMatrix<2>& m, const Point<2>& p)
{
  for(theIter i = m_points.begin(); i != m_points.end(); ++i)
    i->rotate(m, p);

  return *this;
}

// The Polygon<2>/Point<2> intersection function was stolen directly
// from shape.cpp in libCoal

template<>
bool WF::Math::Intersect<2>(const Polygon<2>& r, const Point<2>& p)
{
  const Polygon<2>::theConstIter begin = r.m_points.begin(), end = r.m_points.end();
  bool hit = false;

  for (Polygon<2>::theConstIter i = begin, j = end - 1; i != end; j = i++) {
    bool vertically_between =
        (((*i)[1] <= p[1] && p[1] < (*j)[1]) ||
         ((*j)[1] <= p[1] && p[1] < (*i)[1]));

    if (!vertically_between)
      continue;

    CoordType x_intersect = (*i)[0] + FloatSubtract((*j)[0], (*i)[0])
			    * FloatSubtract(p[1], (*i)[1])
			    / FloatSubtract((*j)[1], (*i)[1]);

    if(IsFloatEqual(p[0], x_intersect))
      return true;

    if(p[0] < x_intersect)
            hit = !hit;
  }

  return hit;
}

template<>
bool WF::Math::IntersectProper<2>(const Polygon<2>& r, const Point<2>& p)
{
  const Polygon<2>::theConstIter begin = r.m_points.begin(), end = r.m_points.end();
  bool hit = false;

  for (Polygon<2>::theConstIter i = begin, j = end - 1; i != end; j = i++) {
    bool vertically_between =
        (((*i)[1] <= p[1] && p[1] < (*j)[1]) ||
         ((*j)[1] <= p[1] && p[1] < (*i)[1]));

    if (!vertically_between)
      continue;

    CoordType x_intersect = (*i)[0] + FloatSubtract((*j)[0], (*i)[0])
			    * FloatSubtract(p[1], (*i)[1])
			    / FloatSubtract((*j)[1], (*i)[1]);

    if(IsFloatEqual(p[0], x_intersect))
      return false;

    if(p[0] < x_intersect)
            hit = !hit;
  }

  return hit;
}

template<>
bool WF::Math::Contains<2>(const Point<2>& p, const Polygon<2>& r)
{
  for(unsigned int i = 0; i < r.m_points.size(); ++i)
    if(p != r.m_points[i])
      return false;

  return true;
}

template<>
bool WF::Math::Intersect<2>(const Polygon<2>& p, const AxisBox<2>& b)
{
  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();
  bool hit = false;

  for (Polygon<2>::theConstIter i = begin, j = end - 1; i != end; j = i++) {
    bool low_vertically_between =
        (((*i)[1] <= b.m_low[1] && b.m_low[1] < (*j)[1]) ||
         ((*j)[1] <= b.m_low[1] && b.m_low[1] < (*i)[1]));
    bool low_horizontally_between =
        (((*i)[0] <= b.m_low[0] && b.m_low[0] < (*j)[0]) ||
         ((*j)[0] <= b.m_low[0] && b.m_low[0] < (*i)[0]));
    bool high_vertically_between =
        (((*i)[1] <= b.m_high[1] && b.m_high[1] < (*j)[1]) ||
         ((*j)[1] <= b.m_high[1] && b.m_high[1] < (*i)[1]));
    bool high_horizontally_between =
        (((*i)[0] <= b.m_high[0] && b.m_high[0] < (*j)[0]) ||
         ((*j)[0] <= b.m_high[0] && b.m_high[0] < (*i)[0]));

    CoordType xdiff = FloatSubtract((*j)[0], (*i)[0]);
    CoordType ydiff = FloatSubtract((*j)[0], (*i)[0]);

    if(low_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (*i)[0] + FloatSubtract(b.m_low[1], (*i)[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(b.m_low[0], x_intersect) || IsFloatEqual(b.m_high[0], x_intersect))
        return true;
      if(b.m_low[0] < x_intersect && b.m_high[0] > x_intersect)
        return true;

      // Also check for point inclusion here, only need to do this for one point
      if(b.m_low[0] < x_intersect)
        hit = !hit;
    }

    if(low_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (*i)[1] + FloatSubtract(b.m_low[0], (*i)[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(b.m_low[1], y_intersect) || IsFloatEqual(b.m_high[1], y_intersect))
        return true;
      if(b.m_low[1] < y_intersect && b.m_high[1] > y_intersect)
        return true;
    }

    if(high_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (*i)[0] + FloatSubtract(b.m_high[1], (*i)[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(b.m_low[0], x_intersect) || IsFloatEqual(b.m_high[0], x_intersect))
        return true;
      if(b.m_low[0] < x_intersect && b.m_high[0] > x_intersect)
        return true;
    }

    if(high_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (*i)[1] + FloatSubtract(b.m_high[0], (*i)[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(b.m_low[1], y_intersect) || IsFloatEqual(b.m_high[1], y_intersect))
        return true;
      if(b.m_low[1] < y_intersect && b.m_high[1] > y_intersect)
        return true;
    }
  }

  return hit;
}

template<>
bool WF::Math::IntersectProper<2>(const Polygon<2>& p, const AxisBox<2>& b)
{
  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();
  bool hit = false;

  for (Polygon<2>::theConstIter i = begin, j = end - 1; i != end; j = i++) {
    bool low_vertically_between =
        (((*i)[1] <= b.m_low[1] && b.m_low[1] < (*j)[1]) ||
         ((*j)[1] <= b.m_low[1] && b.m_low[1] < (*i)[1]));
    bool low_horizontally_between =
        (((*i)[0] <= b.m_low[0] && b.m_low[0] < (*j)[0]) ||
         ((*j)[0] <= b.m_low[0] && b.m_low[0] < (*i)[0]));
    bool high_vertically_between =
        (((*i)[1] <= b.m_high[1] && b.m_high[1] < (*j)[1]) ||
         ((*j)[1] <= b.m_high[1] && b.m_high[1] < (*i)[1]));
    bool high_horizontally_between =
        (((*i)[0] <= b.m_high[0] && b.m_high[0] < (*j)[0]) ||
         ((*j)[0] <= b.m_high[0] && b.m_high[0] < (*i)[0]));

    CoordType xdiff = FloatSubtract((*j)[0], (*i)[0]);
    CoordType ydiff = FloatSubtract((*j)[0], (*i)[0]);

    if(low_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (*i)[0] + FloatSubtract(b.m_low[1], (*i)[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(b.m_low[0], x_intersect) || IsFloatEqual(b.m_high[0], x_intersect))
        return false;
      if(b.m_low[0] < x_intersect && b.m_high[0] > x_intersect)
        return true;

      // Also check for point inclusion here, only need to do this for one point
      if(b.m_low[0] < x_intersect)
        hit = !hit;
    }

    if(low_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (*i)[1] + FloatSubtract(b.m_low[0], (*i)[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(b.m_low[1], y_intersect) || IsFloatEqual(b.m_high[1], y_intersect))
        return false;
      if(b.m_low[1] < y_intersect && b.m_high[1] > y_intersect)
        return true;
    }

    if(high_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (*i)[0] + FloatSubtract(b.m_high[1], (*i)[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(b.m_low[0], x_intersect) || IsFloatEqual(b.m_high[0], x_intersect))
        return false;
      if(b.m_low[0] < x_intersect && b.m_high[0] > x_intersect)
        return true;
    }

    if(high_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (*i)[1] + FloatSubtract(b.m_high[0], (*i)[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(b.m_low[1], y_intersect) || IsFloatEqual(b.m_high[1], y_intersect))
        return false;
      if(b.m_low[1] < y_intersect && b.m_high[1] > y_intersect)
        return true;
    }
  }

  return hit;
}

template<>
bool WF::Math::Contains<2>(const Polygon<2>& p, const AxisBox<2>& b)
{
  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();
  bool hit = false;

  for (Polygon<2>::theConstIter i = begin, j = end - 1; i != end; j = i++) {
    bool low_vertically_between =
        (((*i)[1] <= b.m_low[1] && b.m_low[1] < (*j)[1]) ||
         ((*j)[1] <= b.m_low[1] && b.m_low[1] < (*i)[1]));
    bool low_horizontally_between =
        (((*i)[0] <= b.m_low[0] && b.m_low[0] < (*j)[0]) ||
         ((*j)[0] <= b.m_low[0] && b.m_low[0] < (*i)[0]));
    bool high_vertically_between =
        (((*i)[1] <= b.m_high[1] && b.m_high[1] < (*j)[1]) ||
         ((*j)[1] <= b.m_high[1] && b.m_high[1] < (*i)[1]));
    bool high_horizontally_between =
        (((*i)[0] <= b.m_high[0] && b.m_high[0] < (*j)[0]) ||
         ((*j)[0] <= b.m_high[0] && b.m_high[0] < (*i)[0]));

    CoordType xdiff = FloatSubtract((*j)[0], (*i)[0]);
    CoordType ydiff = FloatSubtract((*j)[0], (*i)[0]);

    if(low_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (*i)[0] + FloatSubtract(b.m_low[1], (*i)[1])
			      * xdiff / ydiff;

      // Use this twice, use a temporary value
      bool is_low = !IsFloatEqual(b.m_low[0], x_intersect)
		    && b.m_low[0] < x_intersect;

      if(is_low && !IsFloatEqual(b.m_high[0], x_intersect)
         && b.m_high[0] > x_intersect)
        return false;

      // Also check for point inclusion here, only need to do this for one point
      if(is_low)
        hit = !hit;
    }

    if(low_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (*i)[1] + FloatSubtract(b.m_low[0], (*i)[0])
			      * ydiff / xdiff;

      if(!IsFloatEqual(b.m_low[1], y_intersect)
	 && !IsFloatEqual(b.m_high[1], y_intersect)
         && b.m_low[1] < y_intersect && b.m_high[1] > y_intersect)
        return false;
    }

    if(high_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (*i)[0] + FloatSubtract(b.m_high[1], (*i)[1])
			      * xdiff / ydiff;

      if(!IsFloatEqual(b.m_low[0], x_intersect)
	 && !IsFloatEqual(b.m_high[0], x_intersect)
         && b.m_low[0] < x_intersect && b.m_high[0] > x_intersect)
        return false;
    }

    if(high_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (*i)[1] + FloatSubtract(b.m_high[0], (*i)[0])
			      * ydiff / xdiff;

      if(!IsFloatEqual(b.m_low[1], y_intersect)
	 && !IsFloatEqual(b.m_high[1], y_intersect)
         && b.m_low[1] < y_intersect && b.m_high[1] > y_intersect)
        return false;
    }
  }

  return hit;
}

template<>
bool WF::Math::ContainsProper<2>(const Polygon<2>& p, const AxisBox<2>& b)
{
  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();
  bool hit = false;

  for (Polygon<2>::theConstIter i = begin, j = end - 1; i != end; j = i++) {
    bool low_vertically_between =
        (((*i)[1] <= b.m_low[1] && b.m_low[1] < (*j)[1]) ||
         ((*j)[1] <= b.m_low[1] && b.m_low[1] < (*i)[1]));
    bool low_horizontally_between =
        (((*i)[0] <= b.m_low[0] && b.m_low[0] < (*j)[0]) ||
         ((*j)[0] <= b.m_low[0] && b.m_low[0] < (*i)[0]));
    bool high_vertically_between =
        (((*i)[1] <= b.m_high[1] && b.m_high[1] < (*j)[1]) ||
         ((*j)[1] <= b.m_high[1] && b.m_high[1] < (*i)[1]));
    bool high_horizontally_between =
        (((*i)[0] <= b.m_high[0] && b.m_high[0] < (*j)[0]) ||
         ((*j)[0] <= b.m_high[0] && b.m_high[0] < (*i)[0]));

    CoordType xdiff = FloatSubtract((*j)[0], (*i)[0]);
    CoordType ydiff = FloatSubtract((*j)[0], (*i)[0]);

    if(low_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (*i)[0] + FloatSubtract(b.m_low[1], (*i)[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(b.m_low[0], x_intersect) || IsFloatEqual(b.m_high[0], x_intersect))
        return false;
      if(b.m_low[0] < x_intersect && b.m_high[0] > x_intersect)
        return false;

      // Also check for point inclusion here, only need to do this for one point
      if(b.m_low[0] < x_intersect)
        hit = !hit;
    }

    if(low_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (*i)[1] + FloatSubtract(b.m_low[0], (*i)[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(b.m_low[1], y_intersect) || IsFloatEqual(b.m_high[1], y_intersect))
        return false;
      if(b.m_low[1] < y_intersect && b.m_high[1] > y_intersect)
        return false;
    }

    if(high_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (*i)[0] + FloatSubtract(b.m_high[1], (*i)[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(b.m_low[0], x_intersect) || IsFloatEqual(b.m_high[0], x_intersect))
        return false;
      if(b.m_low[0] < x_intersect && b.m_high[0] > x_intersect)
        return false;
    }

    if(high_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (*i)[1] + FloatSubtract(b.m_high[0], (*i)[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(b.m_low[1], y_intersect) || IsFloatEqual(b.m_high[1], y_intersect))
        return false;
      if(b.m_low[1] < y_intersect && b.m_high[1] > y_intersect)
        return false;
    }
  }

  return hit;
}

template<>
bool WF::Math::Contains<2>(const AxisBox<2>& b, const Polygon<2>& p)
{
  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i)
    if(!Contains(b, *i))
      return false;

  return true;
}

template<>
bool WF::Math::ContainsProper<2>(const AxisBox<2>& b, const Polygon<2>& p)
{
  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i)
    if(!ContainsProper(b, *i))
      return false;

  return true;
}

template<>
bool WF::Math::Intersect<2>(const Polygon<2>& p, const Ball<2>& b)
{
  if(Contains(p, b.m_center))
    return true;

  Segment<2> s2;
  s2.endpoint(0) = p.m_points.back();
  int next_end = 1;

  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i) {
    s2.endpoint(next_end) = *i;
    if(Intersect(s2, b))
      return true;
    next_end = next_end ? 0 : 1;
  }

  return false;
}

template<>
bool WF::Math::IntersectProper<2>(const Polygon<2>& p, const Ball<2>& b)
{
  if(ContainsProper(p, b.m_center))
    return true;

  Segment<2> s2;
  s2.endpoint(0) = p.m_points.back();
  int next_end = 1;

  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i) {
    s2.endpoint(next_end) = *i;
    if(IntersectProper(s2, b))
      return true;
    next_end = next_end ? 0 : 1;
  }

  return false;
}

template<>
bool WF::Math::Contains<2>(const Polygon<2>& p, const Ball<2>& b)
{
  if(!Contains(p, b.m_center))
    return false;

  Segment<2> s2;
  s2.endpoint(0) = p.m_points.back();
  int next_end = 1;

  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i) {
    s2.endpoint(next_end) = *i;
    if(IntersectProper(s2, b))
      return false;
    next_end = next_end ? 0 : 1;
  }

  return true;
}

template<>
bool WF::Math::ContainsProper<2>(const Polygon<2>& p, const Ball<2>& b)
{
  if(!ContainsProper(p, b.m_center))
    return false;

  Segment<2> s2;
  s2.endpoint(0) = p.m_points.back();
  int next_end = 1;

  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i) {
    s2.endpoint(next_end) = *i;
    if(Intersect(s2, b))
      return false;
    next_end = next_end ? 0 : 1;
  }

  return true;
}

template<>
bool WF::Math::Contains<2>(const Ball<2>& b, const Polygon<2>& p)
{
  CoordType sqr_dist = b.m_radius * b.m_radius;

  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i)
    if(SquaredDistance(b.m_center, *i) > sqr_dist)
      return false;

  return true;
}

template<>
bool WF::Math::ContainsProper<2>(const Ball<2>& b, const Polygon<2>& p)
{
  CoordType sqr_dist = b.m_radius * b.m_radius;

  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i)
    if(SquaredDistance(b.m_center, *i) >= sqr_dist)
      return false;

  return true;
}

template<>
bool WF::Math::Intersect<2>(const Polygon<2>& p, const Segment<2>& s)
{
  if(Contains(p, s.endpoint(0)))
    return true;

  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();

  Segment<2> s2;

  s2.endpoint(0) = p.m_points.back();
  int next_point = 1;

  for(Polygon<2>::theConstIter i = begin; i != end; ++i) {
    s2.endpoint(next_point) = *i;
    if(Intersect(s, s2))
      return true;
    next_point = next_point ? 0 : 1;
  }

  return false;
}

template<>
bool WF::Math::IntersectProper<2>(const Polygon<2>& p, const Segment<2>& s)
{
  if(ContainsProper(p, s.endpoint(0)))
    return true;

  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();

  Segment<2> s2;

  s2.endpoint(0) = p.m_points.back();
  int next_point = 1;

  for(Polygon<2>::theConstIter i = begin; i != end; ++i) {
    s2.endpoint(next_point) = *i;
    if(IntersectProper(s, s2))
      return true;
    next_point = next_point ? 0 : 1;
  }

  return false;
}

template<>
bool WF::Math::Contains<2>(const Polygon<2>& p, const Segment<2>& s)
{
  if(!Contains(p, s.endpoint(0)))
    return false;

  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();

  Segment<2> s2;

  s2.endpoint(0) = p.m_points.back();
  int next_point = 1;

  for(Polygon<2>::theConstIter i = begin; i != end; ++i) {
    s2.endpoint(next_point) = *i;
    if(IntersectProper(s2, s)) // FIXME crossing at corners
      return false;
    next_point = next_point ? 0 : 1;
  }

  return true;
}

template<>
bool WF::Math::ContainsProper<2>(const Polygon<2>& p, const Segment<2>& s)
{
  if(!ContainsProper(p, s.endpoint(0)))
    return false;

  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();

  Segment<2> s2;

  s2.endpoint(0) = p.m_points.back();
  int next_point = 1;

  for(Polygon<2>::theConstIter i = begin; i != end; ++i) {
    s2.endpoint(next_point) = *i;
    if(Intersect(s2, s))
      return false;
    next_point = next_point ? 0 : 1;
  }

  return true;
}

template<>
bool WF::Math::Contains<2>(const Segment<2>& s, const Polygon<2>& p)
{
  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i)
    if(!Contains(s, *i))
      return false;

  return true;
}

template<>
bool WF::Math::ContainsProper<2>(const Segment<2>& s, const Polygon<2>& p)
{
  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i)
    if(!ContainsProper(s, *i))
      return false;

  return true;
}

template<>
bool WF::Math::Intersect<2>(const Polygon<2>& p, const RotBox<2>& r)
{
  CoordType m_low[2], m_high[2];

  for(int j = 0; j < 2; ++j) {
    if(r.m_size[j] > 0) {
      m_low[j] = r.m_corner0[j];
      m_high[j] = FloatAdd(r.m_corner0[j], r.m_size[j]);
    }
    else {
      m_high[j] = r.m_corner0[j];
      m_low[j] = FloatAdd(r.m_corner0[j], r.m_size[j]);
    }
  }

  Point<2> ends[2];
  ends[0] = p.m_points.back();
  // FIXME Point<>::rotateInverse()
  ends[0].rotate(r.m_orient.inverse(), r.m_corner0);
  int next_end = 1;

  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();
  bool hit = false;

  for (Polygon<2>::theConstIter i = begin; i != end; ++i) {
    ends[next_end] = *i;
    // FIXME Point<>::rotateInverse()
    ends[next_end].rotate(r.m_orient.inverse(), r.m_corner0);
    next_end = next_end ? 0 : 1;

    bool low_vertically_between =
        (((ends[0])[1] <= m_low[1] && m_low[1] < (ends[1])[1]) ||
         ((ends[1])[1] <= m_low[1] && m_low[1] < (ends[0])[1]));
    bool low_horizontally_between =
        (((ends[0])[0] <= m_low[0] && m_low[0] < (ends[1])[0]) ||
         ((ends[1])[0] <= m_low[0] && m_low[0] < (ends[0])[0]));
    bool high_vertically_between =
        (((ends[0])[1] <= m_high[1] && m_high[1] < (ends[1])[1]) ||
         ((ends[1])[1] <= m_high[1] && m_high[1] < (ends[0])[1]));
    bool high_horizontally_between =
        (((ends[0])[0] <= m_high[0] && m_high[0] < (ends[1])[0]) ||
         ((ends[1])[0] <= m_high[0] && m_high[0] < (ends[0])[0]));

    CoordType xdiff = FloatSubtract((ends[1])[0], (ends[0])[0]);
    CoordType ydiff = FloatSubtract((ends[1])[0], (ends[0])[0]);

    if(low_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (ends[0])[0] + FloatSubtract(m_low[1], (ends[0])[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(m_low[0], x_intersect) || IsFloatEqual(m_high[0], x_intersect))
        return true;
      if(m_low[0] < x_intersect && m_high[0] > x_intersect)
        return true;

      // Also check for point inclusion here, only need to do this for one point
      if(m_low[0] < x_intersect)
        hit = !hit;
    }

    if(low_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (ends[0])[1] + FloatSubtract(m_low[0], (ends[0])[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(m_low[1], y_intersect) || IsFloatEqual(m_high[1], y_intersect))
        return true;
      if(m_low[1] < y_intersect && m_high[1] > y_intersect)
        return true;
    }

    if(high_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (ends[0])[0] + FloatSubtract(m_high[1], (ends[0])[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(m_low[0], x_intersect) || IsFloatEqual(m_high[0], x_intersect))
        return true;
      if(m_low[0] < x_intersect && m_high[0] > x_intersect)
        return true;
    }

    if(high_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (ends[0])[1] + FloatSubtract(m_high[0], (ends[0])[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(m_low[1], y_intersect) || IsFloatEqual(m_high[1], y_intersect))
        return true;
      if(m_low[1] < y_intersect && m_high[1] > y_intersect)
        return true;
    }
  }

  return hit;
}

template<>
bool WF::Math::IntersectProper<2>(const Polygon<2>& p, const RotBox<2>& r)
{
  CoordType m_low[2], m_high[2];

  for(int j = 0; j < 2; ++j) {
    if(r.m_size[j] > 0) {
      m_low[j] = r.m_corner0[j];
      m_high[j] = FloatAdd(r.m_corner0[j], r.m_size[j]);
    }
    else {
      m_high[j] = r.m_corner0[j];
      m_low[j] = FloatAdd(r.m_corner0[j], r.m_size[j]);
    }
  }

  Point<2> ends[2];
  ends[0] = p.m_points.back();
  // FIXME Point<>::rotateInverse()
  ends[0].rotate(r.m_orient.inverse(), r.m_corner0);
  int next_end = 1;

  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();
  bool hit = false;

  for (Polygon<2>::theConstIter i = begin; i != end; ++i) {
    ends[next_end] = *i;
    // FIXME Point<>::rotateInverse()
    ends[next_end].rotate(r.m_orient.inverse(), r.m_corner0);
    next_end = next_end ? 0 : 1;

    bool low_vertically_between =
        (((ends[0])[1] <= m_low[1] && m_low[1] < (ends[1])[1]) ||
         ((ends[1])[1] <= m_low[1] && m_low[1] < (ends[0])[1]));
    bool low_horizontally_between =
        (((ends[0])[0] <= m_low[0] && m_low[0] < (ends[1])[0]) ||
         ((ends[1])[0] <= m_low[0] && m_low[0] < (ends[0])[0]));
    bool high_vertically_between =
        (((ends[0])[1] <= m_high[1] && m_high[1] < (ends[1])[1]) ||
         ((ends[1])[1] <= m_high[1] && m_high[1] < (ends[0])[1]));
    bool high_horizontally_between =
        (((ends[0])[0] <= m_high[0] && m_high[0] < (ends[1])[0]) ||
         ((ends[1])[0] <= m_high[0] && m_high[0] < (ends[0])[0]));

    CoordType xdiff = FloatSubtract((ends[1])[0], (ends[0])[0]);
    CoordType ydiff = FloatSubtract((ends[1])[0], (ends[0])[0]);

    if(low_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (ends[0])[0] + FloatSubtract(m_low[1], (ends[0])[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(m_low[0], x_intersect) || IsFloatEqual(m_high[0], x_intersect))
        return false;
      if(m_low[0] < x_intersect && m_high[0] > x_intersect)
        return true;

      // Also check for point inclusion here, only need to do this for one point
      if(m_low[0] < x_intersect)
        hit = !hit;
    }

    if(low_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (ends[0])[1] + FloatSubtract(m_low[0], (ends[0])[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(m_low[1], y_intersect) || IsFloatEqual(m_high[1], y_intersect))
        return false;
      if(m_low[1] < y_intersect && m_high[1] > y_intersect)
        return true;
    }

    if(high_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (ends[0])[0] + FloatSubtract(m_high[1], (ends[0])[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(m_low[0], x_intersect) || IsFloatEqual(m_high[0], x_intersect))
        return false;
      if(m_low[0] < x_intersect && m_high[0] > x_intersect)
        return true;
    }

    if(high_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (ends[0])[1] + FloatSubtract(m_high[0], (ends[0])[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(m_low[1], y_intersect) || IsFloatEqual(m_high[1], y_intersect))
        return false;
      if(m_low[1] < y_intersect && m_high[1] > y_intersect)
        return true;
    }
  }

  return hit;
}

template<>
bool WF::Math::Contains<2>(const Polygon<2>& p, const RotBox<2>& r)
{
  CoordType m_low[2], m_high[2];

  for(int j = 0; j < 2; ++j) {
    if(r.m_size[j] > 0) {
      m_low[j] = r.m_corner0[j];
      m_high[j] = FloatAdd(r.m_corner0[j], r.m_size[j]);
    }
    else {
      m_high[j] = r.m_corner0[j];
      m_low[j] = FloatAdd(r.m_corner0[j], r.m_size[j]);
    }
  }

  Point<2> ends[2];
  ends[0] = p.m_points.back();
  // FIXME Point<>::rotateInverse()
  ends[0].rotate(r.m_orient.inverse(), r.m_corner0);
  int next_end = 1;

  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();
  bool hit = false;

  for (Polygon<2>::theConstIter i = begin; i != end; ++i) {
    ends[next_end] = *i;
    // FIXME Point<>::rotateInverse()
    ends[next_end].rotate(r.m_orient.inverse(), r.m_corner0);
    next_end = next_end ? 0 : 1;

    bool low_vertically_between =
        (((ends[0])[1] <= m_low[1] && m_low[1] < (ends[1])[1]) ||
         ((ends[1])[1] <= m_low[1] && m_low[1] < (ends[0])[1]));
    bool low_horizontally_between =
        (((ends[0])[0] <= m_low[0] && m_low[0] < (ends[1])[0]) ||
         ((ends[1])[0] <= m_low[0] && m_low[0] < (ends[0])[0]));
    bool high_vertically_between =
        (((ends[0])[1] <= m_high[1] && m_high[1] < (ends[1])[1]) ||
         ((ends[1])[1] <= m_high[1] && m_high[1] < (ends[0])[1]));
    bool high_horizontally_between =
        (((ends[0])[0] <= m_high[0] && m_high[0] < (ends[1])[0]) ||
         ((ends[1])[0] <= m_high[0] && m_high[0] < (ends[0])[0]));

    CoordType xdiff = FloatSubtract((ends[1])[0], (ends[0])[0]);
    CoordType ydiff = FloatSubtract((ends[1])[0], (ends[0])[0]);

    if(low_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (ends[0])[0] + FloatSubtract(m_low[1], (ends[0])[1])
			      * xdiff / ydiff;

      // Use this twice, use a temporary value
      bool is_low = !IsFloatEqual(m_low[0], x_intersect)
		    && m_low[0] < x_intersect;

      if(is_low && !IsFloatEqual(m_high[0], x_intersect)
         && m_high[0] > x_intersect)
        return false;

      // Also check for point inclusion here, only need to do this for one point
      if(is_low)
        hit = !hit;
    }

    if(low_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (ends[0])[1] + FloatSubtract(m_low[0], (ends[0])[0])
			      * ydiff / xdiff;

      if(!IsFloatEqual(m_low[1], y_intersect)
	 && !IsFloatEqual(m_high[1], y_intersect)
         && m_low[1] < y_intersect && m_high[1] > y_intersect)
        return false;
    }

    if(high_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (ends[0])[0] + FloatSubtract(m_high[1], (ends[0])[1])
			      * xdiff / ydiff;

      if(!IsFloatEqual(m_low[0], x_intersect)
	 && !IsFloatEqual(m_high[0], x_intersect)
         && m_low[0] < x_intersect && m_high[0] > x_intersect)
        return false;
    }

    if(high_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (ends[0])[1] + FloatSubtract(m_high[0], (ends[0])[0])
			      * ydiff / xdiff;

      if(!IsFloatEqual(m_low[1], y_intersect)
	 && !IsFloatEqual(m_high[1], y_intersect)
         && m_low[1] < y_intersect && m_high[1] > y_intersect)
        return false;
    }
  }

  return hit;
}

template<>
bool WF::Math::ContainsProper<2>(const Polygon<2>& p, const RotBox<2>& r)
{
  CoordType m_low[2], m_high[2];

  for(int j = 0; j < 2; ++j) {
    if(r.m_size[j] > 0) {
      m_low[j] = r.m_corner0[j];
      m_high[j] = FloatAdd(r.m_corner0[j], r.m_size[j]);
    }
    else {
      m_high[j] = r.m_corner0[j];
      m_low[j] = FloatAdd(r.m_corner0[j], r.m_size[j]);
    }
  }

  Point<2> ends[2];
  ends[0] = p.m_points.back();
  // FIXME Point<>::rotateInverse()
  ends[0].rotate(r.m_orient.inverse(), r.m_corner0);
  int next_end = 1;

  const Polygon<2>::theConstIter begin = p.m_points.begin(), end = p.m_points.end();
  bool hit = false;

  for (Polygon<2>::theConstIter i = begin; i != end; ++i) {
    ends[next_end] = *i;
    // FIXME Point<>::rotateInverse()
    ends[next_end].rotate(r.m_orient.inverse(), r.m_corner0);
    next_end = next_end ? 0 : 1;

    bool low_vertically_between =
        (((ends[0])[1] <= m_low[1] && m_low[1] < (ends[1])[1]) ||
         ((ends[1])[1] <= m_low[1] && m_low[1] < (ends[0])[1]));
    bool low_horizontally_between =
        (((ends[0])[0] <= m_low[0] && m_low[0] < (ends[1])[0]) ||
         ((ends[1])[0] <= m_low[0] && m_low[0] < (ends[0])[0]));
    bool high_vertically_between =
        (((ends[0])[1] <= m_high[1] && m_high[1] < (ends[1])[1]) ||
         ((ends[1])[1] <= m_high[1] && m_high[1] < (ends[0])[1]));
    bool high_horizontally_between =
        (((ends[0])[0] <= m_high[0] && m_high[0] < (ends[1])[0]) ||
         ((ends[1])[0] <= m_high[0] && m_high[0] < (ends[0])[0]));

    CoordType xdiff = FloatSubtract((ends[1])[0], (ends[0])[0]);
    CoordType ydiff = FloatSubtract((ends[1])[0], (ends[0])[0]);

    if(low_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (ends[0])[0] + FloatSubtract(m_low[1], (ends[0])[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(m_low[0], x_intersect) || IsFloatEqual(m_high[0], x_intersect))
        return false;
      if(m_low[0] < x_intersect && m_high[0] > x_intersect)
        return false;

      // Also check for point inclusion here, only need to do this for one point
      if(m_low[0] < x_intersect)
        hit = !hit;
    }

    if(low_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (ends[0])[1] + FloatSubtract(m_low[0], (ends[0])[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(m_low[1], y_intersect) || IsFloatEqual(m_high[1], y_intersect))
        return false;
      if(m_low[1] < y_intersect && m_high[1] > y_intersect)
        return false;
    }

    if(high_vertically_between) { // Check for edge intersect
      CoordType x_intersect = (ends[0])[0] + FloatSubtract(m_high[1], (ends[0])[1])
			      * xdiff / ydiff;

      if(IsFloatEqual(m_low[0], x_intersect) || IsFloatEqual(m_high[0], x_intersect))
        return false;
      if(m_low[0] < x_intersect && m_high[0] > x_intersect)
        return false;
    }

    if(high_horizontally_between) { // Check for edge intersect
      CoordType y_intersect = (ends[0])[1] + FloatSubtract(m_high[0], (ends[0])[0])
			      * ydiff / xdiff;

      if(IsFloatEqual(m_low[1], y_intersect) || IsFloatEqual(m_high[1], y_intersect))
        return false;
      if(m_low[1] < y_intersect && m_high[1] > y_intersect)
        return false;
    }
  }

  return hit;
}

template<>
bool WF::Math::Contains<2>(const RotBox<2>& r, const Polygon<2>& p)
{
  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i)
    if(!Contains(r, *i))
      return false;

  return true;
}

template<>
bool WF::Math::ContainsProper<2>(const RotBox<2>& r, const Polygon<2>& p)
{
  for(Polygon<2>::theConstIter i = p.m_points.begin(); i != p.m_points.end(); ++i)
    if(!ContainsProper(r, *i))
      return false;

  return true;
}

template<>
bool WF::Math::Intersect<2>(const Polygon<2>& p1, const Polygon<2>& p2)
{
  Polygon<2>::theConstIter begin1 = p1.m_points.begin(), end1 = p1.m_points.end();
  Polygon<2>::theConstIter begin2 = p2.m_points.begin(), end2 = p2.m_points.end();
  Segment<2> s1, s2;
  int next_end1, next_end2;

  s1.endpoint(0) = p1.m_points.back();
  s2.endpoint(0) = p2.m_points.back();
  next_end1 = next_end2 = 1;
  for(Polygon<2>::theConstIter i1 = begin1; i1 != end1; ++i1) {
    s1.endpoint(next_end1) = *i1;
    next_end1 = next_end1 ? 0 : 1;

    for(Polygon<2>::theConstIter i2 = begin2; i2 != end2; ++i2) {
      s2.endpoint(next_end2) = *i2;
      next_end2 = next_end2 ? 0 : 1;

      if(Intersect(s1, s2))
        return true;
    }
  }

  return Contains(p1, p2.m_points.front())
      || Contains(p2, p1.m_points.front());
}

template<>
bool WF::Math::IntersectProper<2>(const Polygon<2>& p1, const Polygon<2>& p2)
{
  Polygon<2>::theConstIter begin1 = p1.m_points.begin(), end1 = p1.m_points.end();
  Polygon<2>::theConstIter begin2 = p2.m_points.begin(), end2 = p2.m_points.end();
  Segment<2> s1, s2;
  int next_end1, next_end2;

  s1.endpoint(0) = p1.m_points.back();
  s2.endpoint(0) = p2.m_points.back();
  next_end1 = next_end2 = 1;
  for(Polygon<2>::theConstIter i1 = begin1; i1 != end1; ++i1) {
    s1.endpoint(next_end1) = *i1;
    next_end1 = next_end1 ? 0 : 1;

    for(Polygon<2>::theConstIter i2 = begin2; i2 != end2; ++i2) {
      s2.endpoint(next_end2) = *i2;
      next_end2 = next_end2 ? 0 : 1;

      if(IntersectProper(s1, s2))
        return true;
    }
  }

  return ContainsProper(p1, p2.m_points.front())
      || ContainsProper(p2, p1.m_points.front());
}

template<>
bool WF::Math::Contains<2>(const Polygon<2>& outer, const Polygon<2>& inner)
{
  if(!Contains(outer, inner.m_points.front()))
    return false;

  Polygon<2>::theConstIter begin1 = outer.m_points.begin(), end1 = outer.m_points.end();
  Polygon<2>::theConstIter begin2 = inner.m_points.begin(), end2 = inner.m_points.end();
  Segment<2> s1, s2;
  int next_end1, next_end2;

  s1.endpoint(0) = outer.m_points.back();
  s2.endpoint(0) = inner.m_points.back();
  next_end1 = next_end2 = 1;
  for(Polygon<2>::theConstIter i1 = begin1; i1 != end1; ++i1) {
    s1.endpoint(next_end1) = *i1;
    next_end1 = next_end1 ? 0 : 1;

    for(Polygon<2>::theConstIter i2 = begin2; i2 != end2; ++i2) {
      s2.endpoint(next_end2) = *i2;
      next_end2 = next_end2 ? 0 : 1;

      if(IntersectProper(s1, s2)) // FIXME crossing at corners
        return false;
    }
  }

  return true;
}

template<>
bool WF::Math::ContainsProper<2>(const Polygon<2>& outer, const Polygon<2>& inner)
{
  if(!ContainsProper(outer, inner.m_points.front()))
    return false;

  Polygon<2>::theConstIter begin1 = outer.m_points.begin(), end1 = outer.m_points.end();
  Polygon<2>::theConstIter begin2 = inner.m_points.begin(), end2 = inner.m_points.end();
  Segment<2> s1, s2;
  int next_end1, next_end2;

  s1.endpoint(0) = outer.m_points.back();
  s2.endpoint(0) = inner.m_points.back();
  next_end1 = next_end2 = 1;
  for(Polygon<2>::theConstIter i1 = begin1; i1 != end1; ++i1) {
    s1.endpoint(next_end1) = *i1;
    next_end1 = next_end1 ? 0 : 1;

    for(Polygon<2>::theConstIter i2 = begin2; i2 != end2; ++i2) {
      s2.endpoint(next_end2) = *i2;
      next_end2 = next_end2 ? 0 : 1;

      if(Intersect(s1, s2))
        return false;
    }
  }

  return true;
}
