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

#include <wfmath/vector.h>
#include <wfmath/matrix.h>
#include <wfmath/error.h>
#include <wfmath/const.h>

namespace WF { namespace Math {

template<const int dim>
inline RotMatrix<dim>::RotMatrix(const RotMatrix<dim>& m) : m_flip(m.m_flip)
{
  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] = m.m_elem[i][j];
}

template<const int dim>
RotMatrix<dim>& RotMatrix<dim>::operator=(const RotMatrix<dim>& m)
{
  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] = m.m_elem[i][j];

  m_flip = m.m_flip;

  return *this;
}

template<const int dim>
bool RotMatrix<dim>::isEqualTo(const RotMatrix<dim>& rhs, double tolerance) const
{
  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      if(!IsFloatEqual(m_elem[i][j], rhs.m_elem[i][j], tolerance))
        return false;

  // Don't need to test m_flip, it's determined by the values of m_elem

  return true;
}

template<const int dim>
bool RotMatrix<dim>::operator< (const RotMatrix<dim>& m) const
{
  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      if(!IsFloatEqual(m_elem[i][j], m.m_elem[i][j]))
        return m_elem[i][j] < m.m_elem[i][j];

  // Don't need to test m_flip, it's determined by the values of m_elem

  return false;
}

template<const int dim> // m1 * m2
RotMatrix<dim> Prod(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2)
{
  RotMatrix<dim> out;

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      out.m_elem[i][j] = 0;
      CoordType max_val = 0;
      for(int k = 0; k < dim; ++k) {
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

  out.m_flip = (m1.m_flip != m2.m_flip); // XOR

  return out;
}

template<const int dim> // m1 * m2^-1
RotMatrix<dim> ProdInv(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2)
{
  RotMatrix<dim> out;

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      out.m_elem[i][j] = 0;
      CoordType max_val = 0;
      for(int k = 0; k < dim; ++k) {
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

  out.m_flip = (m1.m_flip != m2.m_flip); // XOR

  return out;
}

template<const int dim> // m1^-1 * m2
RotMatrix<dim> InvProd(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2)
{
  RotMatrix<dim> out;

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      out.m_elem[i][j] = 0;
      CoordType max_val = 0;
      for(int k = 0; k < dim; ++k) {
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

  out.m_flip = (m1.m_flip != m2.m_flip); // XOR

  return out;
}

template<const int dim> // m1^-1 * m2^-1
RotMatrix<dim> InvProdInv(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2)
{
  RotMatrix<dim> out;

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      out.m_elem[i][j] = 0;
      CoordType max_val = 0;
      for(int k = 0; k < dim; ++k) {
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

  out.m_flip = (m1.m_flip != m2.m_flip); // XOR

  return out;
}

template<const int dim> // m * v
Vector<dim> Prod(const RotMatrix<dim>& m, const Vector<dim>& v)
{
  Vector<dim> out;

  for(int i = 0; i < dim; ++i) {
    out.m_elem[i] = 0;
    CoordType max_val = 0;
    for(int j = 0; j < dim; ++j) {
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

template<const int dim> // m^-1 * v
Vector<dim> InvProd(const RotMatrix<dim>& m, const Vector<dim>& v)
{
  Vector<dim> out;

  for(int i = 0; i < dim; ++i) {
    out.m_elem[i] = 0;
    CoordType max_val = 0;
    for(int j = 0; j < dim; ++j) {
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

template<const int dim>
bool RotMatrix<dim>::setVals(const CoordType vals[dim][dim], double precision)
{
  // Scratch space for the backend
  CoordType scratch_vals[dim*dim];

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      scratch_vals[i*dim+j] = vals[i][j];

  return _setVals(scratch_vals, precision);
}

template<const int dim>
bool RotMatrix<dim>::setVals(const CoordType vals[dim*dim], double precision)
{
  // Scratch space for the backend
  CoordType scratch_vals[dim*dim];

  for(int i = 0; i < dim*dim; ++i)
      scratch_vals[i] = vals[i];

  return _setVals(scratch_vals, precision);
}

bool _MatrixSetValsImpl(const int size, CoordType* vals, bool& flip,
			CoordType* buf1, CoordType* buf2, double precision);

template<const int dim>
bool RotMatrix<dim>::_setVals(CoordType *vals, double precision)
{
  // Cheaper to allocate space on the stack here than with
  // new in _MatrixSetValsImpl()
  CoordType buf1[dim*dim], buf2[dim*dim];
  bool flip;

  if(!_MatrixSetValsImpl(dim, vals, flip, buf1, buf2, precision))
    return false;

  // Do the assignment

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] = vals[i*dim+j];

  m_flip = flip;

  return true;
}

template<const int dim>
inline Vector<dim> RotMatrix<dim>::row(const int i) const
{
  Vector<dim> out;

  for(int j = 0; j < dim; ++j)
    out[j] = m_elem[i][j];

  return out;
}

template<const int dim>
inline Vector<dim> RotMatrix<dim>::column(const int i) const
{
  Vector<dim> out;

  for(int j = 0; j < dim; ++j)
    out[j] = m_elem[j][i];

  return out;
}

template<const int dim>
inline RotMatrix<dim> RotMatrix<dim>::inverse() const
{
  RotMatrix<dim> m;

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m.m_elem[j][i] = m_elem[i][j];

  return m;
}

template<const int dim>
inline RotMatrix<dim>& RotMatrix<dim>::identity()
{
  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] = (i == j) ? 1 : 0;

  m_flip = false;

  return *this;
}

template<const int dim>
inline CoordType RotMatrix<dim>::trace() const
{
  CoordType out = 0;

  for(int i = 0; i < dim; ++i)
    out = FloatAdd(out, m_elem[i][i]);

  return out;
}

template<const int dim>
RotMatrix<dim>& RotMatrix<dim>::fromEuler(const CoordType angles[nParams], bool flip)
{
  int ang_num = 0;

  m_flip = flip;

  if(flip)
    mirror(0);
  else
    identity();

  for(int i = dim - 1; i > 0; --i)
    for(int j = 0; j < i; ++j)
      *this = Prod(RotMatrix<dim>().rotation(j, j + 1, angles[ang_num++]), *this);

  assert(ang_num == nParams);

  return *this;
}

// TODO generalize if possible at some point

//template<const int dim>
//bool RotMatrix<dim>::toEuler(CoordType angles[nParams]) const
//{
//
//}

// Only have this for special cases
template<> inline bool RotMatrix<2>::toEuler(CoordType angles[1]) const
	{angles[0] = atan2(m_elem[1][1], m_elem[0][1]); return m_flip;}
template<> bool RotMatrix<3>::toEuler(CoordType angles[3]) const;

template<const int dim>
RotMatrix<dim>& RotMatrix<dim>::rotation (const int i, const int j,
					  const CoordType& theta)
{
  assert(i >= 0 && i < dim && j >= 0 && j < dim && i != j);

  CoordType ctheta = cos(theta), stheta = sin(theta);

  for(int k = 0; k < dim; ++k) {
    for(int l = 0; l < dim; ++l) {
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

  m_flip = false;

  return *this;
}

template<const int dim>
RotMatrix<dim>& RotMatrix<dim>::rotation (const Vector<dim>& v1,
					  const Vector<dim>& v2,
					  const CoordType& theta)
{
  CoordType v1_sqr_mag = v1.sqrMag();

  assert(v1_sqr_mag > 0);

  // Get an in-plane vector which is perpendicular to v1

  Vector<dim> vperp = v2 - v1 * Dot(v1, v2) / v1_sqr_mag;
  CoordType vperp_sqr_mag = vperp.sqrMag();

  if((vperp_sqr_mag / v1_sqr_mag) < (dim * WFMATH_EPSILON * WFMATH_EPSILON)) {
    assert(v2.sqrMag() > 0);
    // The original vectors were parallel
    throw ColinearVectors<dim>(v1, v2);
  }

  // If we were rotating a vector vin, the answer would be
  // vin + Dot(v1, vin) * (v1 (cos(theta) - 1)/ v1_sqr_mag
  // + vperp * sin(theta) / sqrt(v1_sqr_mag * vperp_sqr_mag))
  // + Dot(vperp, vin) * (a similar term). From this, we find
  // the matrix components.

  CoordType mag_prod = sqrt(v1_sqr_mag * vperp_sqr_mag);
  CoordType ctheta = cos(theta), stheta = sin(theta);

  identity(); // Initialize to identity matrix

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] += FloatAdd((ctheta - 1) * FloatAdd(v1[i] * v1[j] / v1_sqr_mag,
		      vperp[i] * vperp[j] / vperp_sqr_mag), stheta
		      * FloatSubtract(vperp[i] * v1[j], v1[i] * vperp[j]) / mag_prod);

  m_flip = false;

  return *this;
}

template<> RotMatrix<3>& RotMatrix<3>::rotation (const Vector<3>& axis,
						 const CoordType& theta);

template<const int dim>
RotMatrix<dim>& RotMatrix<dim>::mirror	(const Vector<dim>& v)
{
  // Get a flip by subtracting twice the projection operator in the
  // direction of the vector. A projection operator is idempotent (P*P == P),
  // and symmetric, so I - 2P is an orthogonal matrix.
  //
  // (I - 2P) * (I - 2P)^T == (I - 2P)^2 == I - 4P + 4P^2 == I

  CoordType sqr_mag = v.sqrMag();

  assert(sqr_mag > 0);

  // off diagonal
  for(int i = 0; i < dim - 1; ++i)
    for(int j = i + 1; j < dim; ++j)
      m_elem[i][j] = m_elem[j][i] = -2 * v[i] * v[j] / sqr_mag;

  // diagonal
  for(int i = 0; i < dim; ++i)
    m_elem[i][i] = FloatSubtract(1, 2 * v[i] * v[i] / sqr_mag);

  m_flip = true;

  return *this;
}

template<const int dim>
RotMatrix<dim>& RotMatrix<dim>::mirror()
{
  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] = (i == j) ? -1 : 0;

  m_flip = dim%2;

  return *this;
}

}} // namespace WF::Math

#endif // WFMATH_MATRIX_FUNCS_H
