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
#include <wfmath/shape.h>
#include <wfmath/axisbox.h>
#include <wfmath/rotbox.h>

#include <string>

namespace WF { namespace Math {

template<const int dim>
RotBox<dim>::RotBox(const Point<dim>& p, const Vector<dim>& size,
		    const RotMatrix<dim>& orientation)
	: RotShape<dim>(p + Prod(orientation, size / 2)),
	  m_corner0(p), m_size(size), m_orient(orientation) {}

template<const int dim>
std::string RotBox<dim>::toString() const
{
  return "RotBox: m_corner0 = " + m_corner0.toString()
	 + ", m_size = " + m_size.toString()
	 + ", m_orient = " + m_orient.toString();
}

template<const int dim>
bool RotBox<dim>::fromString(const std::string& s)
{
  int corner_pos, size_pos, orient_pos;

  corner_pos = s.find('(', s.find("m_corner0"));
  size_pos = s.find('(', s.find("m_size"));
  orient_pos = s.find('(', s.find("m_orient"));

  if(corner_pos == std::npos || size_pos == std::npos || orient_pos == std::npos)
    return false;

  if(!m_corner0.fromString(s.substr(corner_pos))
      || !m_size.fromString(s.substr(size_pos))
      || !m_orient.fromString(s.substr(orient_pos))) {
    m_corner0.origin();
    m_size.zero();
    m_orient.identity();
    Shape<dim>::m_center.origin();
    return false;
  }

  Shape<dim>::m_center = m_corner0 + Prod(m_orient, m_size / 2);

  return true;
}

template<const int dim>
RotBox<dim>& RotBox<dim>::operator=(const RotBox<dim>& a)
{
  m_corner0 = a.m_corner0;
  m_size = a.m_size;
  m_orient = a.m_orient;
  Shape<dim>::m_center = a.Shape<dim>::m_center;
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
Shape<dim>& RotBox<dim>::shift(const Vector<dim>& v)
{
  m_corner0 += v;
  Shape<dim>::m_center += v;

  return *this;
}

template<const int dim>
RotShape<dim>& RotBox<dim>::rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p)
{
  m_orient = Prod(m, m_orient);

  m_corner0.rotate(m, p);
  Shape<dim>::m_center.rotate(m, p);

  return *this;
}

template<const int dim>
AxisBox<dim> RotBox<dim>::boundingBox() const
{
  Point<dim> min = m_corner0, max = m_corner0;

  for(int i = 0; i < dim; ++i) {
//    Vector<dim> edge;
//    edge.zero();
//    edge[i] = m_size[i];
//    edge = Prod(m_orient, edge);
    // Edge now represents the i'th edge
    // pointing away from m_corner0
    for(int j = 0; j < dim; ++j) {
      // You can get the same value as edge[j] in
      // the above result by the following:
      CoordType value = m_orient.elem(j, i) * m_size[i];
      // All coords of min <= 0, all coords of max >= 0, don't need _FloatAdd()
      if(value < 0)
        min[j] += value;
      else
        max[j] += value;
    }
  }

  return AxisBox<dim>(min, max);
}

template<const int dim>
AxisBox<dim> RotBox<dim>::enclosedBox() const
{
  // FIXME

  return AxisBox<dim>();
}

template<const int dim>
bool RotBox<dim>::contains(const Point<dim>& p) const
{
  // Rotate the point into the internal coordinate system of the box

  Vector<dim> shift = InvProd(m_orient, p - m_corner0);

  for(int i = 0; i < dim; ++i) {
    if(m_size[i] < 0) {
      if(shift[i] < m_size[i] || shift[i] > 0)
        return false;
    }
    else {
      if(shift[i] > m_size[i] || shift[i] < 0)
        return false;
    }
  }

  return true;
}

template<const int dim>
bool RotBox<dim>::containsProper(const Point<dim>& p) const
{
  // Rotate the point into the internal coordinate system of the box

  Vector<dim> shift = InvProd(m_orient, p - m_corner0);

  for(int i = 0; i < dim; ++i) {
    if(m_size[i] < 0) {
      if(shift[i] <= m_size[i] || shift[i] >= 0)
        return false;
    }
    else {
      if(shift[i] >= m_size[i] || shift[i] <= 0)
        return false;
    }
  }

  return true;
}

template<const int dim>
bool RotBox<dim>::contains(const AxisBox<dim>& b) const
{
  // FIXME

  return false;
}

template<const int dim>
bool RotBox<dim>::containsProper(const AxisBox<dim>& b) const
{
  // FIXME

  return false;
}

template<const int dim>
bool RotBox<dim>::isContainedBy(const AxisBox<dim>& b) const
{
  return boundingBox().isContainedBy(b);
}

template<const int dim>
bool RotBox<dim>::isContainedByProper(const AxisBox<dim>& b) const
{
  return boundingBox().isContainedByProper(b);
}

template<const int dim>
bool RotBox<dim>::intersects(const AxisBox<dim>& b) const
{
  // FIXME

  return false;
}

template<const int dim>
bool RotBox<dim>::intersectsProper(const AxisBox<dim>& b) const
{
  // FIXME

  return false;
}

template<const int dim>
int RotBox<dim>::isSubContainedBy(const AxisBox<dim>& b) const
{
  return boundingBox().isSubContainedBy(b);
}

template<const int dim>
int RotBox<dim>::isSubContainedByProper(const AxisBox<dim>& b) const
{
  return boundingBox().isSubContainedByProper(b);
}

}} // namespace WF::Math

#endif  // WFMATH_ROT_BOX_FUNCS_H
