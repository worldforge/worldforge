// -*-C++-*-
// matrix.h (RotMatrix<> class definition)
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

#ifndef WFMATH_MATRIX_H
#define WFMATH_MATRIX_H

#include <wfmath/const.h>

namespace WF { namespace Math {

template<const int dim> class Vector;

// Elements of this class represent rotation matrices. The NxN dimensional
// rotation matrices form a group called SO(N), the special orthogonal
// matrices. They satisfy two conditions:
//
// 1) They are ortohgonal. That is, their transpose is equal to their inverse.
//    Hence, this class does not implement a transpose() method, only an
//    inverse().
//
// 2) Their determinant is equal to 1 (as opposed to -1, indicating a combination
//    of a rotation and a mirror-image flip). This is the "special" part
//    of "special orthogonal."
//
// A general N dimensional matrix of this type has N(N-1)/2 degrees of freedom.
// This gives one rotation angle in 2D, the three Euler angles in 3D, etc.

// TODO? Extend to general orthogonal matrices (include mirror-image flips)?
// Probably want to add a boolean variable to keep track of the sign of the
// determinant if we do this.

template<const int dim> class RotMatrix;

template<const int dim> // m1 * m2
RotMatrix<dim> Prod(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2);
template<const int dim> // m1 * m2^-1
RotMatrix<dim> ProdInv(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2);
template<const int dim> // m1^-1 * m2
RotMatrix<dim> InvProd(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2);
template<const int dim> // m1^-1 * m2^-1
RotMatrix<dim> InvProdInv(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2);

template<const int dim> // m * v
Vector<dim> Prod(const RotMatrix<dim>& m, const Vector<dim>& v);
template<const int dim> // m^-1 * v
Vector<dim> InvProd(const RotMatrix<dim>& m, const Vector<dim>& v);
template<const int dim> // v * m
inline Vector<dim> Prod(const Vector<dim>& v, const RotMatrix<dim>& m)
	{return InvProd(m, v);} // Since transpose() and inverse() are the same
template<const int dim> // v * m^-1
inline Vector<dim> ProdInv(const Vector<dim>& v, const RotMatrix<dim>& m)
	{return Prod(m, v);} // Since transpose() and inverse() are the same

template<const int dim>
class RotMatrix {
 public:
  RotMatrix() {identity();} // Need a valid RotMatrix for default
  RotMatrix(const RotMatrix& m);

  std::string toString() const;
  bool fromString(const std::string& s);

  RotMatrix& operator=(const RotMatrix& m);
  // No operator=(double d[dim][dim]), since it can fail.
  // Use setVals() instead.

  bool isEqualTo(const RotMatrix& rhs, double tolerance = WFMATH_EPSILON) const;

  bool operator==(const RotMatrix& m) const {return isEqualTo(m);}
  bool operator!=(const RotMatrix& m) const {return !isEqualTo(m);}

  RotMatrix& identity();

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the matrix.
  bool operator< (const RotMatrix& m) const;

  static const int nParams = dim*(dim-1)/2;

  const CoordType& elem(const int i, const int j) const 	{return m_elem[i][j];}

  // Can't set one element at a time and keep it an SO(N) matrix,
  // but can try to set all values at once, and see if they match.
  // The first one is vals[row][column], the second is vals[row*dim+column].
  bool setVals(const CoordType vals[dim][dim], double precision = WFMATH_EPSILON);
  bool setVals(const CoordType vals[dim*dim], double precision = WFMATH_EPSILON);

  Vector<dim> row(const int i) const;
  Vector<dim> column(const int i) const;

  double trace() const;
  double determinant() const {return 1;} // here in case we extend to O(N) later
  RotMatrix inverse() const;

  RotMatrix& fromEuler(const CoordType angles[nParams]);
  void toEuler(CoordType angles[nParams]) const;

  friend RotMatrix Prod<dim>	   (const RotMatrix& m1, const RotMatrix& m2);
  friend RotMatrix ProdInv<dim>	   (const RotMatrix& m1, const RotMatrix& m2);
  friend RotMatrix InvProd<dim>	   (const RotMatrix& m1, const RotMatrix& m2);
  friend RotMatrix InvProdInv<dim> (const RotMatrix& m1, const RotMatrix& m2);
  friend Vector<dim> Prod<dim>	   (const RotMatrix& m, const Vector<dim>& v);
  friend Vector<dim> InvProd<dim>  (const RotMatrix& m, const Vector<dim>& v);

  // Set the value to a given rotation
  const RotMatrix& rotation	(const int i, const int j, const double& theta);
  const RotMatrix& rotation	(const Vector<dim>& v1, const Vector<dim>& v2,
				 const double& theta);

  // 2D/3D stuff

  RotMatrix(const CoordType& alpha, const CoordType& beta, const CoordType& gamma)
	{CoordType d[3] = {alpha, beta, gamma}; toEuler(d);} // Euler angles, 3D only

  // 2D only
  const RotMatrix<2>& rotation			(const double& theta)
	{return rotation(0, 1, theta);}

  // 3D only
  const RotMatrix<3>& rotationX			(const double& theta)
	{return rotation(1, 2, theta);}
  const RotMatrix<3>& rotationY			(const double& theta)
	{return rotation(2, 0, theta);}
  const RotMatrix<3>& rotationZ			(const double& theta)
	{return rotation(0, 1, theta);}
  const RotMatrix<3>& rotation			(const Vector<3>& axis,
						 const double& theta);

 private:
  CoordType m_elem[dim][dim];

  // Backend to setVals() above, also used in fromString()
  bool _setVals(CoordType *vals, double precision = WFMATH_EPSILON);
};

}} // namespace WF::Math

#include <wfmath/matrix_funcs.h>

#endif // WFMATH_MATRIX_H
