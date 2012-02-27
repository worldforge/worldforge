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

// Author: Ron Steinke

#ifndef WFMATH_BALL_FUNCS_H
#define WFMATH_BALL_FUNCS_H

#include <wfmath/ball.h>

#include <wfmath/axisbox.h>
#include <wfmath/miniball.h>

#include <cassert>

namespace WFMath {

template<int dim>
inline bool Ball<dim>::isEqualTo(const Ball<dim>& b, CoordType epsilon) const
{
  return Equal(m_center, b.m_center, epsilon)
      && Equal(m_radius, b.m_radius, epsilon);
}

template<int dim>
AxisBox<dim> Ball<dim>::boundingBox() const
{
  Point<dim> p_low, p_high;

  for(int i = 0; i < dim; ++i) {
    p_low[i] = m_center[i] - m_radius;
    p_high[i] = m_center[i] + m_radius;
  }

  bool valid = m_center.isValid();

  p_low.setValid(valid);
  p_high.setValid(valid);

  return AxisBox<dim>(p_low, p_high, true);
}

template<int dim, template<class, class> class container>
Ball<dim> BoundingSphere(const container<Point<dim>, std::allocator<Point<dim> > >& c)
{
  _miniball::Miniball<dim> m;
  _miniball::Wrapped_array<dim> w;

  typename container<Point<dim>, std::allocator<Point<dim> > >::const_iterator i, end = c.end();
  bool valid = true;

  for(i = c.begin(); i != end; ++i) {
    valid = valid && i->isValid();
    for(int j = 0; j < dim; ++j)
      w[j] = (*i)[j];
    m.check_in(w);
  }

  m.build();

#ifndef NDEBUG
  double dummy;
#endif
  assert("Check that bounding sphere is good to library accuracy" &&
         m.accuracy(dummy) < numeric_constants<CoordType>::epsilon());

  w = m.center();
  Point<dim> center;

  for(int j = 0; j < dim; ++j)
    center[j] = w[j];

  center.setValid(valid);

  return Ball<dim>(center, std::sqrt(m.squared_radius()));
}

template<int dim, template<class, class> class container>
Ball<dim> BoundingSphereSloppy(const container<Point<dim>, std::allocator<Point<dim> > >& c)
{
  // This is based on the algorithm given by Jack Ritter
  // in Volume 2, Number 4 of Ray Tracing News
  // <http://www.acm.org/tog/resources/RTNews/html/rtnews7b.html>

  typename container<Point<dim>, std::allocator<Point<dim> > >::const_iterator i = c.begin(),
						end = c.end();
  if (i == end) {
    return Ball<dim>();
  }

  CoordType min[dim], max[dim];
  typename container<Point<dim>, std::allocator<Point<dim> > >::const_iterator min_p[dim], max_p[dim];
  bool valid = i->isValid();

  for(int j = 0; j < dim; ++j) {
    min[j] = max[j] = (*i)[j];
    min_p[j] = max_p[j] = i;
  }

  while(++i != end) {
    valid = valid && i->isValid();
    for(int j = 0; j < dim; ++j) {
      if(min[j] > (*i)[j]) {
        min[j] = (*i)[j];
        min_p[j] = i;
      }
      if(max[j] < (*i)[j]) {
        max[j] = (*i)[j];
        max_p[j] = i;
      }
    }
  }

  CoordType span = -1;
  int direction = -1;

  for(int j = 0; j < dim; ++j) {
    CoordType new_span = max[j] - min[j];
    if(new_span > span) {
      span = new_span;
      direction = j;
    }
  }

  assert("Have a direction of maximum size" && direction != -1);

  Point<dim> center = Midpoint(*(min_p[direction]), *(max_p[direction]));
  CoordType dist = SloppyDistance(*(min_p[direction]), center);

  for(i = c.begin(); i != end; ++i) {
    if(i == min_p[direction] || i == max_p[direction])
      continue; // We already have these

    CoordType new_dist = SloppyDistance(*i, center);

    if(new_dist > dist) {
      CoordType delta_dist = (new_dist - dist) / 2;
      // Even though new_dist may be too large, delta_dist / new_dist
      // always gives enough of a shift to include the new point.
      center += (*i - center) * delta_dist / new_dist;
      dist += delta_dist;
      assert("Shifted ball contains new point" &&
             SquaredDistance(*i, center) <= dist * dist);
    }
  }

  center.setValid(valid);

  return Ball<dim>(center, dist);
}

// These two are here, instead of defined in the class, to
// avoid include order problems

template<int dim>
inline Ball<dim> Point<dim>::boundingSphere() const
{
  return Ball<dim>(*this, 0);
}

template<int dim>
inline Ball<dim> Point<dim>::boundingSphereSloppy() const
{
  return Ball<dim>(*this, 0);
}

} // namespace WFMath

#endif  // WFMATH_BALL_FUNCS_H
