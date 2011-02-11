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

#include <iosfwd>

namespace WFMath {

/// returns m1 * m2
template<const int dim> // m1 * m2
RotMatrix<dim> Prod(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2);
/// returns m1 * m2^-1
template<const int dim> // m1 * m2^-1
RotMatrix<dim> ProdInv(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2);
/// returns m1^-1 * m2
template<const int dim> // m1^-1 * m2
RotMatrix<dim> InvProd(const RotMatrix<dim>& m1, const RotMatrix<dim>& m2);
/// returns m1^-1 * m2^-1
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

/// returns m1 * m2
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

/// A dim dimensional rotation matrix. Technically, a member of the group O(dim).
/**
 * Elements of this class represent rotation matrices. The NxN dimensional
 * rotation matrices form a group called O(N), the orthogonal
 * matrices. They satisfy the following condition:
 *
 *  They are orthogonal. That is, their transpose is equal to their inverse.
 *  Hence, this class does not implement a transpose() method, only an
 *  inverse().
 *
 * A general N dimensional matrix of this type has N(N-1)/2 degrees of freedom.
 * This gives one rotation angle in 2D, the three Euler angles in 3D, etc.
 *
 * This class implements the 'generic' subset of the interface in
 * the fake class Shape.
 **/
template<const int dim = 3>
class RotMatrix {
 public:
  ///
  RotMatrix() : m_valid(false) {}
  ///
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

  /// set the matrix to the identity matrix
  RotMatrix& identity();

  /// get the (i, j) element of the matrix
  CoordType elem(const int i, const int j) const {return m_elem[i][j];}

  /// Set the values of the elements of the matrix
  /**
   * Can't set one element at a time and keep it an O(N) matrix,
   * but can try to set all values at once, and see if they match.
   * This fails if the passed matrix is not orthogonal within the
   * passed precision, and orthogonalizes the matrix to within
   * precision WFMATH_EPSILON.
   **/
  bool setVals(const CoordType vals[dim][dim], double precision = WFMATH_EPSILON);
  /// Set the values of the elements of the matrix
  /**
   * Can't set one element at a time and keep it an O(N) matrix,
   * but can try to set all values at once, and see if they match.
   * This fails if the passed matrix is not orthogonal within the
   * passed precision, and orthogonalizes the matrix to within
   * precision WFMATH_EPSILON.
   **/
  bool setVals(const CoordType vals[dim*dim], double precision = WFMATH_EPSILON);

  /// Get a copy of the i'th row as a Vector
  Vector<dim> row(const int i) const;
  /// Get a copy of the i'th column as a Vector
  Vector<dim> column(const int i) const;

  /// Get the trace of the matrix
  CoordType trace() const;
  /// Get the determinant of the matrix
  /**
   * Since the matrix is orthogonal, the determinant is always either 1 or -1.
   **/
  CoordType determinant() const {return (CoordType) (m_flip ? -1 : 1);}
  /// Get the inverse of the matrix
  /**
   * Since the matrix is orthogonal, the inverse is equal to the transpose.
   **/
  RotMatrix inverse() const;
  /// Get the parity of the matrix
  /**
   * Returns true for odd parity, false for even.
   **/
  bool parity() const {return m_flip;}

  // documented outside the class

  friend RotMatrix Prod<dim>	   (const RotMatrix& m1, const RotMatrix& m2);
  friend RotMatrix ProdInv<dim>	   (const RotMatrix& m1, const RotMatrix& m2);
  friend RotMatrix InvProd<dim>	   (const RotMatrix& m1, const RotMatrix& m2);
  friend RotMatrix InvProdInv<dim> (const RotMatrix& m1, const RotMatrix& m2);
  friend Vector<dim> Prod<dim>	   (const RotMatrix& m, const Vector<dim>& v);
  friend Vector<dim> InvProd<dim>  (const RotMatrix& m, const Vector<dim>& v);

  // Set the value to a given rotation

  /// set the matrix to a rotation by the angle theta in the (i, j) plane
  RotMatrix& rotation	(const int i, const int j, CoordType theta);
  /// set the matrix to a rotation by the angle theta in the v1, v2 plane
  /**
   * Throws CollinearVectors if v1 and v2 are parallel
   **/
  RotMatrix& rotation	(const Vector<dim>& v1, const Vector<dim>& v2,
			 CoordType theta);
  /// set the matrix to a rotation which will move "from" to lie parallel to "to"
  /**
   * Throws CollinearVectors if v1 and v2 are antiparallel (parallel but
   * pointing in opposite directions). If v1 and v2 point in the
   * same direction, the matrix is set to the identity.
   **/
  RotMatrix& rotation	(const Vector<dim>& from, const Vector<dim>& to);

  // Set the value to mirror image about a certain axis

  /// set the matrix to a mirror perpendicular to the i'th axis
  RotMatrix& mirror(const int i);
  /// set the matrix to a mirror perpendicular to the Vector v
  RotMatrix& mirror(const Vector<dim>& v);
  /// set the matrix to mirror all axes
  /**
   * This is a good parity operator if dim is odd.
   **/
  RotMatrix& mirror();

  /// rotate the matrix using another matrix
  RotMatrix& rotate(const RotMatrix& m) {return *this = Prod(*this, m);}

  /// normalize to remove accumulated round-off error
  void normalize();
  /// current round-off age
  unsigned age() const {return m_age;}

  // 2D/3D stuff

  /// 3D only: Construct a RotMatrix from a Quaternion
  /**
   * since Quaternions can only specify parity-even
   * rotations, you can pass the return value of
   * Quaternion::fromRotMatrix() as not_flip to
   * recover the full RotMatrix
   **/
  RotMatrix(const Quaternion& q, const bool not_flip = true);

  /// 2D only: Construct a RotMatrix from an angle theta
  RotMatrix& rotation(CoordType theta)
	{return rotation(0, 1, theta);}

  /// 3D only: set a RotMatrix to a rotation about the x axis by angle theta
  RotMatrix& rotationX(CoordType theta) {return rotation(1, 2, theta);}
  /// 3D only: set a RotMatrix to a rotation about the y axis by angle theta
  RotMatrix& rotationY(CoordType theta) {return rotation(2, 0, theta);}
  /// 3D only: set a RotMatrix to a rotation about the z axis by angle theta
  RotMatrix& rotationZ(CoordType theta) {return rotation(0, 1, theta);}
  /// 3D only: set a RotMatrix to a rotation about the axis given by the Vector
  RotMatrix& rotation(const Vector<dim>& axis, CoordType theta);
  /// 3D only: set a RotMatrix to a rotation about the axis given by the Vector
  /**
   * the rotation angle is taken from the Vector's magnitude
   **/
  RotMatrix& rotation(const Vector<dim>& axis); // angle taken from magnitude of axis

  /// 3D only: set a RotMatrix from a Quaternion
  /**
   * since Quaternions can only specify parity-even
   * rotations, you can pass the return value of
   * Quaternion::fromRotMatrix() as not_flip to
   * recover the full RotMatrix
   **/
  RotMatrix& fromQuaternion(const Quaternion& q, const bool not_flip = true);

  /// rotate the matrix using the quaternion
  RotMatrix& rotate(const Quaternion&);

  /// set a RotMatrix to a mirror perpendicular to the x axis
  RotMatrix& mirrorX()	{return mirror(0);}
  /// set a RotMatrix to a mirror perpendicular to the y axis
  RotMatrix& mirrorY()	{return mirror(1);}
  /// set a RotMatrix to a mirror perpendicular to the z axis
  RotMatrix& mirrorZ()	{return mirror(2);}

 private:
  CoordType m_elem[dim][dim];
  bool m_flip; // True if the matrix is parity odd
  bool m_valid;
  unsigned m_age;

  // Backend to setVals() above, also used in fromStream()
  bool _setVals(CoordType *vals, double precision = WFMATH_EPSILON);
  void checkNormalization() {if(m_age >= WFMATH_MAX_NORM_AGE && m_valid) normalize();}
};

} // namespace WFMath

#endif // WFMATH_ROTMATRIX_H
