// -*-C++-*-
// ball_funcs.h (n-dimensional ball implementation)
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

#ifndef WFMATH_BALL_FUNCS_H
#define WFMATH_BALL_FUNCS_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/shape.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>

#include <string>

namespace WF { namespace Math {

template<const int dim>
std::string Ball<dim>::toString() const
{
  return "Ball: m_center = " + Shape<dim>::m_center.toString()
	  + ", m_radius = " + _StringFromFloat(m_radius);
}

template<const int dim>
bool Ball<dim>::fromString(const std::string& s)
{
  int cen_pos, rad_pos;

  cen_pos = s.find('(', s.find("m_center"));
  rad_pos = s.find('=', s.find("m_radius")) + 2;

  if(cen_pos == std::npos || rad_pos == std::npos)
    return false;

  m_radius = _StringToFloat(s.substr(rad_pos));

  return Shape<dim>::m_center.fromString(s.substr(cen_pos));
}

template<const int dim>
Ball<dim>& Ball<dim>::operator=(const Ball<dim>& a)
{
  m_radius = a.m_radius;
  Shape<dim>::m_center = a.Shape<dim>::m_center;
}

// WARNING! This operator is for sorting only. It does not
// reflect any property of the box.
template<const int dim>
bool Ball<dim>::operator< (const Ball<dim>& a) const
{
  if(m_radius < a.m_radius)
    return true;
  if(a.m_radius < m_radius)
    return false;
  return Shape<dim>::m_center < a.Shape<dim>::m_center;
}

template<const int dim>
AxisBox<dim> Ball<dim>::boundingBox() const
{
  Point<dim> p_low, p_high;

  for(int i = 0; i < dim; ++i) {
    p_low[i] = FloatSubtract(Shape<dim>::m_center[i], m_radius);
    p_high[i] = FloatAdd(Shape<dim>::m_center[i], m_radius);
  }

  return AxisBox<dim>(p_low, p_high);
}

template<const int dim>
AxisBox<dim> Ball<dim>::enclosedBox() const
{
  static const double sqrt_dim = sqrt(dim);
  const double dist = m_radius / sqrt_dim;

  Point<dim> p_low, p_high;

  for(int i = 0; i < dim; ++i) {
    p_low[i] = FloatSubtract(Shape<dim>::m_center[i], dist);
    p_high[i] = FloatAdd(Shape<dim>::m_center[i], dist);
  }

  return AxisBox<dim>(p_low, p_high);
}

template<const int dim>
bool Ball<dim>::contains(const Point<dim>& p) const
{
  return Shape<dim>::m_center.getSquaredDistanceTo(p) <= m_radius * m_radius;
}

template<const int dim>
bool Ball<dim>::containsProper(const Point<dim>& p) const
{
  return Shape<dim>::m_center.getSquaredDistanceTo(p) < m_radius * m_radius;
}

template<const int dim>
bool Ball<dim>::contains(const AxisBox<dim>& b) const
{
  for(int i = 0; i < (1 << dim); ++i)
    if(!contains(b.getCorner(i)))
      return false;

  return true;
}

template<const int dim>
bool Ball<dim>::containsProper(const AxisBox<dim>& b) const
{
  for(int i = 0; i < (1 << dim); ++i)
    if(!containsProper(b.getCorner(i)))
      return false;

  return true;
}

template<const int dim>
bool Ball<dim>::isContainedBy(const AxisBox<dim>& b) const
{
  // Don't use FloatAdd(), only need values for comparison

  for(int i = 0; i < (1 << dim); ++i)
    if(Shape<dim>::m_center[i] - m_radius < b.lowerBound(i)
       || Shape<dim>::m_center[i] + m_radius > b.upperBound(i))
      return false;

  return true;
}

template<const int dim>
bool Ball<dim>::isContainedByProper(const AxisBox<dim>& b) const
{
  // Don't use FloatAdd(), only need values for comparison

  for(int i = 0; i < (1 << dim); ++i)
    if(Shape<dim>::m_center[i] - m_radius <= b.lowerBound(i)
       || Shape<dim>::m_center[i] + m_radius >= b.upperBound(i))
      return false;

  return true;
}

template<const int dim>
bool Ball<dim>::intersects(const AxisBox<dim>& b) const
{
  Vector<dim> dist;

  // Don't use FloatAdd(), only need values for comparison

  for(int i = 0; i < dim; ++i) {
    if(Shape<dim>::m_center[i] < b.lowerBound(i))
      dist[i] = Shape<dim>::m_center[i] - b.lowerBound(i);
    else if(Shape<dim>::m_center[i] > b.upperBound(i))
      dist[i] = Shape<dim>::m_center[i] - b.upperBound(i);
    else
      dist[i] = 0;
  }

  return dist.sqrMag() <= m_radius * m_radius;
}

template<const int dim>
bool Ball<dim>::intersectsProper(const AxisBox<dim>& b) const
{
  Vector<dim> dist;

  // Don't use FloatAdd(), only need values for comparison

  for(int i = 0; i < dim; ++i) {
    if(Shape<dim>::m_center[i] < b.lowerBound(i))
      dist[i] = Shape<dim>::m_center[i] - b.lowerBound(i);
    else if(Shape<dim>::m_center[i] > b.upperBound(i))
      dist[i] = Shape<dim>::m_center[i] - b.upperBound(i);
    else
      dist[i] = 0;
  }

  return dist.sqrMag() < m_radius * m_radius;
}

template<const int dim>
int Ball<dim>::isSubContainedBy(const AxisBox<dim>& b) const
{
  int sector = 0;
  Point<dim> b_center = b.getCenter();

  // Don't use FloatAdd(), only need values for comparison

  for(int i = 0; i < dim; ++i) {
    if(Shape<dim>::m_center[i] - m_radius >= b_center[i])
      sector |= 1 << i;
    else if(Shape<dim>::m_center[i] + m_radius > b_center[i])
      return -1;
  }

  return sector;
}

template<const int dim>
int Ball<dim>::isSubContainedByProper(const AxisBox<dim>& b) const
{
  int sector = 0;
  Point<dim> b_center = b.getCenter();

  // Don't use FloatAdd(), only need values for comparison

  for(int i = 0; i < dim; ++i) {
    if(Shape<dim>::m_center[i] - m_radius > b_center[i])
      sector |= 1 << i;
    else if(Shape<dim>::m_center[i] + m_radius >= b_center[i])
      return -1;
  }

  return sector;
}

}} // namespace WF::Math

#endif  // WFMATH_BALL_FUNCS_H
