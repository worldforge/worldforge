// -*-C++-*-
// matrix_funcs.h (Matrix<> template functions)
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

#ifndef WFMATH_MATRIX_FUNCS_H
#define WFMATH_MATRIX_FUNCS_H

#include <wfmath/matrix.h>
#include <wfmath/error.h>
#include <wfmath/const.h>
#include <wfmath/string_funcs.h>

namespace WF { namespace Math {

template<const int size>
inline RotMatrix<size>::RotMatrix(const RotMatrix<size>& m)
{
  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      m_elem[i][j] = m.m_elem[i][j];
}

template<const int size>
std::string RotMatrix<size>::toString() const
{
  CoordType d[size*size];

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      d[i*size+j] = m_elem[i][j];

  return _StringFromCoordArray(d, size, size);
}

template<const int size>
bool RotMatrix<size>::fromString(const std::string& s)
{
  CoordType d[size*size], buf1[size*size], buf2[size*size];

  if(!_StringToCoordArray(s, d, size, size))
    return false;

  if(_MatrixSetValsImpl(size, d, buf1, buf2, 1e-6) == false)
    return false; // Standard precision for string representation

  // Do the assignment

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      m_elem[i][j] = d[i*size+j];

  return true;
}

template<const int size>
RotMatrix<size>& RotMatrix<size>::operator=(const RotMatrix<size>& m)
{
  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      m_elem[i][j] = m.m_elem[i][j];

  return *this;
}

template<const int size>
bool RotMatrix<size>::isEqualTo(const RotMatrix<size>& rhs, double tolerance) const
{
  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      if(!IsFloatEqual(m_elem[i][j], rhs.m_elem[i][j], tolerance))
        return false;

  return true;
}

template<const int size>
bool RotMatrix<size>::operator< (const RotMatrix<size>& m) const
{
  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
      if(m_elem[i][j] < m.m_elem[i][j])
        return true;
      if(m_elem[i][j] > m.m_elem[i][j])
        return false;
    }
  }

  return false;
}

template<const int size> // m1 * m2
RotMatrix<size> Prod(const RotMatrix<size>& m1, const RotMatrix<size>& m2)
{
  RotMatrix<size> out;

  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
      out.m_elem[i][j] = 0;
      CoordType max_val = 0;
      for(int k = 0; k < size; ++k) {
        CoordType val = m1.m_elem[i][k] * m2.m_elem[k][j];
        out.m_elem[i][j] += val;
        CoordType aval = fabs(val);
        if(aval > max_val)
          max_val = aval;
      }
      if(max_val > 0 && fabs(out.m_elem[i][j]/max_val) < WFMATH_EPSILON)
        out.m_elem[i][j] = 0;
    }
  }

  return out;
}

template<const int size> // m1 * m2^-1
RotMatrix<size> ProdInv(const RotMatrix<size>& m1, const RotMatrix<size>& m2)
{
  RotMatrix<size> out;

  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
      out.m_elem[i][j] = 0;
      CoordType max_val = 0;
      for(int k = 0; k < size; ++k) {
        CoordType val = m1.m_elem[i][k] * m2.m_elem[j][k];
        out.m_elem[i][j] += val;
        CoordType aval = fabs(val);
        if(aval > max_val)
          max_val = aval;
      }
      if(max_val > 0 && fabs(out.m_elem[i][j]/max_val) < WFMATH_EPSILON)
        out.m_elem[i][j] = 0;
    }
  }

  return out;
}

template<const int size> // m1^-1 * m2
RotMatrix<size> InvProd(const RotMatrix<size>& m1, const RotMatrix<size>& m2)
{
  RotMatrix<size> out;

  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
      out.m_elem[i][j] = 0;
      CoordType max_val = 0;
      for(int k = 0; k < size; ++k) {
        CoordType val = m1.m_elem[k][i] * m2.m_elem[k][j];
        out.m_elem[i][j] += val;
        CoordType aval = fabs(val);
        if(aval > max_val)
          max_val = aval;
      }
      if(max_val > 0 && fabs(out.m_elem[i][j]/max_val) < WFMATH_EPSILON)
        out.m_elem[i][j] = 0;
    }
  }

  return out;
}

template<const int size> // m1^-1 * m2^-1
RotMatrix<size> InvProdInv(const RotMatrix<size>& m1, const RotMatrix<size>& m2)
{
  RotMatrix<size> out;

  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
      out.m_elem[i][j] = 0;
      CoordType max_val = 0;
      for(int k = 0; k < size; ++k) {
        CoordType val = m1.m_elem[k][i] * m2.m_elem[j][k];
        out.m_elem[i][j] += val;
        CoordType aval = fabs(val);
        if(aval > max_val)
          max_val = aval;
      }
      if(max_val > 0 && fabs(out.m_elem[i][j]/max_val) < WFMATH_EPSILON)
        out.m_elem[i][j] = 0;
    }
  }

  return out;
}

template<const int size> // m * v
Vector<size> Prod(const RotMatrix<size>& m, const Vector<size>& v)
{
  Vector<size> out;

  for(int i = 0; i < size; ++i) {
    out.m_elem[i] = 0;
    CoordType max_val = 0;
    for(int j = 0; j < size; ++j) {
      CoordType val = m.m_elem[i][j] * v.m_elem[j];
      out.m_elem[i] += val;
      CoordType aval = fabs(val);
      if(aval > max_val)
        max_val = aval;
    }
    if(max_val > 0 && fabs(out.m_elem[i]/max_val) < WFMATH_EPSILON)
      out.m_elem[i] = 0;
  }

  return out;
}

template<const int size> // m^-1 * v
Vector<size> InvProd(const RotMatrix<size>& m, const Vector<size>& v)
{
  Vector<size> out;

  for(int i = 0; i < size; ++i) {
    out.m_elem[i] = 0;
    CoordType max_val = 0;
    for(int j = 0; j < size; ++j) {
      CoordType val = m.m_elem[j][i] * v.m_elem[j];
      out.m_elem[i] += val;
      CoordType aval = fabs(val);
      if(aval > max_val)
        max_val = aval;
    }
    if(max_val > 0 && fabs(out.m_elem[i]/max_val) < WFMATH_EPSILON)
      out.m_elem[i] = 0;
  }

  return out;
}

bool _MatrixSetValsImpl(const int size, CoordType* vals, CoordType* buf1,
			CoordType* buf2, double precision);

template<const int size>
bool RotMatrix<size>::setVals(const CoordType vals[size][size], double precision)
{
  // Scratch space for the backend
  CoordType scratch_vals[size*size], buf1[size*size], buf2[size*size];

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      scratch_vals[i*size+j] = vals[i][j];

  if(_MatrixSetValsImpl(size, scratch_vals, buf1, buf2, precision) == false)
    return false;

  // Do the assignment

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      m_elem[i][j] = scratch_vals[i*size+j];

  return true;
}

template<const int size>
bool RotMatrix<size>::setVals(const CoordType vals[size*size], double precision)
{
  // Scratch space for the backend
  CoordType scratch_vals[size*size], buf1[size*size], buf2[size*size];

  for(int i = 0; i < size*size; ++i)
      scratch_vals[i] = vals[i];

  if(_MatrixSetValsImpl(size, scratch_vals, buf1, buf2, precision) == false)
    return false;

  // Do the assignment

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      m_elem[i][j] = scratch_vals[i*size+j];

  return true;
}

template<const int size>
inline Vector<size> RotMatrix<size>::row(const int i) const
{
  Vector<size> out;

  for(int j = 0; j < size; ++j)
    out[j] = m_elem[i][j];

  return out;
}

template<const int size>
inline Vector<size> RotMatrix<size>::column(const int i) const
{
  Vector<size> out;

  for(int j = 0; j < size; ++j)
    out[j] = m_elem[j][i];

  return out;
}

template<const int size>
inline RotMatrix<size> RotMatrix<size>::inverse() const
{
  RotMatrix<size> m;

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      m.m_elem[j][i] = m_elem[i][j];

  return m;
}

template<const int size>
inline RotMatrix<size>& RotMatrix<size>::identity()
{
  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      m_elem[i][j] = (i == j) ? 1 : 0;

  return *this;
}

template<const int size>
inline double RotMatrix<size>::trace() const
{
  double out = 0;

  for(int i = 0; i < size; ++i)
    out = FloatAdd(out, m_elem[i][i]);

  return out;
}

template<const int size>
RotMatrix<size>& RotMatrix<size>::fromEuler(const CoordType angles[nParams])
{
  int ang_num = 0;

  identity();

  for(int i = size - 1; i > 0; --i)
    for(int j = 0; j < i; ++j)
      *this = Prod(RotMatrix<size>().rotation(j, j + 1, angles[ang_num++]), *this);

  if(ang_num != nParams)
    assert(false);

  return *this;
}

// TODO generalize if possible at some point

//template<const int size>
//void RotMatrix<size>::toEuler(CoordType angles[nParams]) const
//{
//
//}

// Only have this for special cases
template<> inline void RotMatrix<2>::toEuler(CoordType angles[1]) const
	{angles[0] = atan2(m_elem[0][0], m_elem[1][0]);}
template<> void RotMatrix<3>::toEuler(CoordType angles[3]) const;

template<const int size>
const RotMatrix<size>& RotMatrix<size>::rotation (const int i, const int j,
						  const double& theta)
{
  if(i < 0 || i > size || j < 0 || j > size || i == j)
    throw BadRotationAxisNum(i, j);

  double ctheta = cos(theta), stheta = sin(theta);

  for(int k = 0; k < size; ++k) {
    for(int l = 0; l < size; ++l) {
      if(k == l) {
        if(k == i || k == j)
          m_elem[k][l] = ctheta;
        else
          m_elem[k][l] = 1;
      }
      else {
        if(k == i && l == j)
          m_elem[k][l] = -stheta;
        else if(k == j && l == i)
          m_elem[k][l] = stheta;
        else
          m_elem[k][l] = 0;
      }
    }
  }

  return *this;
}

template<const int size>
const RotMatrix<size>& RotMatrix<size>::rotation (const Vector<size>& v1,
						  const Vector<size>& v2,
						  const double& theta)
{
  double v1_sqr_mag = v1.sqrMag();

  if(v1_sqr_mag == 0)
    throw BadRotationPlane<size>(v1, v2);

  // Get an in-plane vector which is perpendicular to v1

  Vector<size> vperp = v2 - v1 * Dot(v1, v2) / v1_sqr_mag;
  double vperp_sqr_mag = vperp.sqrMag();

  if((vperp_sqr_mag / v1_sqr_mag) < (size * WFMATH_EPSILON * WFMATH_EPSILON))
    // The original vectors were parallel
    throw BadRotationPlane<size>(v1, v2);

  // If we were rotating a vector vin, the answer would be
  // vin + Dot(v1, vin) * (v1 (cos(theta) - 1)/ v1_sqr_mag
  // + vperp * sin(theta) / sqrt(v1_sqr_mag * vperp_sqr_mag))
  // + Dot(vperp, vin) * (a similar term). From this, we find
  // the matrix components.

  double mag_prod = sqrt(v1_sqr_mag * vperp_sqr_mag);
  double ctheta = cos(theta), stheta = sin(theta);

  identity(); // Initialize to identity matrix

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      m_elem[i][j] += FloatAdd((ctheta - 1) * FloatAdd(v1[i] * v1[j] / v1_sqr_mag,
		      vperp[i] * vperp[j] / vperp_sqr_mag), stheta
		      * FloatSubtract(vperp[i] * v1[j], v1[i] * vperp[j]) / mag_prod);

  return *this;
}

template<> const RotMatrix<3>& RotMatrix<3>::rotation (const Vector<3>& axis,
						       const double& theta);

}} // namespace WF::Math

#endif // WFMATH_MATRIX_FUNCS_H
