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

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/segment.h>

namespace WF { namespace Math {

template<const int dim>
Segment<dim>& Segment<dim>::operator=(const Segment<dim>& a)
{
  m_p1 = a.m_p1;
  m_p2 = a.m_p2;
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
Point<dim> Segment<dim>::getCenter() const
{
  Point<dim> out;

  for(int i = 0; i < dim; ++i)
    out[i] = FloatAdd(m_p1[i], m_p2[i]) / 2;

  return out;
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

  return *this;
}

}} // namespace WF::Math

#endif  // WFMATH_SEGMENT_FUNCS_H
