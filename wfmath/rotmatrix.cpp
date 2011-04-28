// rotmatrix.cpp (RotMatrix<> implementation)
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rotmatrix_funcs.h"
#include "quaternion.h"
#include "floatmath.h"

using namespace WFMath;

static CoordType _MatrixDeterminantImpl(const int size, CoordType* m);

template<> RotMatrix<3>& RotMatrix<3>::fromQuaternion(const Quaternion& q,
						      const bool not_flip)
{
  CoordType xx, yy, zz, xy, xz, yz;
  const Vector<3> &vref = q.vector();

  xx = vref[0] * vref[0];
  xy = vref[0] * vref[1];
  xz = vref[0] * vref[2];
  yy = vref[1] * vref[1];
  yz = vref[1] * vref[2];
  zz = vref[2] * vref[2];

  Vector<3> wvec = vref * q.scalar();

  m_elem[0][0] = 1 - 2 * (yy + zz);
  m_elem[1][1] = 1 - 2 * (xx + zz);
  m_elem[2][2] = 1 - 2 * (xx + yy);

  m_elem[0][1] = 2 * (xy + wvec[2]);
  m_elem[0][2] = 2 * (xz - wvec[1]);
  m_elem[1][0] = 2 * (xy - wvec[2]);
  m_elem[1][2] = 2 * (yz + wvec[0]);
  m_elem[2][0] = 2 * (xz + wvec[1]);
  m_elem[2][1] = 2 * (yz - wvec[0]);

  m_flip = !not_flip;
  m_age = q.age();
  if(!not_flip)
    *this = Prod(*this, RotMatrix<3>().mirror(0));

  m_valid = true;
  return *this;
}

template<> RotMatrix<3>& RotMatrix<3>::rotate(const Quaternion& q)
{
  Vector<3> vec;
  vec.setValid();
  m_valid = m_valid && q.isValid();
  m_age += q.age();

  // rotate both sides by q

  for(int vec_num = 0; vec_num < 3; ++vec_num) {
    for(int elem_num = 0; elem_num < 3; ++elem_num)
      vec[elem_num] = m_elem[vec_num][elem_num];
    vec.rotate(q);
    for(int elem_num = 0; elem_num < 3; ++elem_num)
      m_elem[vec_num][elem_num] = vec[elem_num];
  }

  checkNormalization();

  return *this;
}

template<>
RotMatrix<3>& WFMath::RotMatrix<3>::rotation (const Vector<3>& axis,
					      CoordType theta)
{
  CoordType max = 0;
  int main_comp = -1;

  for(int i = 0; i < 3; ++i) {
    CoordType val = std::fabs(axis[i]);
    if(val > max) {
      max = val;
      main_comp = i;
    }
  }

  assert("Must pass a nonzero length vector as axis to avoid this" && main_comp != -1);

  Vector<3> tmp, v1, v2;

  int new_comp = main_comp ? main_comp - 1 : 2; // Not parallel to axis
  for(int i = 0; i < 3; ++i)
    tmp[i] = (CoordType) ((i == new_comp) ? 1 : 0);

  v1 = Cross(axis, tmp); // 3D specific part
  v2 = Cross(axis, v1);

  return rotation(v1, v2, theta);
}

template<>
RotMatrix<3>& WFMath::RotMatrix<3>::rotation (const Vector<3>& axis)
{
  CoordType max = 0;
  int main_comp = -1;
  CoordType angle = axis.mag();

  if(angle == 0) {
    return identity();
  }

  for(int i = 0; i < 3; ++i) {
    CoordType val = std::fabs(axis[i]);
    if(val > max) {
      max = val;
      main_comp = i;
    }
  }

  assert("Can't happen with nonzero angle" && main_comp != -1);

  Vector<3> tmp, v1, v2;

  int new_comp = main_comp ? main_comp - 1 : 2; // Not parallel to axis
  for(int i = 0; i < 3; ++i)
    tmp[i] = (CoordType) ((i == new_comp) ? 1 : 0);

  v1 = Cross(axis, tmp); // 3D specific part
  v2 = Cross(axis, v1);

  return rotation(v1, v2, angle);
}

bool WFMath::_MatrixSetValsImpl(const int size, CoordType* vals, bool& flip,
				CoordType* buf1, CoordType* buf2, double precision)
{
  precision = fabs(precision);

  if(precision >= .9) // Can get an infinite loop for precision == 1
    return false;

  // Check that vals form an orthogonal matrix, also increase their
  // precision to WFMATH_EPSILON

  while(true) {
    double try_prec = 0;

    for(int i = 0; i < size; ++i) {
      for(int j = 0; j < size; ++j) {
        CoordType ans = 0;
        for(int k = 0; k < size; ++k) {
          ans += vals[i*size+k] * vals[j*size+k];
        }

        if(i == j) // Subtract identity matrix
          --ans;
        ans = std::fabs(ans);
        if(ans >= try_prec)
          try_prec = ans;
      }
    }

    if(try_prec > precision)
      return false;

    if(try_prec <= WFMATH_EPSILON)
      break;

    // Precision needs improvement, use linear approximation scheme.

    // This scheme takes the original matrix (call it A)
    // and subtracts another matrix (delta), where
    //
    // delta = (A - (A^T)^-1) / 2
    //
    // This is correct, up to errors of order delta^2,
    // if you assume you can choose a delta such that
    // A^T * delta is symmetric (delta is underdetermined
    // by the linear approximation scheme)

    // This procedure will not increase the precision of
    // the parameters which determine the matrix (i.e. the
    // Euler angles), but it will increase the precision
    // to which the matrix satisfies the condition
    // A^T * A == (identity matrix).

    // The symmetry condition on A^T * delta represents an
    // arbitrary condition which will determine the higher
    // precision components of the Euler angles. This
    // makes the problem of increasing the precision of
    // the matrix well determined, and solvable by this
    // perturbative method.

    for(int i = 0; i < size; ++i) {
      for(int j = 0; j < size; ++j) {
        buf1[i*size+j] = vals[j*size+i];
        buf2[i*size+j] = (CoordType) ((i == j) ? 1 : 0);
      }
    }

    bool ans = _MatrixInverseImpl(size, buf1, buf2);

    if(ans == false) // Degenerate matrix, something badly wrong
      return false;

    for(int i = 0; i < size; ++i) {
      for(int j = 0; j < size; ++j) {
        CoordType& elem = vals[i*size+j];
        elem += buf2[i*size+j];
        elem /= 2;
      }
    }

    // The above scheme should approx. square the precision.
    // That is, try_prec -> try_prec * try_prec * (fudge factor)
    // in the next iteration.
  }

  // The determinant is either 1 or -1, depending on the parity.
  // Use that to calculate flip.

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      buf1[i*size+j] = vals[i*size+j];

  flip = _MatrixDeterminantImpl(size, buf1) < 0;

  return true;
}

static CoordType _MatrixDeterminantImpl(const int size, CoordType* m)
{
  // First, construct an upper triangular matrix with the
  // same determinant as the original matrix. Then just
  // multiply the diagonal terms to get the determinant.

  for(int i = 0; i < size - 1; ++i) {
    CoordType minval = 0;
    for(int j = 0; j < size; ++j)
      minval += m[j*size+i] * m[j*size+i];
    minval /= WFMATH_MAX;
    if(minval < WFMATH_MIN)
      minval = WFMATH_MIN;
    if(m[i*size+i] * m[i*size+i] < minval) { // Find a row with nonzero element
      int j;
      for(j = i + 1; j < size; ++j)
        if(m[j*size+i] * m[j*size+i] >= minval)
          break;
      if(j == size) // No nonzero element found, degenerate matrix, det == 0
        return 0;
      m[i*size+i] = m[j*size+i];
      for(int k = i + 1; k < size; ++k) // For k < i, m[j*size+k] == 0
        m[i*size+k] += m[j*size+k];
    }
    for(int j = i + 1; j < size; ++j) {
      CoordType factor = m[j*size+i] / m[i*size+i];
      // We know factor isn't bigger than about sqrt(WFMATH_MAX), due to
      // the comparison with minval done above.
      m[j*size+i] = 0;
      if(factor != 0) {
        for(int k = i + 1; k < size; ++k) // For k < i, m[k*size+j] == 0
	  m[j*size+k] -= m[i*size+k] * factor;
      }
    }
  }

  CoordType out = 1;

  for(int i = 0; i < size; ++i)
    out *= m[i*size+i];

  return out;
}

bool WFMath::_MatrixInverseImpl(const int size, CoordType* in, CoordType* out)
{
  // Invert using row operations. First, make m upper triangular,
  // with 1's on the diagonal

  for(int i = 0; i < size; ++i) {

    // Make sure in[i*size+i] is nonzero
    CoordType minval = 0;
    for(int j = 0; j < size; ++j)
      minval += in[j*size+i] * in[j*size+i];
    minval /= WFMATH_MAX;
    if(minval < WFMATH_MIN)
      minval = WFMATH_MIN;
    if(in[i*size+i] * in[i*size+i] < minval) { // Find a nonzero element
      int j;
      for(j = i + 1;  j < size; ++j)
        if(in[j*size+i] * in[j*size+i] >= minval)
          break;
      if(j == size) // degenerate matrix
        return false;
      for(int k = 0; k < size; ++k) {
        out[i*size+k] += out[j*size+k];
        in[i*size+k] += in[j*size+k];
      }
    }
    // We now know in[i*size+i] / in[j*size+i] >= sqrt(WFMATH_MIN) for any j

    // Normalize the row, so in[i*size+i] == 1
    CoordType tmp = in[i*size+i];
    in[i*size+i] = 1;
    for(int j = 0; j < size; ++j) {
      out[i*size+j] /= tmp;
      if(j > i) // in[i*size+j] == 0 for j < i
        in[i*size+j] /= tmp;
    }

    // Do row subtraction to make in[j*size+i] zero for j > i
    for(int j = i + 1; j < size; ++j) {
      CoordType tmp = in[j*size+i];
      in[j*size+i] = 0;
      if(tmp != 0) {
        for(int k = 0; k < size; ++k) {
          out[j*size+k] -= out[i*size+k] * tmp;
          in[j*size+k] -= in[i*size+k] * tmp;
        }
      }
    }
  }

  // Now perform row operations on "out" which would make "m"
  // into the identity matrix

  for(int i = size - 1; i >= 1; --i) {
    for(int j = i - 1; j >= 0; --j) {
      CoordType tmp = in[j*size+i];
      if(tmp != 0)
        for(int k = 0; k < size; ++k)
          out[j*size+k] -= out[i*size+k] * tmp;
          // Don't bother modifying in[j*size+k], we never use it again.
    }
  }

  return true;
}

namespace WFMath {

template <>
RotMatrix<3>::RotMatrix(const Quaternion& q, const bool not_flip)
{
  fromQuaternion(q, not_flip);
}

template class RotMatrix<2>;
template class RotMatrix<3>;

template RotMatrix<2> operator*<2>(RotMatrix<2> const&, RotMatrix<2> const&);
template RotMatrix<3> operator*<3>(RotMatrix<3> const&, RotMatrix<3> const&);

template RotMatrix<2> ProdInv<2>(RotMatrix<2> const&, RotMatrix<2> const&);
template RotMatrix<3> ProdInv<3>(RotMatrix<3> const&, RotMatrix<3> const&);

template Vector<2> operator*<2>(Vector<2> const&, RotMatrix<2> const&);
template Vector<3> operator*<3>(Vector<3> const&, RotMatrix<3> const&);

template Vector<2> operator*<2>(RotMatrix<2> const&, Vector<2> const&);
template Vector<3> operator*<3>(RotMatrix<3> const&, Vector<3> const&);

template Vector<2> ProdInv<2>(Vector<2> const&, RotMatrix<2> const&);
template Vector<3> ProdInv<3>(Vector<3> const&, RotMatrix<3> const&);

template Vector<3> Prod<3>(Vector<3> const&, RotMatrix<3> const&);
template Vector<2> Prod<2>(Vector<2> const&, RotMatrix<2> const&);

template RotMatrix<3> Prod<3>(RotMatrix<3> const&, RotMatrix<3> const&);
template RotMatrix<2> Prod<2>(RotMatrix<2> const&, RotMatrix<2> const&);

}
