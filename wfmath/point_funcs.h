// point_funcs.h (point class copied from libCoal, subsequently modified)
//
//  The WorldForge Project
//  Copyright (C) 2000, 2001, 2002  The WorldForge Project
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


#ifndef WFMATH_POINT_FUNCS_H
#define WFMATH_POINT_FUNCS_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>

namespace WFMath {

template<const int dim>
Point<dim>::Point(const Point<dim>& p)
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = p.m_elem[i];
}

template<const int dim>
Point<dim>& Point<dim>::origin()
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = 0;

  return *this;
}

template<const int dim>
bool Point<dim>::isEqualTo(const Point<dim> &p, double epsilon) const
{
  CoordType delta = _ScaleEpsilon(m_elem, p.m_elem, dim, epsilon);

  for(int i = 0; i < dim; ++i)
    if(fabs(m_elem[i] - p.m_elem[i]) > delta)
      return false;

  return true;
}

template<const int dim>
Vector<dim> operator-(const Point<dim>& c1, const Point<dim>& c2)
{
  Vector<dim> out;

  for(int i = 0; i < dim; ++i)
    out.m_elem[i] = c1.m_elem[i] - c2.m_elem[i];

  return out;
}

template<const int dim>
Point<dim> operator+(const Point<dim>& c, const Vector<dim>& v)
{
  Point<dim> out;

  for(int i = 0; i < dim; ++i)
    out.m_elem[i] = c.m_elem[i] + v.m_elem[i];

  return out;
}

template<const int dim>
Point<dim> operator-(const Point<dim>& c, const Vector<dim>& v)
{
  Point<dim> out;

  for(int i = 0; i < dim; ++i)
    out.m_elem[i] = c.m_elem[i] - v.m_elem[i];

  return out;
}

template<const int dim>
Point<dim> operator+(const Vector<dim>& v, const Point<dim>& c)
{
  Point<dim> out;

  for(int i = 0; i < dim; ++i)
    out.m_elem[i] = c.m_elem[i] + v.m_elem[i];

  return out;
}

template<const int dim>
Point<dim>& Point<dim>::operator=(const Point<dim>& rhs)
{
    // Compare pointer addresses
    // FIXME does this work in general?
    if (this == &rhs)
	return *this;

    for(int i = 0; i < dim; ++i)
      m_elem[i] = rhs.m_elem[i];

    return *this;
}

template<const int dim>
Point<dim>& operator+=(Point<dim>& p, const Vector<dim> &rhs)
{
    for(int i = 0; i < dim; ++i)
      p.m_elem[i] += rhs.m_elem[i];

    return p;
}

template<const int dim>
Point<dim>& operator-=(Point<dim>& p, const Vector<dim> &rhs)
{
    for(int i = 0; i < dim; ++i)
      p.m_elem[i] -= rhs.m_elem[i];

    return p;
}

// Pointinate comparison operator, most likely only used for sorting
// in an STL container.  The implementation is fairly arbitrary, but
// consistent, i.e., it satisfies the following constraint:
//
//   !(a < b) && !(b < a) => a == b
//
// The implementation doesn't express any physical or geometrical
// relationship (e.g., vector magnitude or distance from origin).
template<const int dim>
bool Point<dim>::operator< (const Point<dim>& rhs) const
{
  if(operator==(rhs))
    return false;

  for(int i = 0; i < dim; ++i)
    if(m_elem[i] != rhs.m_elem[i])
      return m_elem[i] < rhs.m_elem[i];

  assert(false);
}

// These three are here, instead of defined in the class, to
// avoid include order problems

template<const int dim>
inline AxisBox<dim> Point<dim>::boundingBox() const
{
  return AxisBox<dim>(*this, *this, true);
}

template<const int dim>
inline Ball<dim> Point<dim>::boundingSphere() const
{
  return Ball<dim>(*this, 0);
}

template<const int dim>
inline Ball<dim> Point<dim>::boundingSphereSloppy() const
{
  return Ball<dim>(*this, 0);
}

template<const int dim>
CoordType SquaredDistance(const Point<dim>& p1, const Point<dim>& p2)
{
  CoordType ans = 0;

  for(int i = 0; i < dim; ++i) {
    CoordType diff = p1.m_elem[i] - p2.m_elem[i];
    ans += diff * diff;
  }

  return (fabs(ans) >= _ScaleEpsilon(p1.m_elem, p2.m_elem, dim)) ? ans : 0;
}

template<const int dim, template<class> class container,
			template<class> class container2>
Point<dim> Barycenter(const container<Point<dim> >& c,
		      const container2<CoordType>& weights)
{
  // FIXME become friend

  typename container<Point<dim> >::const_iterator c_i = c.begin(), c_end = c.end();
  typename container2<CoordType>::const_iterator w_i = weights.begin(),
						 w_end = weights.end();

  assert(c_i != c_end);
  assert(w_i != w_end);

  CoordType tot_weight = *w_i, max_weight = fabs(*w_i);
  Point<dim> out;
  for(int j = 0; j < dim; ++j)
    out[j] = (*c_i)[j] * *w_i;

  while(++c_i != c_end && ++w_i != w_end) {
    tot_weight += *w_i;
    CoordType val = fabs(*w_i);
    if(val > max_weight)
      max_weight = val;
    for(int j = 0; j < dim; ++j)
      out[j] += (*c_i)[j] * *w_i;
  }

  // Make sure the weights don't add up to zero
  assert(max_weight > 0 && fabs(tot_weight) > max_weight * WFMATH_EPSILON);

  for(int j = 0; j < dim; ++j)
    out[j] /= tot_weight;

  return out;
}

template<const int dim, template<class> class container>
Point<dim> Barycenter(const container<Point<dim> >& c)
{
  // FIXME become friend

  typename container<Point<dim> >::const_iterator i = c.begin(), end = c.end();

  assert(i != end);

  Point<dim> out = *i;
  int num_points = 1;

  while(++i != end) {
    ++num_points;
    for(int j = 0; j < dim; ++j)
      out[j] += (*i)[j];
  }

  for(int j = 0; j < dim; ++j)
    out[j] /= num_points;

  return out;
}

template<const int dim>
Point<dim> Midpoint(const Point<dim>& p1, const Point<dim>& p2, CoordType dist)
{
  Point<dim> out;
  CoordType conj_dist = 1 - dist;

  for(int i = 0; i < dim; ++i)
    out.m_elem[i] = p1.m_elem[i] * conj_dist + p2.m_elem[i] * dist;

  return out;
}

template<> inline Point<2>::Point(CoordType x, CoordType y)
{
  m_elem[0] = x;
  m_elem[1] = y;
}

template<> inline Point<3>::Point(CoordType x, CoordType y, CoordType z)
{
  m_elem[0] = x;
  m_elem[1] = y;
  m_elem[2] = z;
}

template<> Point<2>& Point<2>::polar(CoordType r, CoordType theta);
template<> void Point<2>::asPolar(CoordType& r, CoordType& theta) const;

template<> Point<3>& Point<3>::polar(CoordType r, CoordType theta,
				     CoordType z);
template<> void Point<3>::asPolar(CoordType& r, CoordType& theta,
				  CoordType& z) const;
template<> Point<3>& Point<3>::spherical(CoordType r, CoordType theta,
					 CoordType phi);
template<> void Point<3>::asSpherical(CoordType& r, CoordType& theta,
				      CoordType& phi) const;

} // namespace WFMath

#endif  // WFMATH_POINT_FUNCS_H
