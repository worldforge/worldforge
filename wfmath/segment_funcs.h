// -*-C++-*-
// segment_funcs.h (line segment implementation)
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

#ifndef WFMATH_SEGMENT_FUNCS_H
#define WFMATH_SEGMENT_FUNCS_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/shape.h>
#include <wfmath/axisbox.h>
#include <wfmath/segment.h>

#include <string>

namespace WF { namespace Math {

template<const int dim>
Segment<dim>::Segment(const Point<dim>& p1, const Point<dim>& p2)
{
  for(int i = 0; i < dim; ++i) {
    m_p1[i] = p1[i];
    m_p2[i] = p2[i];
    Shape<dim>::m_center[i] = FloatAdd(p1[i], p2[i]) / 2;
  }
}

template<const int dim>
std::string Segment<dim>::toString() const
{
  return "Segment: m_p1 = " + m_p1.toString() + ", m_p2 = " + m_p2.toString();
}

template<const int dim>
bool Segment<dim>::fromString(const std::string& s)
{
  int p1_pos, p2_pos;

  p1_pos = s.find('(', s.find("m_p1"));
  p2_pos = s.find('(', s.find("m_p2"));

  if(p1_pos == std::npos || p2_pos == std::npos)
    return false;

  if(!m_p1.fromString(s.substr(p1_pos))
     || !m_p2.fromString(s.substr(p2_pos))) {
     m_p1.origin();
     m_p2.origin();
     Shape<dim>::m_center.origin();
     return false;
  }

  for(int i = 0; i < dim; ++i)
    Shape<dim>::m_center[i] = FloatAdd(m_p1[i], m_p2[i]) / 2;

  return true;
}

template<const int dim>
Segment<dim>& Segment<dim>::operator=(const Segment<dim>& a)
{
  m_p1 = a.m_p1;
  m_p2 = a.m_p2;
  Shape<dim>::m_center = a.Shape<dim>::m_center;
}

// WARNING! This operator is for sorting only. It does not
// reflect any property of the box.
template<const int dim>
bool Segment<dim>::operator< (const Segment<dim>& a) const
{
  if(m_p1 < a.m_p1)
    return true;
  if(a.m_p1 < m_p1)
    return false;
  return m_p2 < a.m_p2;
}

template<const int dim>
Point<dim> Segment<dim>::getCorner(int i) const
{
  switch(i) {
    case 0:
      return m_p1;
    case 1:
      return m_p2;
    default:
      assert(false);
      return Point<dim>();
  }
}

template<const int dim>
Segment<dim>& Segment<dim>::setCorner(const Point<dim>& p, int i)
{
  switch(i) {
    case 0:
      m_p1 = p;
    case 1:
      m_p2 = p;
    default:
      assert(false);
  }

  for(int j = 0; j < dim; ++j)
    Shape<dim>::m_center[i] = FloatAdd(m_p1[i], m_p2[i]) / 2;

  return *this;
}

template<const int dim>
bool Segment<dim>::contains(const Point<dim>& p) const
{
  // This is only true if p lies on the line between m_p1 and m_p2

  Vector<dim> v1 = m_p1 - p, v2 = m_p2 - p;

  double proj = Dot(v1, v2);

  if(proj > 0) // p is on the same side of both ends, not between them
    return false;

  // Check for colinearity
  return IsFloatEqual(proj * proj, v1.sqrMag() * v2.sqrMag());
}

template<const int dim>
bool Segment<dim>::containsProper(const Point<dim>& p) const
{
  // This is only true if p lies on the line between m_p1 and m_p2

  Vector<dim> v1 = m_p1 - p, v2 = m_p2 - p;

  double proj = Dot(v1, v2);

  if(proj >= 0) // p is on the same side of both ends, not between them
    return false;

  // Check for colinearity
  return IsFloatEqual(proj * proj, v1.sqrMag() * v2.sqrMag());
}

template<const int dim>
bool Segment<dim>::contains(const AxisBox<dim>& b) const
{
  // This is only possible for a one-point box, in which case
  // we use contains(Point<>).

  Point<dim> p = b.getCorner(0);

  for(int i = 0; i < (1 << dim); ++i)
    if(!IsFloatEqual(p[i], b.upperBound(i)))
      return false;

  return contains(p);
}

template<const int dim>
bool Segment<dim>::containsProper(const AxisBox<dim>& b) const
{
  // This is only possible for a one-point box, in which case
  // we use contains(Point<>).

  Point<dim> p = b.getCorner(0);

  for(int i = 0; i < (1 << dim); ++i)
    if(!IsFloatEqual(p[i], b.upperBound(i)))
      return false;

  return containsProper(p);
}

template<const int dim>
bool Segment<dim>::intersects(const AxisBox<dim>& b) const
{
  // Use parametric coordinates on the line, where 0 is the location
  // of m_p1 and 1 is the location of m_p2

  // Find the parametric coordinates of the portion of the line
  // which lies betweens b.lowerBound(i) and b.upperBound(i) for
  // each i. Find the intersection of those segments and the
  // segment (0, 1), and check if it's nonzero.

  double min = 0, max = 1, low, high;

  for(int i = 0; i < dim; ++i) {
    double dist = FloatSubtract(m_p2[i], m_p1[i]);
    if(dist == 0) {
      if(m_p1[i] < b.lowerBound(i) || m_p1[i] > b.upperBound(i))
        return false;
    }
    else {
      low = (b.lowerBound(i) - m_p1[i]) / dist;
      high = (b.upperBound(i) - m_p1[i]) / dist;
      if(low > min)
        min = low;
      if(high < max)
        max = high;
    }
  }

  return min <= max;
}

template<const int dim>
bool Segment<dim>::intersectsProper(const AxisBox<dim>& b) const
{
  // Use parametric coordinates on the line, where 0 is the location
  // of m_p1 and 1 is the location of m_p2

  // Find the parametric coordinates of the portion of the line
  // which lies betweens b.lowerBound(i) and b.upperBound(i) for
  // each i. Find the intersection of those segments and the
  // segment (0, 1), and check if it's nonzero.

  double min = 0, max = 1, low, high;

  for(int i = 0; i < dim; ++i) {
    double dist = FloatSubtract(m_p2[i], m_p1[i]);
    if(dist == 0) {
      if(m_p1[i] <= b.lowerBound(i) || m_p1[i] >= b.upperBound(i))
        return false;
    }
    else {
      low = (b.lowerBound(i) - m_p1[i]) / dist;
      high = (b.upperBound(i) - m_p1[i]) / dist;
      if(low > min)
        min = low;
      if(high < max)
        max = high;
    }
  }

  return min < max;
}

template<const int dim>
int Segment<dim>::isSubContainedBy(const AxisBox<dim>& b) const
{
  int sector = 0;
  Point<dim> b_center = b.getCenter();

  for(int i = 0; i < dim; ++i) {
    if(m_p1[i] >= b_center[i]) {
      sector |= 1 << i;
      if(m_p2[i] < b_center[i])
        return -1;
    }
    else if(m_p2[i] > b_center[i])
      return -1;
  }

  return sector;
}

template<const int dim>
int Segment<dim>::isSubContainedByProper(const AxisBox<dim>& b) const
{
  int sector = 0;
  Point<dim> b_center = b.getCenter();

  for(int i = 0; i < dim; ++i) {
    if(m_p1[i] > b_center[i]) {
      sector |= 1 << i;
      if(m_p2[i] <= b_center[i])
        return -1;
    }
    else if(m_p2[i] >= b_center[i])
      return -1;
  }

  return sector;
}

}} // namespace WF::Math

#endif  // WFMATH_SEGMENT_FUNCS_H
