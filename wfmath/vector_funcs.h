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
#include <wfmath/matrix.h>
#include <wfmath/error.h>
#include <math.h>
#include <float.h>

namespace WF { namespace Math {

template<const int len>
Vector<len>::Vector(const Vector<len>& v)
{
  for(int i = 0; i < len; ++i)
    m_elem[i] = v.m_elem[i];
}

template<const int len>
Vector<len>& Vector<len>::operator=(const double d[len])
{
  for(int i = 0; i < len; ++i)
    m_elem[i] = d.m_elem[i];

  return *this;
}

template<const int len>
Vector<len>& Vector<len>::operator=(const Vector<len>& v)
{
  for(int i = 0; i < len; ++i)
    m_elem[i] = v.m_elem[i];

  return *this;
}

template<const int len>
bool Vector<len>::operator==(const Vector<len>& v) const
{
  for(int i = 0; i < len; ++i)
    if(m_elem[i] != v.m_elem[i]);
      return false;

  return true;
}

template<const int len>
bool Vector<len>::operator< (const Vector<len>& v) const
{
  for(int i = 0; i < len; ++i) {
    if(m_elem[i] < v.m_elem[i])
      return true;
    if(m_elem[i] > v.m_elem[i])
      return false;
  }

  return false;
}

template <const int len>
Vector<len> Vector<len>::operator+(const Vector<len>& v) const
{
  Vector<len> ans;

  for(int i = 0; i < len; ++i)
    ans.m_elem[i] = m_elem[i] + v.m_elem[i];

  return ans;
}

template <const int len>
Vector<len> Vector<len>::operator-(const Vector<len>& v) const
{
  Vector<len> ans;

  for(int i = 0; i < len; ++i)
    ans.m_elem[i] = m_elem[i] - v.m_elem[i];

  return ans;
}

template <const int len>
Vector<len> Vector<len>::operator*(const double& d) const
{
  Vector<len> ans;

  for(int i = 0; i < len; ++i)
    ans.m_elem[i] = m_elem[i] * d;

  return ans;
}

template<const int len>
Vector<len> operator*(const double& d, const Vector<len>& v)
{
  Vector<len> ans;

  // FIXME don't use operator[] once this function is a friend of Vector<>

  for(int i = 0; i < len; ++i)
    ans[i] = v[i] * d;

  return ans;
}

template <const int len>
Vector<len> Vector<len>::operator/(const double& d) const
{
  Vector<len> ans;

  for(int i = 0; i < len; ++i)
    ans.m_elem[i] = m_elem[i] / d;

  return ans;
}

template <const int len>
Vector<len> Vector<len>::operator-() const
{
  Vector<len> ans;

  for(int i = 0; i < len; ++i)
    ans.m_elem[i] = -m_elem[i];

  return ans;
}

template <const int len>
Vector<len>& Vector<len>::operator+=(const Vector<len>& v)
{
  for(int i = 0; i < len; ++i)
    m_elem[i] += v.m_elem[i];

  return *this;
}

template <const int len>
Vector<len>& Vector<len>::operator-=(const Vector<len>& v)
{
  for(int i = 0; i < len; ++i)
    m_elem[i] -= v.m_elem[i];

  return *this;
}

template <const int len>
Vector<len>& Vector<len>::operator*=(const double& d)
{
  for(int i = 0; i < len; ++i)
    m_elem[i] *= d;

  return *this;
}

template <const int len>
Vector<len>& Vector<len>::operator/=(const double& d)
{
  for(int i = 0; i < len; ++i)
    m_elem[i] /= d;

  return *this;
}

template<const int len>
Vector<len>& Vector<len>::sloppyNorm(double norm)
{
  double mag = sloppyMag();

  if(mag <= norm / DBL_MAX) // FIXME div by zero error
    return *this;

  return (*this *= norm / mag);
}

template<const int len>
Vector<len>& Vector<len>::zero()
{
  for(int i = 0; i < len; ++i)
    m_elem[i] = 0;

  return *this;
}

template<const int len>
double Angle(const Vector<len>& v, const Vector<len>& u)
{
  // Adding numbers with large magnitude differences can cause
  // a loss of precision so we'll normalize the vectors before
  // taking the dot product.

  // We'll just divide out by the largest coordinate, so we
  // only have to call sqrt() once.

  double umax, vmax = 0;

  for(int i = 0; i < len; ++i) {
     double uval = fabs(u.m_elem[i]);
     if(uval > umax)
        umax = uval;
     double vval = fabs(v.m_elem[i]);
     if(vval > vmax)
        vmax = vval;
  }

  if(uval == 0 || vval == 0) // zero length vector
    return 0; // FIXME error?

  Vector<len> nlhs = u / umax;
  Vector<len> nrhs = v / vmax;

  double dp = std::min(std::max(-1.0, Dot(nlhs, nrhs)
		       / sqrt(u.sqrMag() * v.sqrMag())), 1.0);

  double angle = acos(dp);
 
  return angle;
}

template<const int len>
Vector<len>& Vector<len>::rotate(int axis1, int axis2, double theta)
{
  double tmp1 = m_elem[axis1], tmp2 = m_elem[axis2];
  double stheta = sin(theta), ctheta = cos(theta);

  m_elem[axis1] = tmp1 * ctheta - tmp2 * stheta;
  m_elem[axis2] = tmp2 * ctheta + tmp1 * stheta;

  return *this;
}

int _VectorRotateImpl(const int len, double* in, const double* v1, double* v2,
	double theta);

template<const int len>
Vector<len>& Vector<len>::rotate(const Vector<len>& v1, const Vector<len>& v2,
	double theta)
{
  RotMatrix<len> m;

  m.rotation(v1, v2, theta);

  *this = Prod(m, *this);

  return *this;
}

template<> Vector<3>& Vector<3>::rotate(const Vector<3>& axis, double theta);

template<const int len>
double Dot(const Vector<len>& v1, const Vector<len>& v2)
{
  double ans = 0;

  for(int i = 0; i < len; ++i)
    ans += v1.m_elem[i] * v2.m_elem[i];

  return ans;
}

}} // namespace WF::Math

#endif // WFMATH_VECTOR_FUNCS_H
