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

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>

namespace WF { namespace Math {

template<const int dim>
bool Intersection(const AxisBox<dim>& a1, const AxisBox<dim>& a2, AxisBox<dim>& out)
{
  for(int i = 0; i < dim; ++i) {
    out.m_low[i] = FloatMax(a1.m_low[i], a2.m_low[i]);
    out.m_high[i] = FloatMin(a1.m_high[i], a2.m_high[i]);
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
    out.m_low[i] = FloatMin(a1.m_low[i], a2.m_low[i]);
    out.m_high[i] = FloatMax(a1.m_high[i], a2.m_high[i]);
  }

  return out;
}

template<const int dim>
AxisBox<dim>& AxisBox<dim>::setCorners(const Point<dim>& p1, const Point<dim>& p2,
				       bool ordered)
{
  if(ordered) {
    m_low = p1;
    m_high = p2;
    return *this;
  }

  for(int i = 0; i < dim; ++i) {
    if(p1[i] > p2[i]) {
      m_low[i] = p2[i];
      m_high[i] = p1[i];
    }
    else {
      m_low[i] = p1[i];
      m_high[i] = p2[i];
    }
  }

  return *this;
}

template<const int dim>
AxisBox<dim>& AxisBox<dim>::operator=(const AxisBox<dim>& a)
{
  m_low = a.m_low;
  m_high = a.m_high;

  return *this;
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
Point<dim> AxisBox<dim>::getCenter() const
{
  Point<dim> out;

  for(int i = 0; i < dim; ++i)
    out[i] = FloatAdd(m_low[i], m_high[i]) / 2;

  return out;
}

template<const int dim>
AxisBox<dim>& AxisBox<dim>::shift(const Vector<dim>& v)
{
  m_low += v;
  m_high += v;

  return *this;
}

template<const int dim>
Ball<dim> AxisBox<dim>::boundingSphere() const
{
  return Ball<dim>(getCenter(), Distance(m_low, m_high) / 2);
}

template<const int dim>
Ball<dim> AxisBox<dim>::boundingSphereSloppy() const
{
  return Ball<dim>(getCenter(), SloppyDistance(m_low, m_high) / 2);
}

}} // namespace WF::Math

#endif  // WFMATH_AXIS_BOX_FUNCS_H
