// -*-C++-*-
// rotbox_funcs.h (line rotbox implementation)
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

#ifndef WFMATH_ROT_BOX_FUNCS_H
#define WFMATH_ROT_BOX_FUNCS_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/axisbox.h>
#include <wfmath/rotbox.h>

namespace WF { namespace Math {

template<const int dim>
RotBox<dim>& RotBox<dim>::operator=(const RotBox<dim>& a)
{
  m_corner0 = a.m_corner0;
  m_size = a.m_size;
  m_orient = a.m_orient;
}

template<const int dim>
bool RotBox<dim>::isEqualTo(const RotBox<dim>& s, double tolerance) const
{
  return m_corner0.isEqualTo(s.m_corner0, tolerance)
      && m_size.isEqualTo(s.m_size, tolerance)
      && m_orient.isEqualTo(s.m_orient, tolerance);
}

// WARNING! This operator is for sorting only. It does not
// reflect any property of the box.
template<const int dim>
bool RotBox<dim>::operator< (const RotBox<dim>& a) const
{
  if(m_corner0 < a.m_corner0)
    return true;
  if(a.m_corner0 < m_corner0)
    return false;
  if(m_size < a.m_size)
    return true;
  if(a.m_size < m_size)
    return false;
  return m_orient < a.m_orient;
}

template<const int dim>
Point<dim> RotBox<dim>::getCorner(int i) const
{
  Vector<dim> dist;

  for(int j = 0; j < dim; ++j)
    dist[j] = (i & (1 << j)) ? m_size[j] : 0;

  return m_corner0 + Prod(m_orient, dist);
}

template<const int dim>
AxisBox<dim> RotBox<dim>::boundingBox() const
{
  Point<dim> min, max;

//  min.zero();
//  max.zero();
//
//  for(int i = 0; i < dim; ++i) {
//    Vector<dim> edge;
//    edge.zero();
//    edge[i] = m_size[i];
//    edge = Prod(m_orient, edge);
//    // Edge now represents the i'th edge
//    // pointing away from m_corner0
//    for(int j = 0; j < dim; ++j) {
//      // All coords of min <= 0, all coords of max >= 0, don't need FloatAdd()
//      if(edge[j] < 0)
//        min[j] += edge[j];
//      else
//        max[j] += edge[j];
//    }
//  }
//
//  for(int i = 0; i < dim; ++i) {
//    min[i] = FloatAdd(min[i], m_corner0[i]);
//    max[i] = FloatAdd(max[i], m_corner0[i]);
//  }

// The following is equivalent to the above. The above is easier to understand,
// so leave it in as a comment.

  for(int i = 0; i < dim; ++i) {
    double minval = 0, maxval = 0;
    for(int j = 0; j < dim; ++j) {
      CoordType value = m_orient.elem(i, j) * m_size[j];
      // All coords of min <= 0, all coords of max >= 0, don't need FloatAdd()
      if(value < 0)
        minval += value;
      else
        maxval += value;
    }
    min[i] = FloatAdd(minval, m_corner0[i]);
    max[i] = FloatAdd(maxval, m_corner0[i]);
  }

  return AxisBox<dim>(min, max, true);
}

}} // namespace WF::Math

#endif  // WFMATH_ROT_BOX_FUNCS_H
