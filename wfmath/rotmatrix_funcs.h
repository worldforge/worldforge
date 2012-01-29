// rotmatrix_funcs.h (RotMatrix<> template functions)
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

#ifndef WFMATH_ROTMATRIX_FUNCS_H
#define WFMATH_ROTMATRIX_FUNCS_H

#include <wfmath/rotmatrix.h>

#include <wfmath/vector.h>
#include <wfmath/error.h>
#include <wfmath/const.h>

#include <cmath>

#include <cassert>

namespace WFMath {

template<int dim>
inline RotMatrix<dim>::RotMatrix(const RotMatrix<dim>& m)
	: m_flip(m.m_flip), m_valid(m.m_valid), m_age(1)
{
  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] = m.m_elem[i][j];
}

template<int dim>
inline RotMatrix<dim>& RotMatrix<dim>::operator=(const RotMatrix<dim>& m)
{
  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] = m.m_elem[i][j];

  m_flip = m.m_flip;
  m_valid = m.m_valid;
  m_age = m.m_age;

  return *this;
}

template<int dim>
inline bool RotMatrix<dim>::isEqualTo(const RotMatrix<dim>& m, CoordType epsilon) const
{
  // Since the sum of the squares of the elements in any row or column add
  // up to 1, all the elements lie between -1 and 1, and each row has
  // at least one element whose magnitude is at least 1/sqrt(dim).
  // Therefore, we don't need to scale epsilon, as we did for
  // Vector<> and Point<>.

  assert(epsilon > 0);

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      if(fabs(m_elem[i][j] - m.m_elem[i][j]) > epsilon)
        return false;

  // Don't need to test m_flip, it's determined by the values of m_elem.

  assert("Generated values, must be equal if all components are equal" &&
         m_flip == m.m_flip);

  return true;
}

template<int dim> // m1 * m2
inline RotMatrix<dim> Prod(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2)
{
  RotMatrix<dim> out;

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      out.m_elem[i][j] = 0;
      for(int k = 0; k < dim; ++k) {
        out.m_elem[i][j] += m1.m_elem[i][k] * m2.m_elem[k][j];
      }
    }
  }

  out.m_flip = (m1.m_flip != m2.m_flip); // XOR
  out.m_valid = m1.m_valid && m2.m_valid;
  out.m_age = m1.m_age + m2.m_age;
  out.checkNormalization();

  return out;
}

template<int dim> // m1 * m2^-1
inline RotMatrix<dim> ProdInv(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2)
{
  RotMatrix<dim> out;

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      out.m_elem[i][j] = 0;
      for(int k = 0; k < dim; ++k) {
        out.m_elem[i][j] += m1.m_elem[i][k] * m2.m_elem[j][k];
      }
    }
  }

  out.m_flip = (m1.m_flip != m2.m_flip); // XOR
  out.m_valid = m1.m_valid && m2.m_valid;
  out.m_age = m1.m_age + m2.m_age;
  out.checkNormalization();

  return out;
}

template<int dim> // m1^-1 * m2
inline RotMatrix<dim> InvProd(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2)
{
  RotMatrix<dim> out;

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      out.m_elem[i][j] = 0;
      for(int k = 0; k < dim; ++k) {
        out.m_elem[i][j] += m1.m_elem[k][i] * m2.m_elem[k][j];
      }
    }
  }

  out.m_flip = (m1.m_flip != m2.m_flip); // XOR
  out.m_valid = m1.m_valid && m2.m_valid;
  out.m_age = m1.m_age + m2.m_age;
  out.checkNormalization();

  return out;
}

template<int dim> // m1^-1 * m2^-1
inline RotMatrix<dim> InvProdInv(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2)
{
  RotMatrix<dim> out;

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      out.m_elem[i][j] = 0;
      for(int k = 0; k < dim; ++k) {
        out.m_elem[i][j] += m1.m_elem[k][i] * m2.m_elem[j][k];
      }
    }
  }

  out.m_flip = (m1.m_flip != m2.m_flip); // XOR
  out.m_valid = m1.m_valid && m2.m_valid;
  out.m_age = m1.m_age + m2.m_age;
  out.checkNormalization();

  return out;
}

template<int dim> // m * v
inline Vector<dim> Prod(const RotMatrix<dim>& m, const Vector<dim>& v)
{
  Vector<dim> out;

  for(int i = 0; i < dim; ++i) {
    out.m_elem[i] = 0;
    for(int j = 0; j < dim; ++j) {
      out.m_elem[i] += m.m_elem[i][j] * v.m_elem[j];
    }
  }

  out.m_valid = m.m_valid && v.m_valid;

  return out;
}

template<int dim> // m^-1 * v
inline Vector<dim> InvProd(const RotMatrix<dim>& m, const Vector<dim>& v)
{
  Vector<dim> out;

  for(int i = 0; i < dim; ++i) {
    out.m_elem[i] = 0;
    for(int j = 0; j < dim; ++j) {
      out.m_elem[i] += m.m_elem[j][i] * v.m_elem[j];
    }
  }

  out.m_valid = m.m_valid && v.m_valid;

  return out;
}

template<int dim> // v * m
inline Vector<dim> Prod(const Vector<dim>& v, const RotMatrix<dim>& m)
{
  return InvProd(m, v); // Since transpose() and inverse() are the same
}

template<int dim> // v * m^-1
inline Vector<dim> ProdInv(const Vector<dim>& v, const RotMatrix<dim>& m)
{
  return Prod(m, v); // Since transpose() and inverse() are the same
}

template<int dim>
inline RotMatrix<dim> operator*(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2)
{
  return Prod(m1, m2);
}

template<int dim>
inline Vector<dim> operator*(const RotMatrix<dim>& m, const Vector<dim>& v)
{
  return Prod(m, v);
}

template<int dim>
inline Vector<dim> operator*(const Vector<dim>& v, const RotMatrix<dim>& m)
{
  return InvProd(m, v); // Since transpose() and inverse() are the same
}

template<int dim>
inline bool RotMatrix<dim>::setVals(const CoordType vals[dim][dim], double precision)
{
  // Scratch space for the backend
  CoordType scratch_vals[dim*dim];

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      scratch_vals[i*dim+j] = vals[i][j];

  return _setVals(scratch_vals, precision);
}

template<int dim>
inline bool RotMatrix<dim>::setVals(const CoordType vals[dim*dim], double precision)
{
  // Scratch space for the backend
  CoordType scratch_vals[dim*dim];

  for(int i = 0; i < dim*dim; ++i)
      scratch_vals[i] = vals[i];

  return _setVals(scratch_vals, precision);
}

bool _MatrixSetValsImpl(const int size, CoordType* vals, bool& flip,
			CoordType* buf1, CoordType* buf2, double precision);

template<int dim>
inline bool RotMatrix<dim>::_setVals(CoordType *vals, double precision)
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
  m_valid = true;
  m_age = 1;

  return true;
}

template<int dim>
inline Vector<dim> RotMatrix<dim>::row(const int i) const
{
  Vector<dim> out;

  for(int j = 0; j < dim; ++j)
    out[j] = m_elem[i][j];

  out.setValid(m_valid);

  return out;
}

template<int dim>
inline Vector<dim> RotMatrix<dim>::column(const int i) const
{
  Vector<dim> out;

  for(int j = 0; j < dim; ++j)
    out[j] = m_elem[j][i];

  out.setValid(m_valid);

  return out;
}

template<int dim>
inline RotMatrix<dim> RotMatrix<dim>::inverse() const
{
  RotMatrix<dim> m;

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m.m_elem[j][i] = m_elem[i][j];

  m.m_flip = m_flip;
  m.m_valid = m_valid;
  m.m_age = m_age + 1;

  return m;
}

template<int dim>
inline RotMatrix<dim>& RotMatrix<dim>::identity()
{
  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] = ((i == j) ? 1.0f : 0.0f);

  m_flip = false;
  m_valid = true;
  m_age = 0; // 1 and 0 are exact, no roundoff error

  return *this;
}

template<int dim>
inline CoordType RotMatrix<dim>::trace() const
{
  CoordType out = 0;

  for(int i = 0; i < dim; ++i)
    out += m_elem[i][i];

  return out;
}

template<int dim>
RotMatrix<dim>& RotMatrix<dim>::rotation (const int i, const int j,
					  CoordType theta)
{
  assert(i >= 0 && i < dim && j >= 0 && j < dim && i != j);

  CoordType ctheta = std::cos(theta), stheta = std::sin(theta);

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
          m_elem[k][l] = stheta;
        else if(k == j && l == i)
          m_elem[k][l] = -stheta;
        else
          m_elem[k][l] = 0;
      }
    }
  }

  m_flip = false;
  m_valid = true;
  m_age = 1;

  return *this;
}

template<int dim>
RotMatrix<dim>& RotMatrix<dim>::rotation (const Vector<dim>& v1,
					  const Vector<dim>& v2,
					  CoordType theta)
{
  CoordType v1_sqr_mag = v1.sqrMag();

  assert("need nonzero length vector" && v1_sqr_mag > 0);

  // Get an in-plane vector which is perpendicular to v1

  Vector<dim> vperp = v2 - v1 * Dot(v1, v2) / v1_sqr_mag;
  CoordType vperp_sqr_mag = vperp.sqrMag();

  if((vperp_sqr_mag / v1_sqr_mag) < (dim * WFMATH_EPSILON * WFMATH_EPSILON)) {
    assert("need nonzero length vector" && v2.sqrMag() > 0);
    // The original vectors were parallel
    throw ColinearVectors<dim>(v1, v2);
  }

  // If we were rotating a vector vin, the answer would be
  // vin + Dot(v1, vin) * (v1 (cos(theta) - 1)/ v1_sqr_mag
  // + vperp * sin(theta) / sqrt(v1_sqr_mag * vperp_sqr_mag))
  // + Dot(vperp, vin) * (a similar term). From this, we find
  // the matrix components.

  CoordType mag_prod = std::sqrt(v1_sqr_mag * vperp_sqr_mag);
  CoordType ctheta = std::cos(theta),
            stheta = std::sin(theta);

  identity(); // Initialize to identity matrix

  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] += ((ctheta - 1) * (v1[i] * v1[j] / v1_sqr_mag
		      + vperp[i] * vperp[j] / vperp_sqr_mag)
		      - stheta * (vperp[i] * v1[j] - v1[i] * vperp[j]) / mag_prod);

  m_flip = false;
  m_valid = true;
  m_age = 1;

  return *this;
}

template<int dim>
RotMatrix<dim>& RotMatrix<dim>::rotation(const Vector<dim>& from,
					 const Vector<dim>& to)
{
  // This is sort of similar to the above, with the rotation angle determined
  // by the angle between the vectors

  CoordType fromSqrMag = from.sqrMag();
  assert("need nonzero length vector" && fromSqrMag > 0);
  CoordType toSqrMag = to.sqrMag();
  assert("need nonzero length vector" && toSqrMag > 0);
  CoordType dot = Dot(from, to);
  CoordType sqrmagprod = fromSqrMag * toSqrMag;
  CoordType magprod = std::sqrt(sqrmagprod);
  CoordType ctheta_plus_1 = dot / magprod + 1;

  if(ctheta_plus_1 < WFMATH_EPSILON) {
    // 180 degree rotation, rotation plane indeterminate
    if(dim == 2) { // special case, only one rotation plane possible
      m_elem[0][0] = m_elem[1][1] = ctheta_plus_1 - 1;
      CoordType sin_theta = std::sqrt(2 * ctheta_plus_1); // to leading order
      bool direction = ((to[0] * from[1] - to[1] * from[0]) >= 0);
      m_elem[0][1] = direction ?  sin_theta : -sin_theta;
      m_elem[1][0] = -m_elem[0][1];
      m_flip = false;
      m_valid = true;
      m_age = 1;
      return *this;
    }
    throw ColinearVectors<dim>(from, to);
  }

  for(int i = 0; i < dim; ++i) {
    for(int j = i; j < dim; ++j) {
      CoordType projfrom = from[i] * from[j] / fromSqrMag;
      CoordType projto = to[i] * to[j] / toSqrMag;

      CoordType ijprod = from[i] * to[j], jiprod = to[i] * from[j];

      CoordType termthree = (ijprod + jiprod) * dot / sqrmagprod;

      CoordType combined = (projfrom + projto - termthree) / ctheta_plus_1;

      if(i == j) {
        m_elem[i][i] = 1 - combined;
      }
      else {
        CoordType diffterm = (jiprod - ijprod) / magprod;

        m_elem[i][j] = -diffterm - combined;
        m_elem[j][i] = diffterm - combined;
      }
    }
  }

  m_flip = false;
  m_valid = true;
  m_age = 1;

  return *this;
}

template<> RotMatrix<3>& RotMatrix<3>::rotation (const Vector<3>& axis,
						 CoordType theta);
template<> RotMatrix<3>& RotMatrix<3>::rotation (const Vector<3>& axis);
template<> RotMatrix<3>& RotMatrix<3>::fromQuaternion(const Quaternion& q,
						      const bool not_flip);

template<> RotMatrix<3>& RotMatrix<3>::rotate(const Quaternion&);

template<int dim>
inline RotMatrix<dim>& RotMatrix<dim>::mirror(const int i)
{
  assert(i >= 0 && i < dim);

  identity();
  m_elem[i][i] = -1;
  m_flip = true;
  // m_valid and m_age already set correctly

  return *this;
}

template<int dim>
inline RotMatrix<dim>& RotMatrix<dim>::mirror	(const Vector<dim>& v)
{
  // Get a flip by subtracting twice the projection operator in the
  // direction of the vector. A projection operator is idempotent (P*P == P),
  // and symmetric, so I - 2P is an orthogonal matrix.
  //
  // (I - 2P) * (I - 2P)^T == (I - 2P)^2 == I - 4P + 4P^2 == I

  CoordType sqr_mag = v.sqrMag();

  assert("need nonzero length vector" && sqr_mag > 0);

  // off diagonal
  for(int i = 0; i < dim - 1; ++i)
    for(int j = i + 1; j < dim; ++j)
      m_elem[i][j] = m_elem[j][i] = -2 * v[i] * v[j] / sqr_mag;

  // diagonal
  for(int i = 0; i < dim; ++i)
    m_elem[i][i] = 1 - 2 * v[i] * v[i] / sqr_mag;

  m_flip = true;
  m_valid = true;
  m_age = 1;

  return *this;
}

template<int dim>
inline RotMatrix<dim>& RotMatrix<dim>::mirror()
{
  for(int i = 0; i < dim; ++i)
    for(int j = 0; j < dim; ++j)
      m_elem[i][j] = (i == j) ? -1 : 0;

  m_flip = dim%2;
  m_valid = true;
  m_age = 0; // -1 and 0 are exact, no roundoff error


  return *this;
}

bool _MatrixInverseImpl(const int size, CoordType* in, CoordType* out);

template<int dim>
inline void RotMatrix<dim>::normalize()
{
  // average the matrix with it's inverse transpose,
  // that will clean up the error to linear order

  CoordType buf1[dim*dim], buf2[dim*dim]; 

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      buf1[j*dim + i] = m_elem[i][j];
      buf2[j*dim + i] = ((i == j) ? 1.f : 0.f);
    }
  }

  bool success = _MatrixInverseImpl(dim, buf1, buf2);
  assert(success); // matrix can't be degenerate
  if (!success) {
    return;
  }

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      CoordType& elem = m_elem[i][j];
      elem += buf2[i*dim + j];
      elem /= 2;
    }
  }

  m_age = 1;
}

} // namespace WFMath

#endif // WFMATH_ROTMATRIX_FUNCS_H
