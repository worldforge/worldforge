// vector.cpp (Vector<> implementation)
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

#include "const.h"
#include "basis.h"
#include "vector.h"
#include "quaternion.h"

using namespace WFMath;

template<> CoordType WFMath::Vector<2>::sloppyMag() const
{
  CoordType ax = fabs(m_elem[0]), ay = fabs(m_elem[1]);
  const CoordType p = Sqrt2 - 1;

  // Don't need float add, all terms > 0

  if(ax > ay)
    return ax + p * ay;
  else if (ay > 0)
    return ay + p * ax;
  else
    return 0;
}

template<> CoordType WFMath::Vector<3>::sloppyMag() const
{
  CoordType ax = fabs(m_elem[0]), ay = fabs(m_elem[1]), az = fabs(m_elem[2]);
  const CoordType p = Sqrt2 - 1;
  const CoordType q = Sqrt3 + 1 - 2 * Sqrt2;

  // Don't need FloatAdd, only term < 0 is q, it's very small,
  // and amin1 * amin2 / amax < amax.

  if(ax > ay && ax > az)
    return ax + p * (ay + az) + q * ay * az / ax;
  else if (ay > az)
    return ay + p * (ax + az) + q * ax * az / ay;
  else if (az > 0)
    return az + p * (ax + ay) + q * ax * ay / az;
  else
    return 0;
}

template<> WFMath::Vector<3>& Vector<3>::rotate(const Vector<3>& axis, CoordType theta)
{
  CoordType axis_sqr_mag = axis.sqrMag();

  assert(axis_sqr_mag != 0);

  Vector<3> perp_part = *this - axis * Dot(*this, axis) / axis_sqr_mag;
  Vector<3> rot90 = Cross(axis, perp_part) / sqrt(axis_sqr_mag);

  *this += perp_part * (cos(theta) - 1) + rot90 * sin(theta);

  return *this;
}

template<> Vector<3>& Vector<3>::rotate(const Quaternion& q)
{
  // FIXME get friend stuff working

  CoordType w = q.scalar();
  const Vector<3>& vec = q.vector();

  *this = (2 * w * w - 1) * *this
	  + 2 * vec * Dot(vec, *this)
	  + 2 * w * Cross(vec, *this);

  return *this;
}

// FIXME make the Cross() functions friends of Vector<3> somehow,
// get rid of this
template<const int dim>
static double CopyOfScaleEpsilon(const Vector<dim>& v1, const Vector<dim>& v2)
{
  double max1 = 0, max2 = 0;

  for(int i = 0; i < dim; ++i) {
    double val1 = fabs(v1[i]), val2 = fabs(v2[i]);
    if(val1 > max1)
      max1 = val1;
    if(val2 > max2)
      max2 = val2;
  }

  return _ScaleEpsilon(max1, max2, WFMATH_EPSILON);
}

CoordType WFMath::Cross(const Vector<2>& v1, const Vector<2>& v2)
{
  CoordType ans = v1[0] * v2[1] - v2[0] * v1[1];

  return (ans >= CopyOfScaleEpsilon(v1, v2)) ? ans : 0;
}

Vector<3> WFMath::Cross(const Vector<3>& v1, const Vector<3>& v2)
{
  Vector<3> ans;

  ans[0] = v1[1] * v2[2] - v2[1] * v1[2];
  ans[1] = v1[2] * v2[0] - v2[2] * v1[0];
  ans[2] = v1[0] * v2[1] - v2[0] * v1[1];

  double delta = CopyOfScaleEpsilon(v1, v2);

  for(int i = 0; i < 3; ++i)
    if(fabs(ans[i]) < delta)
      ans[i] = 0;

  return ans;
}

template<>
Vector<2>& WFMath::Vector<2>::polar(CoordType r, CoordType theta)
{
  CoordType d[2] = {r, theta};
  _PolarToCart(d, m_elem);
  return *this;
}

template<>
void WFMath::Vector<2>::asPolar(CoordType& r, CoordType& theta) const
{
  CoordType d[2];
  _CartToPolar(m_elem, d);
  r = d[0];
  theta = d[1];
}

template<>
Vector<3>& WFMath::Vector<3>::polar(CoordType r, CoordType theta,
				      CoordType z)
{
  CoordType d[2] = {r, theta};
  _PolarToCart(d, m_elem);
  m_elem[2] = z;
  return *this;
}

template<>
void WFMath::Vector<3>::asPolar(CoordType& r, CoordType& theta,
				  CoordType& z) const
{
  CoordType d[2];
  _CartToPolar(m_elem, d);
  r = d[0];
  theta = d[1];
  z = m_elem[2];
}

template<>
Vector<3>& WFMath::Vector<3>::spherical(CoordType r, CoordType theta,
					  CoordType phi)
{
  CoordType d[3] = {r, theta, phi};
  _SphericalToCart(d, m_elem);
  return *this;
}

template<>
void WFMath::Vector<3>::asSpherical(CoordType& r, CoordType& theta,
				      CoordType& phi) const
{
  CoordType d[3];
  _CartToSpherical(m_elem, d);
  r = d[0];
  theta = d[1];
  phi = d[2];
}
