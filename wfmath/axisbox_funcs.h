// -*-C++-*-
// axisbox_funcs.h (Axis-aligned box implementation)
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

// The implementation of this class is based on the geometric
// parts of the Tree and Placement classes from stage/shepherd/sylvanus

#ifndef WFMATH_AXIS_BOX_FUNCS_H
#define WFMATH_AXIS_BOX_FUNCS_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/shape.h>
#include <wfmath/axisbox.h>
#include <wfmath/vector_funcs.h>
#include <wfmath/point_funcs.h>

#include <algorithm>
#include <string>

namespace WF { namespace Math {

template<const int dim> class AxisBox;

template<const int dim>
bool Intersect(const AxisBox<dim>& a1, const AxisBox<dim>& a2, AxisBox<dim>& out)
{
  for(int i = 0; i < dim; ++i) {
    out.m_low[i] = std::max(a1.m_low[i], a2.m_low[i]);
    out.m_high[i] = std::min(a1.m_high[i], a2.m_high[i]);
    if(out.m_low[i] > out.m_high[i])
      return false;
  }

  return true;
}

template<const int dim>
AxisBox<dim> Union(const AxisBox<dim>& a1, const AxisBox<dim>& a2)
{
  AxisBox<dim> out;

  for(int i = 0; i < dim; ++i) {
    out.m_low[i] = std::min(a1.m_low[i], a2.m_low[i]);
    out.m_high[i] = std::max(a1.m_high[i], a2.m_high[i]);
  }

  return out;
}

template<const int dim>
AxisBox<dim>& AxisBox<dim>::setCorners(const Point<dim>& p1, const Point<dim>& p2)
{
  for(int i = 0; i < dim; ++i) {
    Shape<dim>::m_center[i] = (p1[i] + p2[i]) / 2;
    if(p1[i] > p2[i]) {
      m_low[i] = p2[i];
      m_high[i] = p1[1];
    }
    else {
      m_low[i] = p1[i];
      m_high[i] = p2[1];
    }
  }

  return *this;
}

template<const int dim>
std::string AxisBox<dim>::toString() const
{
  return "AxisBox: m_low = " + m_low.toString() + ", m_high = " + m_high.toString();
}

template<const int dim>
bool AxisBox<dim>::fromString(const std::string& s)
{
  int low_pos, high_pos;

  low_pos = s.find('(', s.find("m_low"));
  high_pos = s.find('(', s.find("m_high"));

  return m_low.fromString(s.substr(low_pos))
      && m_high.fromString(s.substr(high_pos));
}

template<const int dim>
AxisBox<dim>& AxisBox<dim>::operator=(const AxisBox<dim>& a)
{
  m_low = a.m_low;
  m_high = a.m_high;
  Shape<dim>::m_center = a.Shape<dim>::m_center;
}

// WARNING! This operator is for sorting only. It does not
// reflect any property of the box.
template<const int dim>
bool AxisBox<dim>::operator< (const AxisBox<dim>& a) const
{
  if(m_low < a.m_low)
    return true;
  if(a.m_low < m_low)
    return false;
  return m_high < a.m_high;
}

template<const int dim>
Point<dim> AxisBox<dim>::getCorner(int i) const
{
  Point<dim> out;

  for(int j = 0; j < dim; ++j)
    out[j] = (i & (1 << j)) ? m_high[j] : m_low[j];

  return out;
}

template<const int dim>
AxisBox<dim> AxisBox<dim>::quadrant(int i) const
{
  AxisBox<dim> out;

  for(int j = 0; j < dim; ++j) {
    if(i & (1 << j)) {
      out.m_low[i] = Shape<dim>::m_center[i];
      out.m_high[i] = m_high[i];
    }
    else {
      out.m_low[i] = m_low[i];
      out.m_high[i] = Shape<dim>::m_center[i];
    }
  }

  return out;
}

template<const int dim>
AxisBox<dim>& AxisBox<dim>::shift(const Vector<dim>& v)
{
  m_low += v;
  m_high += v;
  Shape<dim>::m_center += v;
}

template<const int dim>
bool AxisBox<dim>::contains(const Point<dim>& p) const
{
  for(int i = 0; i < dim; ++i)
    if(p[i] < m_low[i] || p[i] > m_high[i])
      return false;

  return true;
}

template<const int dim>
bool AxisBox<dim>::containsProper(const Point<dim>& p) const
{
  for(int i = 0; i < dim; ++i)
    if(p[i] <= m_low[i] || p[i] >= m_high[i])
      return false;

  return true;
}

template<const int dim>
bool AxisBox<dim>::contains(const AxisBox<dim>& b) const
{
  for(int i = 0; i < dim; ++i)
    if(b.m_low[i] < m_low[i] || b.m_high[i] > m_high[i])
      return false;

  return true;
}

template<const int dim>
bool AxisBox<dim>::containsProper(const AxisBox<dim>& b) const
{
  for(int i = 0; i < dim; ++i)
    if(b.m_low[i] <= m_low[i] || b.m_high[i] >= m_high[i])
      return false;

  return true;
}

template<const int dim>
bool AxisBox<dim>::isContainedBy(const AxisBox<dim>& b) const
{
  for(int i = 0; i < dim; ++i)
    if(b.m_low[i] > m_low[i] || b.m_high[i] < m_high[i])
      return false;

  return true;
}

template<const int dim>
bool AxisBox<dim>::isContainedByProper(const AxisBox<dim>& b) const
{
  for(int i = 0; i < dim; ++i)
    if(b.m_low[i] >= m_low[i] || b.m_high[i] <= m_high[i])
      return false;

  return true;
}

template<const int dim>
bool AxisBox<dim>::intersects(const AxisBox<dim>& b) const
{
  for(int i = 0; i < dim; ++i)
    if(b.m_low[i] > m_high[i] || b.m_high[i] < m_low[i])
      return false;

  return true;
}

template<const int dim>
bool AxisBox<dim>::intersectsProper(const AxisBox<dim>& b) const
{
  for(int i = 0; i < dim; ++i)
    if(b.m_low[i] >= m_high[i] || b.m_high[i] <= m_low[i])
      return false;

  return true;
}

template<const int dim>
int AxisBox<dim>::isSubContainedBy(const AxisBox<dim>& b) const
{
  int sector = 0;

  for(int i = 0; i < dim; ++i) {
    if(m_low[i] >= b.Shape<dim>::m_center[i])
      sector |= 1 << i;
    else if(m_high[i] > b.Shape<dim>::m_center[i])
      return -1;
  }

  return sector;
}

template<const int dim>
int AxisBox<dim>::isSubContainedByProper(const AxisBox<dim>& b) const
{
  int sector = 0;

  for(int i = 0; i < dim; ++i) {
    if(m_low[i] > b.Shape<dim>::m_center[i])
      sector |= 1 << i;
    else if(m_high[i] >= b.Shape<dim>::m_center[i])
      return -1;
  }

  return sector;
}

}} // namespace WF::Math

#endif  // WFMATH_AXIS_BOX_FUNCS_H
