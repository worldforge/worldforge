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

namespace WFMath {

template<const int dim>
Vector<dim>::Vector(const Vector<dim>& v)
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = v.m_elem[i];
}

template<const int dim>
Vector<dim>& Vector<dim>::operator=(const Vector<dim>& v)
{
  for(int i = 0; i < dim; ++i)
    m_elem[i] = v.m_elem[i];

  return *this;
}

template<const int dim>
bool Vector<dim>::isEqualTo(const Vector<dim>& v, double epsilon) const
{
  double delta = _ScaleEpsilon(m_elem, v.m_elem, dim, epsilon);

  for(int i = 0; i < dim; ++i)
    if(fabs(m_elem[i] - v.m_elem[i]) > delta)
      return false;

  return true;
}

template<const int dim>
bool Vector<dim>::operator< (const Vector<dim>& v) const
{
  if(operator==(v))
    return false;

  for(int i = 0; i < dim; ++i)
    if(m_elem[i] != v.m_elem[i])
      return m_elem[i] < v.m_elem[i];

  assert(false); // Checked for equality earlier
}

template <const int dim>
Vector<dim> operator+(const Vector<dim>& v1, const Vector<dim>& v2)
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = v1.m_elem[i] + v2.m_elem[i];

  return ans;
}

template <const int dim>
Vector<dim> operator-(const Vector<dim>& v1, const Vector<dim>& v2)
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = v1.m_elem[i] - v2.m_elem[i];

  return ans;
}

template <const int dim>
Vector<dim> operator*(const Vector<dim>& v, CoordType d)
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = v.m_elem[i] * d;

  return ans;
}

template<const int dim>
Vector<dim> operator*(CoordType d, const Vector<dim>& v)
{
  Vector<dim> ans;

  for(int i = 0; i < dim; ++i)
    ans.m_elem[i] = v.m_elem[i] * d;

  return ans;
}

template <const int dim>
Vector<dim> operator/(const Vector<dim>& v, CoordType d)
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
    v1.m_elem[i] += v2.m_elem[i];

  return v1;
}

template <const int dim>
Vector<dim>& operator-=(Vector<dim>& v1, const Vector<dim>& v2)
{
  for(int i = 0; i < dim; ++i)
    v1.m_elem[i] -= v2.m_elem[i];

  return v1;
}

template <const int dim>
Vector<dim>& operator*=(Vector<dim>& v, CoordType d)
{
  for(int i = 0; i < dim; ++i)
    v.m_elem[i] *= d;

  return v;
}

template <const int dim>
Vector<dim>& operator/=(Vector<dim>& v, CoordType d)
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
  // a loss of precision, but Dot() checks for this now

  CoordType dp = FloatClamp(Dot(u, v) / sqrt(u.sqrMag() * v.sqrMag()),
			 -1.0, 1.0);

  CoordType angle = acos(dp);
 
  return angle;
}

template<const int dim>
Vector<dim>& Vector<dim>::rotate(int axis1, int axis2, CoordType theta)
{
  CoordType tmp1 = m_elem[axis1], tmp2 = m_elem[axis2];
  CoordType stheta = sin(theta), ctheta = cos(theta);

  m_elem[axis1] = tmp1 * ctheta - tmp2 * stheta;
  m_elem[axis2] = tmp2 * ctheta + tmp1 * stheta;

  return *this;
}

template<const int dim>
Vector<dim>& Vector<dim>::rotate(const Vector<dim>& v1, const Vector<dim>& v2,
				 CoordType theta)
{
  RotMatrix<dim> m;
  return operator=(Prod(m.rotation(v1, v2, theta), *this));
}

template<> Vector<3>& Vector<3>::rotate(const Vector<3>& axis, CoordType theta);
template<> Vector<3>& Vector<3>::rotate(const Quaternion& q);

template<const int dim>
CoordType Dot(const Vector<dim>& v1, const Vector<dim>& v2)
{
  double delta = _ScaleEpsilon(v1.m_elem, v2.m_elem, dim);

  CoordType ans = 0;

  for(int i = 0; i < dim; ++i)
    ans += v1.m_elem[i] * v2.m_elem[i];

  return (fabs(ans) >= delta) ? ans : 0;
}

template<const int dim>
CoordType Vector<dim>::sqrMag() const
{
  CoordType ans = 0;

  for(int i = 0; i < dim; ++i)
    // all terms > 0, no loss of precision through cancelation
    ans += m_elem[i] * m_elem[i];

  return ans;
}

template<>
inline const CoordType Vector<1>::sloppyMagMax()
{
  return 1;
}

template<>
inline const CoordType Vector<2>::sloppyMagMax()
{
  return 1.082392200292393968799446410733;
}

template<>
inline const CoordType Vector<3>::sloppyMagMax()
{
  return 1.145934719303161490541433900265;
}

template<>
inline const CoordType Vector<1>::sloppyMagMaxSqrt()
{
  return 1;
}

template<>
inline const CoordType Vector<2>::sloppyMagMaxSqrt()
{
  return 1.040380795811030899095785063701;
}

template<>
inline const CoordType Vector<3>::sloppyMagMaxSqrt()
{
  return 1.070483404496847625250328653179;
}

// Note for people trying to compute the above numbers
// more accurately:

// The worst value for dim == 2 occurs when the ratio of the components
// of the vector is sqrt(2) - 1. The value above is equal to sqrt(4 - 2 * sqrt(2)).

// The worst value for dim == 3 occurs when the two smaller components
// are equal, and their ratio with the large component is the
// (unique, real) solution to the equation q x^3 + (q-1) x + p == 0,
// where p = sqrt(2) - 1, and q = sqrt(3) + 1 - 2 * sqrt(2).
// Running the script bc_sloppy_mag_3 provided with the WFMath source
// will calculate the above number.

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

template<> inline Vector<2>::Vector(CoordType x, CoordType y)
	{m_elem[0] = x; m_elem[1] = y;}
template<> inline Vector<3>::Vector(CoordType x, CoordType y, CoordType z)
	{m_elem[0] = x; m_elem[1] = y; m_elem[2] = z;}

template<> inline Vector<2>& Vector<2>::rotate(CoordType theta)
	{return rotate(0, 1, theta);}

template<> inline Vector<3>& Vector<3>::rotateX(CoordType theta)
	{return rotate(1, 2, theta);}
template<> inline Vector<3>& Vector<3>::rotateY(CoordType theta)
	{return rotate(2, 0, theta);}
template<> inline Vector<3>& Vector<3>::rotateZ(CoordType theta)
	{return rotate(0, 1, theta);}


} // namespace WFMath

#endif // WFMATH_VECTOR_FUNCS_H
