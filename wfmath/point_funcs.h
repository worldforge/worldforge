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

#include <wfmath/point.h>

#include <wfmath/vector.h>
#include <wfmath/zero.h>

#include <cmath>

namespace WFMath {

template<int dim>
inline Point<dim>::Point(const Point<dim>& p) : m_valid(p.m_valid)
{
  for(int i = 0; i < dim; ++i) {
    m_elem[i] = p.m_elem[i];
  }
}

template<int dim>
inline Point<dim>::Point(const Vector<dim>& v) : m_valid(v.isValid())
{
  for(int i = 0; i < dim; ++i) {
    m_elem[i] = v.elements()[i];
  }
}

template<int dim>
const Point<dim>& Point<dim>::ZERO()
{
  static ZeroPrimitive<Point<dim> > zeroPoint(dim);
  return zeroPoint.getShape();
}


template<int dim>
inline Point<dim>& Point<dim>::setToOrigin()
{
  for(int i = 0; i < dim; ++i) {
    m_elem[i] = 0;
  }

  m_valid = true;

  return *this;
}

template<int dim>
inline bool Point<dim>::isEqualTo(const Point<dim> &p, CoordType epsilon) const
{
  CoordType delta = _ScaleEpsilon(m_elem, p.m_elem, dim, epsilon);

  for(int i = 0; i < dim; ++i) {
    if(std::fabs(m_elem[i] - p.m_elem[i]) > delta) {
      return false;
    }
  }

  return true;
}

template<int dim>
inline Vector<dim> operator-(const Point<dim>& c1, const Point<dim>& c2)
{
  Vector<dim> out;

  for(int i = 0; i < dim; ++i) {
    out.m_elem[i] = c1.m_elem[i] - c2.m_elem[i];
  }

  out.m_valid = c1.m_valid && c2.m_valid;

  return out;
}

template<int dim>
inline Point<dim>& operator+=(Point<dim>& p, const Vector<dim> &rhs)
{
    for(int i = 0; i < dim; ++i) {
      p.m_elem[i] += rhs.m_elem[i];
    }

    p.m_valid = p.m_valid && rhs.m_valid;

    return p;
}

template<int dim>
inline Point<dim> operator+(const Point<dim>& c, const Vector<dim>& v)
{
  Point<dim> out(c);

  out += v;

  return out;
}

template<int dim>
inline Point<dim> operator+(const Vector<dim>& v, const Point<dim>& c)
{
  Point<dim> out(c);

  out += v;

  return out;
}

template<int dim>
inline Point<dim>& operator-=(Point<dim>& p, const Vector<dim> &rhs)
{
    for(int i = 0; i < dim; ++i) {
      p.m_elem[i] -= rhs.m_elem[i];
    }

    p.m_valid = p.m_valid && rhs.m_valid;

    return p;
}

template<int dim>
inline Point<dim> operator-(const Point<dim>& c, const Vector<dim>& v)
{
  Point<dim> out(c);

  out -= v;

  return out;
}

template<int dim>
inline Point<dim>& Point<dim>::operator=(const Point<dim>& rhs)
{
    // Compare pointer addresses
    if (this == &rhs) {
      return *this;
    }

    for(int i = 0; i < dim; ++i) {
      m_elem[i] = rhs.m_elem[i];
    }

    m_valid = rhs.m_valid;

    return *this;
}

template<int dim>
inline CoordType SquaredDistance(const Point<dim>& p1, const Point<dim>& p2)
{
  CoordType ans = 0;

  for(int i = 0; i < dim; ++i) {
    CoordType diff = p1.m_elem[i] - p2.m_elem[i];
    ans += diff * diff;
  }

  return (std::fabs(ans) >= _ScaleEpsilon(p1.m_elem, p2.m_elem, dim)) ? ans : 0;
}

template<int dim, template<class, class> class container,
			template<class, class> class container2>
Point<dim> Barycenter(const container<Point<dim>, std::allocator<Point<dim> > >& c,
		      const container2<CoordType, std::allocator<CoordType> >& weights)
{
  // FIXME become friend

  typename container<Point<dim>, std::allocator<Point<dim> > >::const_iterator c_i = c.begin(), c_end = c.end();
  typename container2<CoordType, std::allocator<CoordType> >::const_iterator w_i = weights.begin(),
						 w_end = weights.end();

  Point<dim> out;

  if (c_i == c_end || w_i == w_end) {
    return out;
  }

  bool valid = c_i->isValid();

  CoordType tot_weight = *w_i, max_weight = std::fabs(*w_i);
  for(int j = 0; j < dim; ++j) {
    out[j] = (*c_i)[j] * *w_i;
  }

  while(++c_i != c_end && ++w_i != w_end) {
    tot_weight += *w_i;
    CoordType val = std::fabs(*w_i);
    if(val > max_weight)
      max_weight = val;
    if(!c_i->isValid())
      valid = false;
    for(int j = 0; j < dim; ++j)
      out[j] += (*c_i)[j] * *w_i;
  }

  // Make sure the weights don't add up to zero
  if (max_weight <= 0 || std::fabs(tot_weight) <= max_weight * WFMATH_EPSILON) {
    return out;
  }

  for(int j = 0; j < dim; ++j) {
    out[j] /= tot_weight;
  }

  out.setValid(valid);

  return out;
}

template<int dim, template<class, class> class container>
Point<dim> Barycenter(const container<Point<dim>, std::allocator<Point<dim> > >& c)
{
  // FIXME become friend

  typename container<Point<dim>, std::allocator<Point<dim> > >::const_iterator i = c.begin(), end = c.end();

  if (i == end) {
    return Point<dim>();
  }

  Point<dim> out = *i;
  float num_points = 1;

  bool valid = i->isValid();

  while(++i != end) {
    ++num_points;
    if(!i->isValid())
      valid = false;
    for(int j = 0; j < dim; ++j)
      out[j] += (*i)[j];
  }

  for(int j = 0; j < dim; ++j) {
    out[j] /= num_points;
  }

  out.setValid(valid);

  return out;
}

template<int dim>
inline Point<dim> Midpoint(const Point<dim>& p1, const Point<dim>& p2, CoordType dist)
{
  Point<dim> out;
  CoordType conj_dist = 1 - dist;

  for(int i = 0; i < dim; ++i) {
    out.m_elem[i] = p1.m_elem[i] * conj_dist + p2.m_elem[i] * dist;
  }

  out.m_valid = p1.m_valid && p2.m_valid;

  return out;
}

template<> Point<2>::Point(CoordType x, CoordType y) : m_valid(true)
{
  m_elem[0] = x;
  m_elem[1] = y;
}

template<> Point<3>::Point(CoordType x, CoordType y, CoordType z) : m_valid(true)
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
