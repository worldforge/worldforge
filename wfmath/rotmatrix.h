// rotmatrix.h (RotMatrix<> class definition)
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

#ifndef WFMATH_ROTMATRIX_H
#define WFMATH_ROTMATRIX_H

#include <wfmath/const.h>

namespace WFMath {

template<const int dim> class Vector;
class Quaternion;

// Elements of this class represent rotation matrices. The NxN dimensional
// rotation matrices form a group called O(N), the orthogonal
// matrices. They satisfy the following condition:
//
//  They are orthogonal. That is, their transpose is equal to their inverse.
//  Hence, this class does not implement a transpose() method, only an
//  inverse().
//
// A general N dimensional matrix of this type has N(N-1)/2 degrees of freedom.
// This gives one rotation angle in 2D, the three Euler angles in 3D, etc.

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
Vector<dim> Prod(const Vector<dim>& v, const RotMatrix<dim>& m);
template<const int dim> // v * m^-1
Vector<dim> ProdInv(const Vector<dim>& v, const RotMatrix<dim>& m);

template<const int dim>
RotMatrix<dim> operator*(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2);
template<const int dim>
Vector<dim> operator*(const RotMatrix<dim>& m, const Vector<dim>& v);
template<const int dim>
Vector<dim> operator*(const Vector<dim>& v, const RotMatrix<dim>& m);

template<const int dim>
std::ostream& operator<<(std::ostream& os, const RotMatrix<dim>& m);
template<const int dim>
std::istream& operator>>(std::istream& is, RotMatrix<dim>& m);

template<const int dim>
class RotMatrix {
 public:
  RotMatrix() : m_valid(false) {}
  RotMatrix(const RotMatrix& m);

  friend std::ostream& operator<< <dim>(std::ostream& os, const RotMatrix& m);
  friend std::istream& operator>> <dim>(std::istream& is, RotMatrix& m);

  RotMatrix& operator=(const RotMatrix& m);
  // No operator=(CoordType d[dim][dim]), since it can fail.
  // Use setVals() instead.

  bool isEqualTo(const RotMatrix& m, double epsilon = WFMATH_EPSILON) const;

  bool operator==(const RotMatrix& m) const {return isEqualTo(m);}
  bool operator!=(const RotMatrix& m) const {return !isEqualTo(m);}

  bool isValid() const {return m_valid;}

  RotMatrix& identity();

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the matrix.
  bool operator< (const RotMatrix& m) const;

  CoordType elem(const int i, const int j) const
	{assert(i >= 0 && j >= 0 && i < dim && j < dim); return m_elem[i][j];}

  // Can't set one element at a time and keep it an O(N) matrix,
  // but can try to set all values at once, and see if they match.
  // The first one is vals[row][column], the second is vals[row*dim+column].
  bool setVals(const CoordType vals[dim][dim], double precision = WFMATH_EPSILON);
  bool setVals(const CoordType vals[dim*dim], double precision = WFMATH_EPSILON);

  Vector<dim> row(const int i) const;
  Vector<dim> column(const int i) const;

  CoordType trace() const;
  CoordType determinant() const {return (CoordType) (m_flip ? -1 : 1);}
  RotMatrix inverse() const;

  friend RotMatrix Prod<dim>	   (const RotMatrix& m1, const RotMatrix& m2);
  friend RotMatrix ProdInv<dim>	   (const RotMatrix& m1, const RotMatrix& m2);
  friend RotMatrix InvProd<dim>	   (const RotMatrix& m1, const RotMatrix& m2);
  friend RotMatrix InvProdInv<dim> (const RotMatrix& m1, const RotMatrix& m2);
  friend Vector<dim> Prod<dim>	   (const RotMatrix& m, const Vector<dim>& v);
  friend Vector<dim> InvProd<dim>  (const RotMatrix& m, const Vector<dim>& v);

  // Set the value to a given rotation

  // Two axes and an angle
  RotMatrix& rotation	(const int i, const int j, CoordType theta);
  // Two vectors in a plane and an angle
  RotMatrix& rotation	(const Vector<dim>& v1, const Vector<dim>& v2,
			 CoordType theta);
  // A rotation which will move "from" to lie parallel to "to"
  RotMatrix& rotation	(const Vector<dim>& from, const Vector<dim>& to);

  // Set the value to mirror image about a certain axis

  RotMatrix& mirror(const int i);
  RotMatrix& mirror(const Vector<dim>& v);
  RotMatrix& mirror(); // Flip all axes, only changes the parity if dim is odd

  // 2D/3D stuff

  // Quaternion, 3D only
  RotMatrix(const Quaternion& q, const bool not_flip = true)
	{fromQuaternion(q, not_flip);}

  // 2D only
  RotMatrix<2>& rotation(CoordType theta)
	{return rotation(0, 1, theta);}

  // 3D only
  RotMatrix<3>& rotationX(CoordType theta) {return rotation(1, 2, theta);}
  RotMatrix<3>& rotationY(CoordType theta) {return rotation(2, 0, theta);}
  RotMatrix<3>& rotationZ(CoordType theta) {return rotation(0, 1, theta);}
  RotMatrix<3>& rotation(const Vector<3>& axis, CoordType theta);
  RotMatrix<3>& rotation(const Vector<3>& axis); // angle taken from magnitude of axis

  RotMatrix<3>& fromQuaternion(const Quaternion& q, const bool not_flip = true);

  RotMatrix& mirrorX()	{return mirror(0);}
  RotMatrix& mirrorY()	{return mirror(1);}
  RotMatrix& mirrorZ()	{return mirror(2);}

 private:
  CoordType m_elem[dim][dim];
  bool m_flip; // True if the matrix is parity odd
  bool m_valid;

  // Backend to setVals() above, also used in fromStream()
  bool _setVals(CoordType *vals, double precision = WFMATH_EPSILON);
};

} // namespace WFMath

#include <wfmath/rotmatrix_funcs.h>

#endif // WFMATH_ROTMATRIX_H
