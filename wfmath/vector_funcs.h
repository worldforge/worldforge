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
#include <wfmath/zero.h>

#include <limits>

#include <cmath>

#include <cassert>

namespace WFMath {

template<int dim>
Vector<dim>::Vector(const Vector<dim>& v) : m_valid(v.m_valid)
{
  for(int i = 0; i < dim; ++i) {
    m_elem[i] = v.m_elem[i];
  }
}

template<int dim>
Vector<dim>::Vector(const Point<dim>& p) : m_valid(p.isValid())
{
  for(int i = 0; i < dim; ++i) {
    m_elem[i] = p.elements()[i];
  }
}

template<int dim>
const Vector<dim>& Vector<dim>::ZERO()
{
  static ZeroPrimitive<Vector<dim> > zeroVector(dim);
  return zeroVector.getShape();
}


template<int dim>
Vector<dim>& Vector<dim>::operator=(const Vector<dim>& v)
{
  m_valid = v.m_valid;

  for(int i = 0; i < dim; ++i) {
    m_elem[i] = v.m_elem[i];
  }

  return *this;
}

template<int dim>
bool Vector<dim>::isEqualTo(const Vector<dim>& v, CoordType epsilon) const
{
  double delta = _ScaleEpsilon(m_elem, v.m_elem, dim, epsilon);

  for(int i = 0; i < dim; ++i) {
    if(std::fabs(m_elem[i] - v.m_elem[i]) > delta) {
      return false;
    }
  }

  return true;
}

template <int dim>
Vector<dim>& operator+=(Vector<dim>& v1, const Vector<dim>& v2)
{
  v1.m_valid = v1.m_valid && v2.m_valid;

  for(int i = 0; i < dim; ++i) {
    v1.m_elem[i] += v2.m_elem[i];
  }

  return v1;
}

template <int dim>
Vector<dim>& operator-=(Vector<dim>& v1, const Vector<dim>& v2)
{
  v1.m_valid = v1.m_valid && v2.m_valid;

  for(int i = 0; i < dim; ++i) {
    v1.m_elem[i] -= v2.m_elem[i];
  }

  return v1;
}

template <int dim>
Vector<dim>& operator*=(Vector<dim>& v, CoordType d)
{
  for(int i = 0; i < dim; ++i) {
    v.m_elem[i] *= d;
  }

  return v;
}

template <int dim>
Vector<dim>& operator/=(Vector<dim>& v, CoordType d)
{
  for(int i = 0; i < dim; ++i) {
    v.m_elem[i] /= d;
  }

  return v;
}


template <int dim>
Vector<dim> operator-(const Vector<dim>& v)
{
  Vector<dim> ans;

  ans.m_valid = v.m_valid;

  for(int i = 0; i < dim; ++i) {
    ans.m_elem[i] = -v.m_elem[i];
  }

  return ans;
}

template<int dim>
Vector<dim>& Vector<dim>::sloppyNorm(CoordType norm)
{
  CoordType mag = sloppyMag();

  assert("need nonzero length vector" && mag > norm / std::numeric_limits<CoordType>::max());

  return (*this *= norm / mag);
}

template<int dim>
Vector<dim>& Vector<dim>::zero()
{
  m_valid = true;

  for(int i = 0; i < dim; ++i) {
    m_elem[i] = 0;
  }

  return *this;
}

template<int dim>
CoordType Angle(const Vector<dim>& v, const Vector<dim>& u)
{
  // Adding numbers with large magnitude differences can cause
  // a loss of precision, but Dot() checks for this now

  CoordType dp = FloatClamp(Dot(u, v) / std::sqrt(u.sqrMag() * v.sqrMag()),
			 -1.0, 1.0);

  CoordType angle = std::acos(dp);
 
  return angle;
}

template<int dim>
Vector<dim>& Vector<dim>::rotate(int axis1, int axis2, CoordType theta)
{
  assert(axis1 >= 0 && axis2 >= 0 && axis1 < dim && axis2 < dim && axis1 != axis2);

  CoordType tmp1 = m_elem[axis1], tmp2 = m_elem[axis2];
  CoordType stheta = std::sin(theta),
            ctheta = std::cos(theta);

  m_elem[axis1] = tmp1 * ctheta - tmp2 * stheta;
  m_elem[axis2] = tmp2 * ctheta + tmp1 * stheta;

  return *this;
}

template<int dim>
Vector<dim>& Vector<dim>::rotate(const Vector<dim>& v1, const Vector<dim>& v2,
				 CoordType theta)
{
  RotMatrix<dim> m;
  return operator=(Prod(*this, m.rotation(v1, v2, theta)));
}

template<int dim>
Vector<dim>& Vector<dim>::rotate(const RotMatrix<dim>& m)
{
  return *this = Prod(*this, m);
}

template<> Vector<3>& Vector<3>::rotate(const Vector<3>& axis, CoordType theta);
template<> Vector<3>& Vector<3>::rotate(const Quaternion& q);

template<int dim>
CoordType Dot(const Vector<dim>& v1, const Vector<dim>& v2)
{
  double delta = _ScaleEpsilon(v1.m_elem, v2.m_elem, dim);

  CoordType ans = 0;

  for(int i = 0; i < dim; ++i) {
    ans += v1.m_elem[i] * v2.m_elem[i];
  }

  return (std::fabs(ans) >= delta) ? ans : 0;
}

template<int dim>
CoordType Vector<dim>::sqrMag() const
{
  CoordType ans = 0;

  for(int i = 0; i < dim; ++i) {
    // all terms > 0, no loss of precision through cancelation
    ans += m_elem[i] * m_elem[i];
  }

  return ans;
}

template<int dim>
bool Perpendicular(const Vector<dim>& v1, const Vector<dim>& v2)
{
  CoordType max1 = 0, max2 = 0;

  for(int i = 0; i < dim; ++i) {
    CoordType val1 = std::fabs(v1[i]), val2 = std::fabs(v2[i]);
    if(val1 > max1) {
      max1 = val1;
    }
    if(val2 > max2) {
      max2 = val2;
    }
  }

  // Need to scale by both, since Dot(v1, v2) goes like the product of the magnitudes
  int exp1, exp2;
  (void) std::frexp(max1, &exp1);
  (void) std::frexp(max2, &exp2);

  return std::fabs(Dot(v1, v2)) < std::ldexp(numeric_constants<CoordType>::epsilon(), exp1 + exp2);
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

template<> CoordType Vector<2>::sloppyMag() const;
template<> CoordType Vector<3>::sloppyMag() const;

template<> CoordType Vector<1>::sloppyMag() const
	{return std::fabs(m_elem[0]);}

template<> Vector<2>::Vector(CoordType x, CoordType y) : m_valid(true)
	{m_elem[0] = x; m_elem[1] = y;}
template<> Vector<3>::Vector(CoordType x, CoordType y, CoordType z) : m_valid(true)
	{m_elem[0] = x; m_elem[1] = y; m_elem[2] = z;}

template<> Vector<2>& Vector<2>::rotate(CoordType theta)
	{return rotate(0, 1, theta);}

template<> Vector<3>& Vector<3>::rotateX(CoordType theta)
	{return rotate(1, 2, theta);}
template<> Vector<3>& Vector<3>::rotateY(CoordType theta)
	{return rotate(2, 0, theta);}
template<> Vector<3>& Vector<3>::rotateZ(CoordType theta)
	{return rotate(0, 1, theta);}


} // namespace WFMath

#endif // WFMATH_VECTOR_FUNCS_H
