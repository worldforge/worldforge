// -*-C++-*-
// vector_funcs.h (Vector<> template functions)
//
//  The WorldForge Project
//  Copyright (C) 2001  The WorldForge Project
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

// Author: Ron Steinke
// Created: 2001-12-7

// Extensive amounts of this material come from the Vector2D
// and Vector3D classes from stage/math, written by Bryce W.
// Harrington, Kosh, and Jari Sundell (Rakshasa).

#ifndef WFMATH_VECTOR_FUNCS_H
#define WFMATH_VECTOR_FUNCS_H

#include <wfmath/vector.h>
#include <wfmath/rotmatrix.h>
#include <wfmath/const.h>

namespace WF { namespace Math {

template<const int dim>
Vector<dim>::Vector(const Vector<dim>& v)
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = v.m_elem[i];
}

template<const int dim>
Vector<dim>& Vector<dim>::operator=(const CoordType d[dim])
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = d.m_elem[i];

  return *this;
}

template<const int dim>
Vector<dim>& Vector<dim>::operator=(const Vector<dim>& v)
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = v.m_elem[i];

  return *this;
}

template<const int dim>
bool Vector<dim>::isEqualTo(const Vector<dim>& rhs, double tolerance) const
{
  for(int i = 0; i < dim; ++i)
    if(!IsFloatEqual(m_elem[i], rhs.m_elem[i], tolerance))
      return false;

  return true;
}

template<const int dim>
bool Vector<dim>::operator< (const Vector<dim>& v) const
{
  for(int i = 0; i < dim; ++i)
    if(!IsFloatEqual(m_elem[i], v.m_elem[i]))
      return m_elem[i] < v.m_elem[i];

  return false;
}

template <const int dim>
Vector<dim> operator+(const Vector<dim>& v1, const Vector<dim>& v2)
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = FloatAdd(v1.m_elem[i], v2.m_elem[i]);

  return ans;
}

template <const int dim>
Vector<dim> operator-(const Vector<dim>& v1, const Vector<dim>& v2)
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = FloatSubtract(v1.m_elem[i], v2.m_elem[i]);

  return ans;
}

template <const int dim>
Vector<dim> operator*(const Vector<dim>& v, const CoordType& d)
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = v.m_elem[i] * d;

  return ans;
}

template<const int dim>
Vector<dim> operator*(const CoordType& d, const Vector<dim>& v)
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = v.m_elem[i] * d;

  return ans;
}

template <const int dim>
Vector<dim> operator/(const Vector<dim>& v, const CoordType& d)
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = v.m_elem[i] / d;

  return ans;
}

template <const int dim>
Vector<dim> operator-(const Vector<dim>& v)
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = -v.m_elem[i];

  return ans;
}

template <const int dim>
Vector<dim>& operator+=(Vector<dim>& v1, const Vector<dim>& v2)
{
  for(int i = 0; i < dim; ++i)
    v1.m_elem[i] = FloatAdd(v1.m_elem[i], v2.m_elem[i]);

  return v1;
}

template <const int dim>
Vector<dim>& operator-=(Vector<dim>& v1, const Vector<dim>& v2)
{
  for(int i = 0; i < dim; ++i)
    v1.m_elem[i] = FloatSubtract(v1.m_elem[i], v2.m_elem[i]);

  return v1;
}

template <const int dim>
Vector<dim>& operator*=(Vector<dim>& v, const CoordType& d)
{
  for(int i = 0; i < dim; ++i)
    v.m_elem[i] *= d;

  return v;
}

template <const int dim>
Vector<dim>& operator/=(Vector<dim>& v, const CoordType& d)
{
  for(int i = 0; i < dim; ++i)
    v.m_elem[i] /= d;

  return v;
}

template<const int dim>
Vector<dim>& Vector<dim>::sloppyNorm(CoordType norm)
{
  CoordType mag = sloppyMag();

  assert(mag > norm / WFMATH_MAX); // nonzero length vector

  return (*this *= norm / mag);
}

template<const int dim>
Vector<dim>& Vector<dim>::zero()
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = 0;

  return *this;
}

template<const int dim>
CoordType Angle(const Vector<dim>& v, const Vector<dim>& u)
{
  // Adding numbers with large magnitude differences can cause
  // a loss of precision so we'll normalize the vectors before
  // taking the dot product.

  // We'll just divide out by the largest coordinate, so we
  // only have to call sqrt() once.

  CoordType umax, vmax = 0;

  for(int i = 0; i < dim; ++i) {
     CoordType uval = fabs(u.m_elem[i]);
     if(uval > umax)
        umax = uval;
     CoordType vval = fabs(v.m_elem[i]);
     if(vval > vmax)
        vmax = vval;
  }

  assert(uval != 0 && vval != 0); // zero length vector

  Vector<dim> nlhs = u / umax;
  Vector<dim> nrhs = v / vmax;

  CoordType dp = FloatClamp(Dot(nlhs, nrhs) / sqrt(u.sqrMag() * v.sqrMag()),
			 -1.0, 1.0);

  CoordType angle = acos(dp);
 
  return angle;
}

template<const int dim>
Vector<dim>& Vector<dim>::rotate(int axis1, int axis2, CoordType theta)
{
  CoordType tmp1 = m_elem[axis1], tmp2 = m_elem[axis2];
  CoordType stheta = sin(theta), ctheta = cos(theta);

  m_elem[axis1] = FloatSubtract(tmp1 * ctheta, tmp2 * stheta);
  m_elem[axis2] = FloatAdd(tmp2 * ctheta, tmp1 * stheta);

  return *this;
}

template<> Vector<3>& Vector<3>::rotate(const Vector<3>& axis, CoordType theta);
template<> Vector<3>& Vector<3>::rotate(const Quaternion& q);

template<const int dim>
CoordType Dot(const Vector<dim>& v1, const Vector<dim>& v2)
{
  CoordType ans = 0, max_val = 0;

  for(int i = 0; i < dim; ++i) {
    CoordType val = v1.m_elem[i] * v2.m_elem[i];
    ans += val;
    CoordType aval = fabs(val);
    if(aval > max_val)
      max_val = aval;
  }

  if(fabs(ans/max_val) < WFMATH_EPSILON)
     return 0;
  else
    return ans;
}

template<const int dim>
CoordType Vector<dim>::sqrMag() const
{
  CoordType ans = 0;

  for(int i = 0; i < dim; ++i)
    ans += m_elem[i] * m_elem[i]; // Don't need FloatAdd, all terms > 0

  return ans;
}

template<> Vector<2>& Vector<2>::polar(CoordType r, CoordType theta);
template<> void Vector<2>::asPolar(CoordType& r, CoordType& theta) const;

template<> Vector<3>& Vector<3>::polar(CoordType r, CoordType theta,
				       CoordType z);
template<> void Vector<3>::asPolar(CoordType& r, CoordType& theta,
				   CoordType& z) const;
template<> Vector<3>& Vector<3>::spherical(CoordType r, CoordType theta,
					   CoordType phi);
template<> void Vector<3>::asSpherical(CoordType& r, CoordType& theta,
				       CoordType& phi) const;

template<> inline CoordType Vector<1>::sloppyMag() const {return fabs(m_elem[0]);}
template<> CoordType Vector<2>::sloppyMag() const;
template<> CoordType Vector<3>::sloppyMag() const;

template<> inline Vector<2>::Vector(const CoordType& x, const CoordType& y)
	{m_elem[0] = x; m_elem[1] = y;}
template<> inline Vector<3>::Vector(const CoordType& x, const CoordType& y,
				    const CoordType& z)
	{m_elem[0] = x; m_elem[1] = y; m_elem[2] = z;}

template<> inline Vector<2>& Vector<2>::rotate(CoordType theta)
	{return rotate(0, 1, theta);}

template<> inline Vector<3>& Vector<3>::rotateX(CoordType theta)
	{return rotate(1, 2, theta);}
template<> inline Vector<3>& Vector<3>::rotateY(CoordType theta)
	{return rotate(2, 0, theta);}
template<> inline Vector<3>& Vector<3>::rotateZ(CoordType theta)
	{return rotate(0, 1, theta);}


}} // namespace WF::Math

#endif // WFMATH_VECTOR_FUNCS_H
