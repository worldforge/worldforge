// -*-C++-*-
// matrix.cpp (Matrix<> implementation)
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

#include "matrix_funcs.h"
#include "vector_funcs.h"
#include "const.h"
#include <float.h>

namespace WF { namespace Math {

template<> void RotMatrix<3>::toEuler(double angles[3]) const
{
  // There's a 2:1 map from Euler angles to matrices. Flipping the
  // sign of the middle angle and adding pi to each of the others produces
  // the same matrix. Therefore, this function will never return a value greater
  // than pi for the middle angle.

  double sin_sqr_beta = m_elem[0][2] * m_elem[0][2] + m_elem[1][2] * m_elem[1][2];

  if(sin_sqr_beta > WFMATH_EPSILON) {
    angles[0] = atan2(m_elem[2][1], m_elem[2][0]);
    angles[1] = acos(m_elem[2][2]);
    angles[2] = -atan2(m_elem[1][2], m_elem[0][2]);
  }
  else {
    angles[1] = -atan2(m_elem[0][0], m_elem[0][1]);
    angles[2] = (m_elem[2][2] > 0) ? 0 : WFMATH_CONST_PI;
    angles[3] = 0;
  }
}

template<>
const RotMatrix<3>& RotMatrix<3>::rotation (const Vector<3>& axis, const double& theta)
{
  double max = 0;
  int main_comp = -1;

  for(int i = 0; i < 3; ++i) {
    double val = fabs(axis[i]);
    if(val > max) {
      max = val;
      main_comp = i;
    }
  }

  if(main_comp == -1) // zero length vector
    throw BadRotationAxis(axis);

  Vector<3> tmp, v1, v2;

  tmp.zero();
  tmp[(main_comp + 1)%3] = 1; // Not parallel to axis

  v1 = Cross(axis, tmp); // 3D specific part
  v2 = Cross(axis, v1);

  return rotation(v1, v2, theta);
}

}} // namespace WF::Math
