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
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2001-12-7

#ifndef WFMATH_MATRIX_FUNCS_H
#define WFMATH_MATRIX_FUNCS_H

#include <wfmath/matrix.h>
#include <wfmath/error.h>

namespace WF { namespace Math {

template<const int rows, const int columns>
inline Matrix<rows,columns>::Matrix(const Matrix<rows,columns>& m)
{
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      m_elem[i][j] = m.m_elem[i][j];
}

template<const int rows, const int columns>
inline bool Matrix<rows,columns>::operator==(const Matrix<rows,columns>& m) const
{
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      if(m_elem[i][j] != m.m_elem[i][j])
        return false;

  return true;
}

template<const int rows, const int columns>
bool Matrix<rows,columns>::operator< (const Matrix<rows,columns>& m) const
{
  for(int i = 0; i < rows; ++i) {
    for(int j = 0; j < columns; ++j) {
      if(m_elem[i][j] < m.m_elem[i][j])
        return true;
      if(m_elem[i][j] > m.m_elem[i][j])
        return false;
    }
  }

  return false;
}

template<const int rows, const int columns>
inline Matrix<rows,columns> Matrix<rows,columns>::operator+(const Matrix<rows,columns>& m) const
{
  Matrix<rows,columns> out;

  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      out.m_elem[i][j] = this->m_elem[i][j] + m.m_elem[i][j];

  return out;
}

template<const int rows, const int columns>
inline Matrix<rows,columns> Matrix<rows,columns>::operator-(const Matrix<rows,columns>& m) const
{
  Matrix<rows,columns> out;

  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      out.m_elem[i][j] = m_elem[i][j] - m.m_elem[i][j];

  return out;
}

template<const int rows, const int columns> template<const int i>
inline Matrix<rows,i> Matrix<rows,columns>::operator*(const Matrix<columns, i>& m) const
{
  Matrix<columns, i> out;

  for(int j = 0; j < rows; ++j) {
    for(int k = 0; k < i; ++k) {
      out.m_elem[j][k] = 0;
      for(int l = 0; l < columns; ++l)
        out.m_elem[j][k] += m_elem[j][l] * m.m_elem[l][k];
    }
  }

  return out;
}

template<const int rows, const int columns>
Matrix<rows,columns> Matrix<rows,columns>::operator*(const double& d) const
{
  Matrix<rows,columns> out;

  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      out.m_elem[i][j] = m_elem[i][j] * d;

  return out;
}

template<const int rows, const int columns>
Matrix<rows,columns> operator*(const double& d, const Matrix<rows,columns>& m)
{
  Matrix<rows,columns> out;

  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      out.m_elem[i][j] = m.m_elem[i][j] * d;

  return out;
}

template<const int rows, const int columns>
Matrix<rows,columns> Matrix<rows,columns>::operator/(const double& d) const
{
  Matrix<rows,columns> out;

  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      out.m_elem[i][j] = m_elem[i][j] / d;

  return out;
}

template<const int rows, const int columns>
inline Matrix<rows,columns> Matrix<rows,columns>::operator-() const // Unary minus
{
  Matrix<rows,columns> out;

  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      out.m_elem[i][j] = -m_elem[i][j];

  return out;
}

template<const int rows, const int columns>
inline Matrix<rows,columns>& Matrix<rows,columns>::operator+=(const Matrix<rows,columns>& m)
{
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      m_elem[i][j] += m.m_elem[i][j];

  return *this;
}

template<const int rows, const int columns>
inline Matrix<rows,columns>& Matrix<rows,columns>::operator-=(const Matrix<rows,columns>& m)
{
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      m_elem[i][j] -= m.m_elem[i][j];

  return *this;
}

template<const int rows, const int columns>
Matrix<rows,columns>& Matrix<rows,columns>::operator*=(const double& d)
{
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      m_elem[i][j] *= d;

  return *this;
}

template<const int rows, const int columns>
Matrix<rows,columns>& Matrix<rows,columns>::operator/=(const double& d)
{
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      m_elem[i][j] /= d;

  return *this;
}

template<const int rows, const int columns>
inline Vector<rows> Matrix<rows,columns>::operator*(const Vector<columns>& v) const
{
  Vector<rows> out;

  for(int i = 0; i < rows; ++i) {
    out[i] = 0;
    for(int j = 0; j < columns; ++j)
      out[i] += m_elem[i][j] * v[j];
  }

  return out;
}

template<const int rows, const int columns>
inline Matrix<rows,columns> OuterProduct(const Vector<rows>& v1, const Vector<columns>& v2)
{
  Matrix<rows,columns> out;

  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      out.m_elem[i][j] = v1[i] * v2[j];
}

template<const int rows, const int columns>
inline Vector<columns> Matrix<rows,columns>::row(const int i) const
{
  Vector<columns> out;

  for(int j = 0; j < columns; ++j)
    out[j] = m_elem[i][j];

  return out;
}

template<const int rows, const int columns>
void Matrix<rows,columns>::setRow(const int i, const Vector<columns>& v)
{
  for(int j = 0; j < columns; ++j)
    m_elem[i][j] = v[j];
}

template<const int rows, const int columns>
inline Vector<rows> Matrix<rows,columns>::column(const int i) const
{
  Vector<columns> out;

  for(int j = 0; j < rows; ++j)
    out[j] = m_elem[j][i];

  return out;
}

template<const int rows, const int columns>
void Matrix<rows,columns>::setColumn(const int i, const Vector<rows>& v)
{
  for(int j = 0; j < rows; ++j)
    m_elem[j][i] = v[j];
}

template<const int rows, const int columns>
inline Matrix<rows,columns>& Matrix<rows,columns>::zero()
{
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      m_elem[i][j] = 0;

  return *this;
}

template<const int rows, const int columns>
inline Matrix<columns,rows> Matrix<rows,columns>::transpose() const
{
  Matrix<columns,rows> m;

  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < columns; ++j)
      m.m_elem[j][i] = m_elem[i][j];

  return m;
}

template<const int rows, const int columns>
inline Matrix<rows,columns>& Matrix<rows,columns>::identity()
{
  Vector<rows> v;

  for(int i = 0; i < rows; ++i)
    v[i] = 1;

  this->diagonal(v);

  return *this;
}

template<const int size>
inline Matrix<size> DiagonalMatrix(const Vector<size>& v)
{
  Matrix<size> out;

  out.zero();

  for(int i = 0; i < size; ++i)
    out.m_elem[i][i] = v[i];

  return out;
}

template<const int size>
inline double Trace(const Matrix<size>& m)
{
  double out = 0;

  for(int i = 0; i < size; ++i)
    out += m.m_elem[i][i];

  return out;
}

double _MatrixDeterminantImpl(const int size, double* m);

template<const int size>
inline double Determinant(const Matrix<size>& m)
{
  double mtmp[size*size]; // Scratch space

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      mtmp[i*size+j] = m.elem(i, j);

  return _MatrixDeterminantImpl(size, mtmp);
}

int _MatrixInverseImpl(const int size, double* in, double* out);

template<const int size>
inline Matrix<size> Inverse(const Matrix<size>& m)
{
  double in[size*size], out[size*size]; // Scratch space

  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
      in[i*size+j] = m.elem(i, j);
      out[i*size+j] = (i == j) ? 1 : 0;
    }
  }

  if(_MatrixInverseImpl(size, in, out) != 0)
    throw DegenerateMatrix<size>(m);

  Matrix<size> mout;

  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      mout.elem(i, j) = out[i*size+j];

  return mout;
}

}} // namespace WF::Math

#endif // WFMATH_MATRIX_FUNCS_H
