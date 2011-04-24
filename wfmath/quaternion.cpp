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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quaternion.h"
#include "error.h"
#include "rotmatrix.h"
#include "floatmath.h"

#include <cmath>

#include <cassert>

using namespace WFMath;

Quaternion::Quaternion (CoordType w_in, CoordType x_in, CoordType y_in, CoordType z_in)
	: m_valid(true), m_age(1)
{
  CoordType norm = F_SQRT(w_in*w_in + x_in*x_in + y_in*y_in + z_in*z_in);

  m_w = w_in / norm;
  m_vec[0] = x_in / norm;
  m_vec[1] = y_in / norm;
  m_vec[2] = z_in / norm;
  m_vec.setValid();
}

// The equality functions regard q and -q as equal, since they
// correspond to the same rotation matrix. We consider the form
// of the quaternion with w > 0 canonical.

bool Quaternion::isEqualTo(const Quaternion &q, double epsilon) const
{
  // Since the sum of squares is 1, the magnitude of the largest
  // element must be between 1 and 0.5, so we don't need to scale epsilon.

  assert(epsilon > 0);

  if(fabs(m_w - q.m_w) <= epsilon) {
    int i;
    for(i = 0; i < 3; ++i)
      if(fabs(m_vec[i] - q.m_vec[i]) > epsilon)
        break; // try again with swapped signs
    if(i == 3) // got through the loop
      return true;
  }

  // This makes q == -q true
  if(fabs(m_w + q.m_w) <= epsilon) {
    for(int i = 0; i < 3; ++i)
      if(fabs(m_vec[i] + q.m_vec[i]) > epsilon)
        return false;
    return true;
  }

  return false;
}

// order of multiplication vs sense of rotation
//
// v.rotate(q1).rotate(q2) is the same as v.rotate(q1 * q2),
// the same as with matrices

Quaternion& Quaternion::operator*= (const Quaternion& rhs)
{
  m_valid = m_valid && rhs.m_valid;
  m_age = m_age + rhs.m_age;
  checkNormalization();

  CoordType old_w = m_w;
  m_w = m_w * rhs.m_w - Dot(m_vec, rhs.m_vec);
  m_vec = old_w * rhs.m_vec + rhs.m_w * m_vec - Cross(m_vec, rhs.m_vec);

  return *this;
}

Quaternion& Quaternion::operator/= (const Quaternion& rhs)
{
  m_valid = m_valid && rhs.m_valid;
  m_age = m_age + rhs.m_age;
  checkNormalization();

  CoordType old_w = m_w;
  m_w = m_w * rhs.m_w + Dot(m_vec, rhs.m_vec);
  m_vec = rhs.m_w * m_vec - old_w * rhs.m_vec + Cross(m_vec, rhs.m_vec);

  return *this;
}

bool Quaternion::fromRotMatrix(const RotMatrix<3>& m)
{
  RotMatrix<3> m_tmp;
  bool not_flip = !m.parity();

  m_valid = m.isValid();
  m_vec.setValid(m.isValid());

  if(!not_flip)
    m_tmp = Prod(m, RotMatrix<3>().mirrorX());

  const RotMatrix<3> &m_ref = not_flip ? m : m_tmp;

  CoordType s;
  const int nxt[3] = {1, 2, 0};
  CoordType tr = m_ref.trace();

  // check the diagonal
  if (tr > 0.0) {
    s = F_SQRT(tr + 1.0f);
    m_w = (s / 2.0f);
    s = (0.5f / s);

    m_vec[0] = -(m_ref.elem(2, 1) - m_ref.elem(1, 2)) * s;
    m_vec[1] = -(m_ref.elem(0, 2) - m_ref.elem(2, 0)) * s;
    m_vec[2] = -(m_ref.elem(1, 0) - m_ref.elem(0, 1)) * s;
  } else {
    // diagonal is negative
    int i = 0;

    if (m_ref.elem(1, 1) > m_ref.elem(0, 0)) i = 1;
    if (m_ref.elem(2, 2) > m_ref.elem(i, i)) i = 2;

    int j = nxt[i], k = nxt[j];

    s = F_SQRT (1.0f + m_ref.elem(i, i) - m_ref.elem(j, j) - m_ref.elem(k, k));
    m_vec[i] = -(s * 0.5f);

    assert("sqrt() returns positive" && s > 0.0);
    s = (0.5f / s);

    m_w = (m_ref.elem(k, j) - m_ref.elem(j, k)) * s;
    m_vec[j] = -(m_ref.elem(i, j) + m_ref.elem(j, i)) * s;
    m_vec[k] = -(m_ref.elem(i, k) + m_ref.elem(k, i)) * s;
  }

  m_age = m.age();

  return not_flip;
}

Quaternion Quaternion::inverse() const
{
  Quaternion q(m_valid);
  q.m_w = m_w;
  q.m_vec = -m_vec;
  q.m_age = m_age; // no multiplication was done, so roundoff error does not increase
  return q;
}

Quaternion& Quaternion::rotate(const RotMatrix<3>& m)
{
  // FIXME find a more efficient way to do this
  Quaternion tmp;
  tmp.fromRotMatrix(m);
  *this *= tmp;
  return *this;
}

Quaternion& Quaternion::rotation(int axis, CoordType angle)
{
  if (axis < 0 || axis > 2) {
    m_valid = false;
    return *this;
  }

  CoordType half_angle = angle / 2;

  m_w = F_COS(half_angle);
  for(int i = 0; i < 3; ++i)
    // Note sin() only called once
    m_vec[i] = (i == axis) ? F_SIN(half_angle) : 0;
  m_vec.setValid();

  m_valid = true;
  m_age = 1;

  return *this;
}

Quaternion& Quaternion::rotation(const Vector<3>& axis, CoordType angle)
{
  CoordType axis_mag = axis.mag();
  CoordType half_angle = angle / 2;

  if (axis_mag < WFMATH_EPSILON) {
    m_valid = false;
    return *this;
  }

  m_w = F_COS(half_angle);
  m_vec = axis * (F_SIN(half_angle) / axis_mag);

  m_valid = axis.isValid();
  m_age = 1;

  return *this;
}

Quaternion& Quaternion::rotation(const Vector<3>& axis)
{
  CoordType axis_mag = axis.mag();
  CoordType half_angle = axis_mag / 2;

  if (axis_mag < WFMATH_EPSILON) {
    m_valid = false;
    return *this;
  }

  m_w = F_COS(half_angle);
  m_vec = axis * (F_SIN(half_angle) / axis_mag);

  m_valid = axis.isValid();
  m_age = 1;

  return *this;
}

Quaternion& Quaternion::rotation(const Vector<3>& from, const Vector<3>& to)
{
  CoordType mag_prod = F_SQRT(from.sqrMag() * to.sqrMag());
  CoordType ctheta_plus_1 = Dot(from, to) / mag_prod + 1;

  if (mag_prod < WFMATH_EPSILON) {
    m_valid = false;
    return *this;
  }

  // antiparallel vectors
  if(ctheta_plus_1 < WFMATH_EPSILON) // same check as used in the RotMatrix function
    throw ColinearVectors<3>(from, to);

  // cosine of half the angle
  m_w = F_SQRT(ctheta_plus_1 / 2.f);

  // vector in direction of axis, magnitude of cross product is proportional to
  // the sin of the angle, divide to make the magnitude the sin of half the angle,
  // sin(x) = 2sin(x/2)cos(x/2), so sin(x/2) = sin(x)/(2cos(x/2))
  m_vec = Cross(from, to) / (2 * mag_prod * m_w);

  m_valid = from.isValid() && to.isValid();
  m_age = 1;

  return *this;
}

void Quaternion::normalize()
{
  // Assume that we're not too far off, and compute the norm
  // only to linear order in the difference from 1.
  // If q.sqrMag() = 1 + x, q.mag() = 1 + x/2 = (q.SqrMag() + 1)/2
  // to linear order.
  CoordType norm = (m_w * m_w + m_vec.sqrMag() + 1)/2;
  m_w /= norm;
  m_vec /= norm;
  m_age = 1;
}
