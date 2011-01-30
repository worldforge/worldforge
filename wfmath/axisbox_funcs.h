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

// Author: Ron Steinke

// The implementation of this class is based on the geometric
// parts of the Tree and Placement classes from stage/shepherd/sylvanus

#ifndef WFMATH_AXIS_BOX_FUNCS_H
#define WFMATH_AXIS_BOX_FUNCS_H

#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>

namespace WFMath {

template<const int dim>
inline bool AxisBox<dim>::isEqualTo(const AxisBox<dim>& b, double epsilon) const
{
  return Equal(m_low, b.m_low, epsilon)
       && Equal(m_high, b.m_high, epsilon);
}

template<const int dim>
bool Intersection(const AxisBox<dim>& a1, const AxisBox<dim>& a2, AxisBox<dim>& out)
{
  for(int i = 0; i < dim; ++i) {
    out.m_low[i] = FloatMax(a1.m_low[i], a2.m_low[i]);
    out.m_high[i] = FloatMin(a1.m_high[i], a2.m_high[i]);
    if(out.m_low[i] > out.m_high[i])
      return false;
  }

  out.m_low.setValid(a1.m_low.isValid() && a2.m_low.isValid());
  out.m_high.setValid(a1.m_high.isValid() && a2.m_high.isValid());

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

  out.m_low.setValid(a1.m_low.isValid() && a2.m_low.isValid());
  out.m_high.setValid(a1.m_high.isValid() && a2.m_high.isValid());

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

  m_low.setValid();
  m_high.setValid();

  return *this;
}

template<const int dim>
Point<dim> AxisBox<dim>::getCorner(int i) const
{
  if(i < 1)
    return m_low;
  if(i >= (1 << dim) - 1)
    return m_high;

  Point<dim> out;

  for(int j = 0; j < dim; ++j)
    out[j] = (i & (1 << j)) ? m_high[j] : m_low[j];

  out.setValid(m_low.isValid() && m_high.isValid());

  return out;
}

template<const int dim>
inline Ball<dim> AxisBox<dim>::boundingSphere() const
{
  return Ball<dim>(getCenter(), Distance(m_low, m_high) / 2);
}

template<const int dim>
inline Ball<dim> AxisBox<dim>::boundingSphereSloppy() const
{
  return Ball<dim>(getCenter(), SloppyDistance(m_low, m_high) / 2);
}


#ifndef WFMATH_NO_TEMPLATES_AS_TEMPLATE_PARAMETERS
template<const int dim, template<class, class> class container>
AxisBox<dim> BoundingBox(const container<AxisBox<dim>, std::allocator<AxisBox<dim> > >& c)
{
  // FIXME become friend

  typename container<AxisBox<dim>, std::allocator<AxisBox<dim> > >::const_iterator i = c.begin(), end = c.end();

  if(i == end) {
    return AxisBox<dim>();
  }

  Point<dim> low = i->lowCorner(), high = i->highCorner();
  bool low_valid = low.isValid(), high_valid = high.isValid();

  while(++i != end) {
    const Point<dim> &new_low = i->lowCorner(), &new_high = i->highCorner();
    low_valid = low_valid && new_low.isValid();
    high_valid = high_valid && new_high.isValid();
    for(int j = 0; j < dim; ++j) {
      low[j] = FloatMin(low[j], new_low[j]);
      high[j] = FloatMax(high[j], new_high[j]);
    }
  }

  low.setValid(low_valid);
  high.setValid(high_valid);

  return AxisBox<dim>(low, high, true);
}

template<const int dim, template<class, class> class container>
AxisBox<dim> BoundingBox(const container<Point<dim>, std::allocator<Point<dim> > >& c)
{
  typename container<Point<dim>, std::allocator<Point<dim> > >::const_iterator i = c.begin(), end = c.end();

  if(i == end) {
    return AxisBox<dim>();
  }

  Point<dim> low = *i, high = *i;
  bool valid = i->isValid();

  while(++i != end) {
    valid = valid && i->isValid();
    for(int j = 0; j < dim; ++j) {
      low[j] = FloatMin(low[j], (*i)[j]);
      high[j] = FloatMax(high[j], (*i)[j]);
    }
  }

  low.setValid(valid);
  high.setValid(valid);

  return AxisBox<dim>(low, high, true);
}
#endif

// This is here, instead of defined in the class, to
// avoid include order problems

template<const int dim>
inline AxisBox<dim> Point<dim>::boundingBox() const
{
  return AxisBox<dim>(*this, *this, true);
}

template<const int dim>
Point<dim> Point<dim>::toParentCoords(const AxisBox<dim>& coords) const
{
  return coords.lowCorner() + (*this - Point().setToOrigin());
}

template<const int dim>
Point<dim> Point<dim>::toLocalCoords(const AxisBox<dim>& coords) const
{
  return Point().setToOrigin() + (*this - coords.lowCorner());
}

} // namespace WFMath

#endif  // WFMATH_AXIS_BOX_FUNCS_H
