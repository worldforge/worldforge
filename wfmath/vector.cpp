// -*-C++-*-
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

#include "vector_funcs.h"
#include "const.h"
#include "basis.h"
#include <math.h>

using namespace WF::Math;

template<> double WF::Math::Vector<2>::sloppyMag() const
{
  CoordType ax = fabs(m_elem[0]), ay = fabs(m_elem[1]);
  const CoordType p = WFMATH_CONST_SQRT2 - 1;

  // Don't need float add, all terms > 0

  if(ax > ay)
    return ax + p * ay;
  else if (ay > 0)
    return ay + p * ax;
  else
    return 0;
}

template<> double WF::Math::Vector<3>::sloppyMag() const
{
  CoordType ax = fabs(m_elem[0]), ay = fabs(m_elem[1]), az = fabs(m_elem[2]);
  const CoordType p = WFMATH_CONST_SQRT2 - 1;
  const CoordType q = WFMATH_CONST_SQRT3 + 1 - 2 * WFMATH_CONST_SQRT2;

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

template<> WF::Math::Vector<3>& Vector<3>::rotate(const Vector<3>& axis, double theta)
{
  double axis_sqr_mag = axis.sqrMag();

  assert(axis_sqr_mag != 0);

  Vector<3> perp_part = *this - axis * Dot(*this, axis) / axis_sqr_mag;
  Vector<3> rot90 = Cross(axis, perp_part) / sqrt(axis_sqr_mag);

  *this += perp_part * (cos(theta) - 1) + rot90 * sin(theta);

  return *this;
}

Vector<3> WF::Math::Cross(const Vector<3>& v1, const Vector<3>& v2)
{
  Vector<3> ans;

  ans[0] = FloatSubtract(v1[1] * v2[2], v2[1] * v1[2]);
  ans[1] = FloatSubtract(v1[2] * v2[0], v2[2] * v1[0]);
  ans[2] = FloatSubtract(v1[0] * v2[1], v2[0] * v1[1]);

  return ans;
}

template<>
Vector<2>& WF::Math::Vector<2>::polar(double r, double theta)
{
  double d[2] = {r, theta};
  _PolarToCart(d, m_elem);
  return *this;
}

template<>
void WF::Math::Vector<2>::asPolar(double& r, double& theta) const
{
  double d[2];
  _CartToPolar(m_elem, d);
  r = d[0];
  theta = d[1];
}

template<>
Vector<3>& WF::Math::Vector<3>::polar(double r, double theta, double z)
{
  double d[2] = {r, theta};
  _PolarToCart(d, m_elem);
  m_elem[2] = z;
  return *this;
}

template<>
void WF::Math::Vector<3>::asPolar(double& r, double& theta, double& z) const
{
  double d[2];
  _CartToPolar(m_elem, d);
  r = d[0];
  theta = d[1];
  z = m_elem[2];
}

template<>
Vector<3>& WF::Math::Vector<3>::spherical(double r, double theta, double phi)
{
  double d[3] = {r, theta, phi};
  _SphericalToCart(d, m_elem);
  return *this;
}

template<>
void WF::Math::Vector<3>::asSpherical(double& r, double& theta, double& phi) const
{
  double d[3];
  _CartToSpherical(m_elem, d);
  r = d[0];
  theta = d[1];
  phi = d[2];
}
