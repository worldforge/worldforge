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
#include <wfmath/const.h>
#include <math.h>
#include <algorithm>

namespace WF { namespace Math {

template<const int dim>
Vector<dim>::Vector(const Vector<dim>& v)
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = v.m_elem[i];
}

template<const int dim>
Vector<dim>& Vector<dim>::operator=(const double d[dim])
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
    if(!IsFloatEqual(m_elem[i], rhs.m_elem[i], tolerance));
      return false;

  return true;
}

template<const int dim>
bool Vector<dim>::operator< (const Vector<dim>& v) const
{
  for(int i = 0; i < dim; ++i) {
    if(m_elem[i] < v.m_elem[i])
      return true;
    if(m_elem[i] > v.m_elem[i])
      return false;
  }

  return false;
}

template <const int dim>
Vector<dim> Vector<dim>::operator+(const Vector<dim>& v) const
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = FloatAdd(m_elem[i], v.m_elem[i]);

  return ans;
}

template <const int dim>
Vector<dim> Vector<dim>::operator-(const Vector<dim>& v) const
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = FloatSubtract(m_elem[i], v.m_elem[i]);

  return ans;
}

template <const int dim>
Vector<dim> Vector<dim>::operator*(const double& d) const
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = m_elem[i] * d;

  return ans;
}

template<const int dim>
Vector<dim> operator*(const double& d, const Vector<dim>& v)
{
  Vector<dim> ans;

  // FIXME don't use operator[] once this function is a friend of Vector<>

  for(int i = 0; i < dim; ++i)
    ans[i] = v[i] * d;

  return ans;
}

template <const int dim>
Vector<dim> Vector<dim>::operator/(const double& d) const
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = m_elem[i] / d;

  return ans;
}

template <const int dim>
Vector<dim> Vector<dim>::operator-() const
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = -m_elem[i];

  return ans;
}

template <const int dim>
Vector<dim>& Vector<dim>::operator+=(const Vector<dim>& v)
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = FloatAdd(m_elem[i], v.m_elem[i]);

  return *this;
}

template <const int dim>
Vector<dim>& Vector<dim>::operator-=(const Vector<dim>& v)
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = FloatSubtract(m_elem[i], v.m_elem[i]);

  return *this;
}

template <const int dim>
Vector<dim>& Vector<dim>::operator*=(const double& d)
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] *= d;

  return *this;
}

template <const int dim>
Vector<dim>& Vector<dim>::operator/=(const double& d)
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] /= d;

  return *this;
}

template<const int dim>
Vector<dim>& Vector<dim>::sloppyNorm(double norm)
{
  double mag = sloppyMag();

  if(mag <= norm / DBL_MAX) // FIXME div by zero error
    return *this;

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
double Angle(const Vector<dim>& v, const Vector<dim>& u)
{
  // Adding numbers with large magnitude differences can cause
  // a loss of precision so we'll normalize the vectors before
  // taking the dot product.

  // We'll just divide out by the largest coordinate, so we
  // only have to call sqrt() once.

  double umax, vmax = 0;

  for(int i = 0; i < dim; ++i) {
     double uval = fabs(u.m_elem[i]);
     if(uval > umax)
        umax = uval;
     double vval = fabs(v.m_elem[i]);
     if(vval > vmax)
        vmax = vval;
  }

  if(uval == 0 || vval == 0) // zero length vector
    return 0; // FIXME error?

  Vector<dim> nlhs = u / umax;
  Vector<dim> nrhs = v / vmax;

  double dp = std::min(std::max(-1.0, Dot(nlhs, nrhs)
		       / sqrt(u.sqrMag() * v.sqrMag())), 1.0);

  double angle = acos(dp);
 
  return angle;
}

template<const int dim>
Vector<dim>& Vector<dim>::rotate(int axis1, int axis2, double theta)
{
  double tmp1 = m_elem[axis1], tmp2 = m_elem[axis2];
  double stheta = sin(theta), ctheta = cos(theta);

  m_elem[axis1] = FloatSubtract(tmp1 * ctheta, tmp2 * stheta);
  m_elem[axis2] = FloatAdd(tmp2 * ctheta, tmp1 * stheta);

  return *this;
}

template<const int dim>
Vector<dim>& Vector<dim>::rotate(const Vector<dim>& v1, const Vector<dim>& v2,
	double theta)
{
  RotMatrix<dim> m;

  m.rotation(v1, v2, theta);

  *this = Prod(m, *this);

  return *this;
}

template<> Vector<3>& Vector<3>::rotate(const Vector<3>& axis, double theta);

template<const int dim>
double Dot(const Vector<dim>& v1, const Vector<dim>& v2)
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
double Vector<dim>::sqrMag() const
{
  CoordType ans = 0;

  for(int i = 0; i < dim; ++i)
    ans += m_elem[i] * m_elem[i]; // Don't need FloatAdd, all terms > 0

  return ans;
}

template<> Vector<2>& Vector<2>::polar(double r, double theta);
template<> void Vector<2>::asPolar(double& r, double& theta) const;

template<> Vector<3>& Vector<3>::polar(double r, double theta, double z);
template<> void Vector<3>::asPolar(double& r, double& theta, double& z) const;
template<> Vector<3>& Vector<3>::spherical(double r, double theta, double phi);
template<> void Vector<3>::asSpherical(double& r, double& theta, double& phi) const;


}} // namespace WF::Math

#endif // WFMATH_VECTOR_FUNCS_H
