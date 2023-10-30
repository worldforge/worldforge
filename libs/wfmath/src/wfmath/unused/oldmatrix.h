// -*-C++-*-
// matrix.h (Matrix<> class definition)
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

#ifndef WFMATH_MATRIX_H
#define WFMATH_MATRIX_H

#include <wfmath/vector.h>

namespace WF { namespace Math {

template<const int rows, const int columns> class RowVector;

template<const int rows, const int columns>
Matrix<rows,columns> operator*(const double& d, const Matrix<rows,columns>& m);
template<const int rows, const int columns>
Matrix<rows,columns> OuterProduct(const Vector<rows>& v1, const Vector<columns>& v2);

template<const int size>
Matrix<size,size> DiagonalMatrix(const Vector<size>& v);
template<const int size>
double Trace(const Matrix<size,size>& m);
template<const int size>
double Determinant(const Matrix<size,size>& m);
template<const int size>
Matrix<size,size> Inverse(const Matrix<size,size>& m);

template<const int rows, const int columns = rows>
class Matrix {
 public:
  Matrix() {}
  Matrix(const Matrix<rows,columns>& m);

  bool operator==(const Matrix<rows,columns>& m) const;
  bool operator!=(const Matrix<rows,columns>& m) const {return !(*this == m);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the matrix.
  bool operator< (const Matrix<rows,columns>& m) const;

  // Division only makes sense for square matrices, make people use
  // inverse() explicitly

  Matrix<rows,columns> operator+(const Matrix<rows,columns>& m) const;
  Matrix<rows,columns> operator-(const Matrix<rows,columns>& m) const;
  template<const int i>
    Matrix<rows,i> operator*(const Matrix<columns,i>& m) const;
  Matrix<rows,columns> operator*(const double& d) const;
  Matrix<rows,columns> operator/(const double& d) const;

  Matrix<rows,columns> operator-() const; // Unary minus

  Matrix<rows,columns>& operator+=(const Matrix<rows,columns>& m);
  Matrix<rows,columns>& operator-=(const Matrix<rows,columns>& m);
  // Since we need to construct a matrix to hold the temporary values
  // anyway, just use operator* to implement this one
  Matrix<rows,columns>& operator*=(const Matrix<columns,columns>& m)
	{*this = *this * m; return *this;}
  Matrix<rows,columns>& operator*=(const double& d);
  Matrix<rows,columns>& operator/=(const double& d);

  Vector<rows> operator*(const Vector<columns>& v) const;

// FIXME it doesn't like this
//  friend Matrix<rows,columns> operator*<rows,columns>(const double& d,
//					const Matrix<rows,columns>& m);

  friend Matrix<rows,columns> OuterProduct<rows,columns>(const Vector<rows>& v1,
							 const Vector<columns>& v2);

  const double& elem(const int i, const int j) const 	{return m_elem[i][j];}
  double& elem(const int i, const int j)		{return m_elem[i][j];}

  Vector<columns> row(const int i) const;
  void setRow(const int i, const Vector<columns>& v);
  Vector<rows> column(const int i) const;
  void setColumn(const int i, const Vector<rows>& v);

  Matrix<rows,columns>& zero();

  Matrix<columns,rows> transpose() const;

  // The following are only defined for square matrices. Trying
  // to use them with non-square matrices will result in various
  // compiler/linker errors.

  // FIXME is there some way to do this with partial specialization,
  // without having to rewrite all the operator functions?

  Matrix<rows,columns>& identity();
  Matrix<rows,columns>& diagonal(const Vector<rows>& v)
	{return *this = DiagonalMatrix(v);}

  double trace() const			{return Trace(*this);}
  double determinant() const		{return Determinant(*this);}
  Matrix<columns,rows> inverse() const	{return Inverse(*this);}

  friend Matrix<rows> DiagonalMatrix<rows>(const Vector<rows>& v);

  friend double Trace<rows>(const Matrix<rows>& m);
  friend double Determinant<rows>(const Matrix<rows>& m);
  friend Matrix<rows> Inverse<rows>(const Matrix<rows>& m);

  //TODO rotate() and string functions, Atlas and Tcross functions from stage/math

 private:
  double m_elem[rows][columns];
};

Matrix<3> SkewSymmetric(const Vector<3>& v);

}} // namespace WF::Math

#endif // WFMATH_MATRIX_H
