// quaternion.cpp (Quaternion implementation)
//
//  The WorldForge Project
//  Copyright (C) 2002  The WorldForge Project
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

// Some code here was taken from the quaternion implementation is
// eris. Some of the other algorithms are based on information
// found here <http://www.cs.ualberta.ca/~andreas/math/matrfaq_latest.html>
// and here <http://www.cs.berkeley.edu/~laura/cs184/quat/quaternion.html>.

#include "quaternion.h"
#include "rotmatrix.h"

using namespace WFMath;

Quaternion::Quaternion (const CoordType w_in, const CoordType x_in,
			const CoordType y_in, const CoordType z_in)
{
  CoordType norm = sqrt(w_in*w_in + x_in*x_in + y_in*y_in + z_in*z_in);

  m_w = w_in / norm;
  m_vec[0] = x_in / norm;
  m_vec[1] = y_in / norm;
  m_vec[2] = z_in / norm;
}

bool Quaternion::isEqualTo(const Quaternion &rhs, double tolerance) const
{
  return IsFloatEqual(m_w, rhs.m_w, tolerance)
      && m_vec.isEqualTo(rhs.m_vec, tolerance);
}

bool Quaternion::operator< (const Quaternion& rhs) const
{
  if(!IsFloatEqual(m_w, rhs.m_w))
    return m_w < rhs.m_w;
  return m_vec < rhs.m_vec;
}

Quaternion Quaternion::operator* (const Quaternion& rhs) const
{
  Quaternion out;

  out.m_w = FloatSubtract(m_w * rhs.m_w, Dot(m_vec, rhs.m_vec));
  out.m_vec = m_w * rhs.m_vec + rhs.m_w * m_vec + Cross(m_vec, rhs.m_vec);

  return out;
}

Quaternion Quaternion::operator/ (const Quaternion& rhs) const
{
  Quaternion out;


  out.m_w = FloatAdd(m_w * rhs.m_w, Dot(m_vec, rhs.m_vec));
  out.m_vec = rhs.m_w * m_vec - m_w * rhs.m_vec - Cross(m_vec, rhs.m_vec);

  return out;
}

bool Quaternion::fromRotMatrix(const RotMatrix<3>& m)
{
  RotMatrix<3> m_tmp;
  bool not_flip;

  if(m.determinant() > 0) { // This is cheap for a RotMatrix<>
    not_flip = true;
  }
  else {
    m_tmp = Prod(m, RotMatrix<3>().mirrorX());
    not_flip = false;
  }

  const RotMatrix<3> &m_ref = not_flip ? m : m_tmp;

  CoordType s;
  const int nxt[3] = {1, 2, 0};
  CoordType tr = m_ref.trace();

  // check the diagonal
  if (tr > 0.0) {
    s = sqrt (tr + 1.0);
    m_w = s / 2.0;
    s = 0.5 / s;

    m_vec[0] = FloatSubtract(m_ref.elem(2, 1), m_ref.elem(1, 2)) * s;
    m_vec[1] = FloatSubtract(m_ref.elem(0, 2), m_ref.elem(2, 0)) * s;
    m_vec[2] = FloatSubtract(m_ref.elem(1, 0), m_ref.elem(0, 1)) * s;
  } else {
    // diagonal is negative
    int i = 0;

    if (m_ref.elem(1, 1) > m_ref.elem(0, 0)) i = 1;
    if (m_ref.elem(2, 2) > m_ref.elem(i, i)) i = 2;

    int j = nxt[i], k = nxt[j];

    // We know the result of the subtraction is non-negative, so we
    // don't need FloatAdd() to add the 1.0.
    s = sqrt (1.0 + FloatSubtract(m_ref.elem(i, i),
				  FloatAdd(m_ref.elem(j, j),
					   m_ref.elem(k, k))));
    m_vec[i] = s * 0.5;

    assert(s > 0.0);
    s = 0.5 / s;

    m_w = FloatSubtract(m_ref.elem(k, j), m_ref.elem(j, k)) * s;
    m_vec[j] = FloatAdd(m_ref.elem(i, j), m_ref.elem(j, i)) * s;
    m_vec[k] = FloatAdd(m_ref.elem(i, k), m_ref.elem(k, i)) * s;
  }

  return not_flip;
}

Quaternion& Quaternion::rotation(int axis, const CoordType angle)
{
  CoordType half_angle = angle / 2;

  m_w = cos(half_angle);
  m_vec.zero();
  m_vec[axis] = sin(half_angle);

  return *this;
}

Quaternion& Quaternion::rotation(const Vector<3>& axis, const CoordType angle)
{
  CoordType half_angle = angle / 2;

  m_w = cos(half_angle);
  m_vec = sin(half_angle) * axis / axis.mag();

  return *this;
}

Quaternion& Quaternion::fromEuler(const CoordType alpha, const CoordType beta,
				  const CoordType gamma)
{
  rotation(2, gamma); // Do the last rotation first, since we'll right multiplying
  operator*=(Quaternion(1, beta));
  operator*=(Quaternion(0, alpha));

  return *this;
}

void Quaternion::toEuler(CoordType& alpha, CoordType& beta, CoordType& gamma)
{
  // This is _really_ complicated, so just use the matrix version so we
  // only have to do this once.

  // FIXME reverse this so that the matrix version uses this one?
  // Will that be simpler?

  RotMatrix<3> m(*this);
  CoordType d[3];

  bool not_flip = m.toEuler(d);
  assert(not_flip);

  alpha = d[0];
  beta = d[1];
  gamma = d[2];
}
