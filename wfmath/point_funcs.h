// -*-C++-*-
// point_funcs.h (imported from forge/servers/pangea/MapLaw/MapPoint.cc via lib)
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


#ifndef WFMATH_POINT_FUNCS_H
#define WFMATH_POINT_FUNCS_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>

namespace WF { namespace Math {

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
bool Point<dim>::isEqualTo(const Point<dim> &rhs, double tolerance) const
{
  for(int i = 0; i < dim; ++i)
    if(!IsFloatEqual(m_elem[i], rhs.m_elem[i], tolerance))
      return false;

  return true;
}

template<const int dim>
Vector<dim> operator-(const Point<dim>& c1, const Point<dim>& c2)
{
  // FIXME friend of vector

  Vector<dim> out;

  for(int i = 0; i < dim; ++i)
    out[i] = FloatSubtract(c1.m_elem[i], c2.m_elem[i]);

  return out;
}

template<const int dim>
Point<dim> operator+(const Point<dim>& c, const Vector<dim>& v)
{
  // FIXME friend of vector

  Point<dim> out;

  for(int i = 0; i < dim; ++i)
    out.m_elem[i] = FloatAdd(c.m_elem[i], v[i]);

  return out;
}

template<const int dim>
Point<dim> operator-(const Point<dim>& c, const Vector<dim>& v)
{
  // FIXME friend of vector

  Point<dim> out;

  for(int i = 0; i < dim; ++i)
    out.m_elem[i] = FloatSubtract(c.m_elem[i], v[i]);

  return out;
}

template<const int dim>
Point<dim> operator+(const Vector<dim>& v, const Point<dim>& c)
{
  // FIXME friend of vector

  Point<dim> out;

  for(int i = 0; i < dim; ++i)
    out.m_elem[i] = FloatAdd(c.m_elem[i], v[i]);

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
Point<dim>& Point<dim>::operator=(const CoordType d[dim])
{
    for(int i = 0; i < dim; ++i)
      m_elem[i] = d[i];

    return *this;
}

template<const int dim>
Point<dim>& Point<dim>::operator+=(const Vector<dim> &rhs)
{
  // FIXME friend of vector

    for(int i = 0; i < dim; ++i)
      m_elem[i] = FloatAdd(m_elem[i], rhs[i]);

    return *this;
}

template<const int dim>
Point<dim>& Point<dim>::operator-=(const Vector<dim> &rhs)
{
  // FIXME friend of vector

    for(int i = 0; i < dim; ++i)
      m_elem[i] = FloatSubtract(m_elem[i], rhs[i]);

    return *this;
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
  for(int i = 0; i < dim; ++i) {
    if(m_elem[i] < rhs.m_elem[i])
      return true;
    if(m_elem[i] > rhs.m_elem[i])
      return false;
  }

  return false;
}

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
    CoordType diff = FloatSubtract(p1.m_elem[i], p2.m_elem[i]);
    ans += diff * diff; // Don't need FloatAdd, all terms > 0
  }

  return ans;
}

template<const int dim>
Point<dim> Barycenter(const int num_points, const Point<dim> *points)
{
  Point<dim> out;

  for(int i = 0; i < dim; ++i) {
    CoordType sum = 0, max_val = 0;
    for(int j = 0; j < num_points; ++j) {
      sum += points[j].m_elem[i];
      max_val = FloatMax(max_val, fabs(points[j].m_elem[i]));
    }
    if(fabs(sum) < max_val * WFMATH_EPSILON)
      out.m_elem[i] = 0;
    else
      out.m_elem[i] = sum / num_points;
  }

  return out;
}

template<const int dim>
Point<dim> Barycenter(const int num_points, const Point<dim> *points,
		      const CoordType *weights)
{
  CoordType tot_weight = 0, max_weight = 0;

  for(int i = 0; i < num_points; ++i) {
    tot_weight += weights[i];
    max_weight = FloatMax(max_weight, fabs(weights[i]));
  }

  assert(max_weight > 0 && fabs(tot_weight) > max_weight * WFMATH_EPSILON);

  Point<dim> out;

  for(int i = 0; i < dim; ++i) {
    out.m_elem[i] = 0;
    CoordType max_val = 0;
    for(int j = 0; j < num_points; ++j) {
      CoordType val = points[j].m_elem[i] * weights[j];
      out.m_elem[i] += val;
      max_val = FloatMax(max_val, fabs(val));
    }
    if(fabs(out.m_elem[i]) < max_val * WFMATH_EPSILON)
      out.m_elem[i] = 0;
    else
      out.m_elem[i] /= tot_weight;
  }

  return out;
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

}} // namespace WF::Math

#endif  // WFMATH_POINT_FUNCS_H
