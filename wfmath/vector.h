// vector.h (Vector<> class definition)
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

// Extensive amounts of this material come from the Vector2D
// and Vector3D classes from stage/math, written by Bryce W.
// Harrington, Kosh, and Jari Sundell (Rakshasa).

#ifndef WFMATH_VECTOR_H
#define WFMATH_VECTOR_H

#include <wfmath/const.h>
#include <wfmath/zero.h>

#include <iosfwd>

namespace WFMath {

template<const int dim>
Vector<dim>& operator+=(Vector<dim>& v1, const Vector<dim>& v2);
template<const int dim>
Vector<dim>& operator-=(Vector<dim>& v1, const Vector<dim>& v2);
template<const int dim>
Vector<dim>& operator*=(Vector<dim>& v, CoordType d);
template<const int dim>
Vector<dim>& operator/=(Vector<dim>& v, CoordType d);

template<const int dim>
Vector<dim> operator+(const Vector<dim>& v1, const Vector<dim>& v2);
template<const int dim>
Vector<dim> operator-(const Vector<dim>& v1, const Vector<dim>& v2);
template<const int dim>
Vector<dim> operator-(const Vector<dim>& v); // Unary minus
template<const int dim>
Vector<dim> operator*(CoordType d, const Vector<dim>& v);
template<const int dim>
Vector<dim> operator*(const Vector<dim>& v, CoordType d);
template<const int dim>
Vector<dim> operator/(const Vector<dim>& v, CoordType d);

template<const int dim>
CoordType Dot(const Vector<dim>& v1, const Vector<dim>& v2);

template<const int dim>
CoordType Angle(const Vector<dim>& v, const Vector<dim>& u);

// The following are defined in rotmatrix_funcs.h
/// returns m * v
template<const int dim> // m * v
Vector<dim> Prod(const RotMatrix<dim>& m, const Vector<dim>& v);
/// returns m^-1 * v
template<const int dim> // m^-1 * v
Vector<dim> InvProd(const RotMatrix<dim>& m, const Vector<dim>& v);
/// returns v * m
/**
 * This is the function to use to rotate a Vector v using a Matrix m
 **/
template<const int dim> // v * m
Vector<dim> Prod(const Vector<dim>& v, const RotMatrix<dim>& m);
/// return v * m^-1
template<const int dim> // v * m^-1
Vector<dim> ProdInv(const Vector<dim>& v, const RotMatrix<dim>& m);

///
template<const int dim>
Vector<dim> operator*(const RotMatrix<dim>& m, const Vector<dim>& v);
///
template<const int dim>
Vector<dim> operator*(const Vector<dim>& v, const RotMatrix<dim>& m);

template<const int dim>
Vector<dim> operator-(const Point<dim>& c1, const Point<dim>& c2);
template<const int dim>
Point<dim> operator+(const Point<dim>& c, const Vector<dim>& v);
template<const int dim>
Point<dim> operator-(const Point<dim>& c, const Vector<dim>& v);
template<const int dim>
Point<dim> operator+(const Vector<dim>& v, const Point<dim>& c);

template<const int dim>
Point<dim>& operator+=(Point<dim>& p, const Vector<dim>& v);
template<const int dim>
Point<dim>& operator-=(Point<dim>& p, const Vector<dim>& v);

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Vector<dim>& v);
template<const int dim>
std::istream& operator>>(std::istream& is, Vector<dim>& v);

/// A dim dimensional vector
/**
 * This class implements the 'generic' subset of the interface in
 * the fake class Shape.
 **/
template<const int dim>
class Vector {
 friend class ZeroPrimitive<Vector<dim> >;
 public:
  /// Construct an uninitialized vector
  Vector() : m_valid(false) {}
  /// Construct a copy of a vector
  Vector(const Vector& v);
  /// Construct a vector from an object passed by Atlas
  explicit Vector(const AtlasInType& a);
  /// Construct a vector from a point.
  explicit Vector(const Point<dim>& point);

  /**
   * @brief Provides a global instance preset to zero.
   */
  static const Vector<dim>& ZERO()
  {
    static ZeroPrimitive<Vector<dim> > zeroVector(dim);
    return zeroVector.getShape();
  }
  
  friend std::ostream& operator<< <dim>(std::ostream& os, const Vector& v);
  friend std::istream& operator>> <dim>(std::istream& is, Vector& v);

  /// Create an Atlas object from the vector
  AtlasOutType toAtlas() const;
  /// Set the vector's value to that given by an Atlas object
  void fromAtlas(const AtlasInType& a);

  Vector& operator=(const Vector& v);

  bool isEqualTo(const Vector& v, double epsilon = WFMATH_EPSILON) const;
  bool operator==(const Vector& v) const {return isEqualTo(v);}
  bool operator!=(const Vector& v) const {return !isEqualTo(v);}

  bool isValid() const {return m_valid;}
  /// make isValid() return true if you've initialized the vector by hand
  void setValid(bool valid = true) {m_valid = valid;}

  /// Zero the components of a vector
  Vector& zero();

  // Math operators

  /// Add the second vector to the first
  friend Vector& operator+=<dim>(Vector& v1, const Vector& v2);
  /// Subtract the second vector from the first
  friend Vector& operator-=<dim>(Vector& v1, const Vector& v2);
  /// Multiply the magnitude of v by d
  friend Vector& operator*=<dim>(Vector& v, CoordType d);
  /// Divide the magnitude of v by d
  friend Vector& operator/=<dim>(Vector& v, CoordType d);

  /// Take the sum of two vectors
  friend Vector operator+<dim>(const Vector& v1, const Vector& v2);
  /// Take the difference of two vectors
  friend Vector operator-<dim>(const Vector& v1, const Vector& v2);
  /// Reverse the direction of a vector
  friend Vector operator-<dim>(const Vector& v); // Unary minus
  /// Multiply a vector by a scalar
  friend Vector operator*<dim>(CoordType d, const Vector& v);
  /// Multiply a vector by a scalar
  friend Vector operator*<dim>(const Vector& v, CoordType d);
  /// Divide a vector by a scalar
  friend Vector operator/<dim>(const Vector& v, CoordType d);

  // documented outside the class definition
  friend Vector Prod<dim>(const RotMatrix<dim>& m, const Vector& v);
  friend Vector InvProd<dim>(const RotMatrix<dim>& m, const Vector& v);

  /// Get the i'th element of the vector
  CoordType operator[](const int i) const {return m_elem[i];}
  /// Get the i'th element of the vector
  CoordType& operator[](const int i)      {return m_elem[i];}

  /// Find the vector which gives the offset between two points
  friend Vector operator-<dim>(const Point<dim>& c1, const Point<dim>& c2);
  /// Find the point at the offset v from the point c
  friend Point<dim> operator+<dim>(const Point<dim>& c, const Vector& v);
  /// Find the point at the offset -v from the point c
  friend Point<dim> operator-<dim>(const Point<dim>& c, const Vector& v);
  /// Find the point at the offset v from the point c
  friend Point<dim> operator+<dim>(const Vector& v, const Point<dim>& c);

  /// Shift a point by a vector
  friend Point<dim>& operator+=<dim>(Point<dim>& p, const Vector& rhs);
  /// Shift a point by a vector, in the opposite direction
  friend Point<dim>& operator-=<dim>(Point<dim>& p, const Vector& rhs);

  /// The dot product of two vectors
  friend CoordType Dot<dim>(const Vector& v1, const Vector& v2);
  /// The angle between two vectors
  friend CoordType Angle<dim>(const Vector& v, const Vector& u);

  /// The squared magnitude of a vector
  CoordType sqrMag() const;
  /// The magnitude of a vector
  CoordType mag() const		{return (CoordType) sqrt(sqrMag());}
  /// Normalize a vector
  Vector& normalize(CoordType norm = 1.0)
  {CoordType themag = mag(); return (*this *= norm / themag);}

  /// An approximation to the magnitude of a vector
  /**
   * The sloppyMag() function gives a value between
   * the true magnitude and sloppyMagMax multiplied by the
   * true magnitude. sloppyNorm() uses sloppyMag() to normalize
   * the vector. This is currently only implemented for
   * dim = {1, 2, 3}. For all current implementations,
   * sloppyMagMax is greater than or equal to one.
   * The constant sloppyMagMaxSqrt is provided for those
   * who want to most closely approximate the true magnitude,
   * without caring whether it's too low or too high.
   **/
  CoordType sloppyMag() const;
  /// Approximately normalize a vector
  /**
   * Normalize a vector using sloppyMag() instead of the true magnitude.
   * The new length of the vector will be between norm/sloppyMagMax()
   * and norm.
   **/
  Vector& sloppyNorm(CoordType norm = 1.0);

  // Can't seem to implement these as constants, implementing
  // inline lookup functions instead.
  /// The maximum ratio of the return value of sloppyMag() to the true magnitude
  static const CoordType sloppyMagMax();
  /// The square root of sloppyMagMax()
  /**
   * This is provided for people who want to obtain maximum accuracy from
   * sloppyMag(), without caring whether the answer is high or low.
   * The result sloppyMag()/sloppyMagMaxSqrt() will be within sloppyMagMaxSqrt()
   * of the true magnitude.
   **/
  static const CoordType sloppyMagMaxSqrt();

  /// Rotate the vector in the (axis1, axis2) plane by the angle theta
  Vector& rotate(int axis1, int axis2, CoordType theta);

  /// Rotate the vector in the (v1, v2) plane by the angle theta
  /**
   * This throws CollinearVectors if v1 and v2 are parallel.
   **/
  Vector& rotate(const Vector& v1, const Vector& v2, CoordType theta);

  /// Rotate the vector using a matrix
  Vector& rotate(const RotMatrix<dim>&);

  // mirror image functions

  /// Reflect a vector in the direction of the i'th axis
  Vector& mirror(const int i) { m_elem[i] *= -1; return *this;}
  /// Reflect a vector in the direction specified by v
  Vector& mirror(const Vector& v)
  {return operator-=(*this, 2 * v * Dot(v, *this) / v.sqrMag());}
  /// Reflect a vector in all directions simultaneously.
  /**
   * This is a nice way to implement the parity operation if dim is odd.
   **/
  Vector& mirror()		{return operator*=(*this, -1);}

  // Specialized 2D/3D stuff starts here

  // The following functions are defined only for
  // two dimensional (rotate(CoordType), Vector<>(CoordType, CoordType))
  // and three dimensional (the rest of them) vectors.
  // Attempting to call these on any other vector will
  // result in a linker error.

  /// 2D only: construct a vector from (x, y) coordinates
  Vector(CoordType x, CoordType y);
  /// 3D only: construct a vector from (x, y, z) coordinates
  Vector(CoordType x, CoordType y, CoordType z);

  /// 2D only: rotate a vector by an angle theta
  Vector& rotate(CoordType theta);

  /// 3D only: rotate a vector about the x axis by an angle theta
  Vector& rotateX(CoordType theta);
  /// 3D only: rotate a vector about the y axis by an angle theta
  Vector& rotateY(CoordType theta);
  /// 3D only: rotate a vector about the z axis by an angle theta
  Vector& rotateZ(CoordType theta);

  /// 3D only: rotate a vector about the i'th axis by an angle theta
  Vector& rotate(const Vector& axis, CoordType theta);
  /// 3D only: rotate a vector using a Quaternion
  Vector& rotate(const Quaternion& q);

  // Label the first three components of the vector as (x,y,z) for
  // 2D/3D convienience

  /// Access the first component of a vector
  CoordType x() const	{return m_elem[0];}
  /// Access the first component of a vector
  CoordType& x()	{return m_elem[0];}
  /// Access the second component of a vector
  CoordType y() const	{return m_elem[1];}
  /// Access the second component of a vector
  CoordType& y()	{return m_elem[1];}
  /// Access the third component of a vector
  CoordType z() const	{return m_elem[2];}
  /// Access the third component of a vector
  CoordType& z()	{return m_elem[2];}

  /// Flip the x component of a vector
  Vector& mirrorX()	{return mirror(0);}
  /// Flip the y component of a vector
  Vector& mirrorY()	{return mirror(1);}
  /// Flip the z component of a vector
  Vector& mirrorZ()	{return mirror(2);}

  /// 2D only: construct a vector from polar coordinates
  Vector& polar(CoordType r, CoordType theta);
  /// 2D only: convert a vector to polar coordinates
  void asPolar(CoordType& r, CoordType& theta) const;

  /// 3D only: construct a vector from polar coordinates
  Vector& polar(CoordType r, CoordType theta, CoordType z);
  /// 3D only: convert a vector to polar coordinates
  void asPolar(CoordType& r, CoordType& theta, CoordType& z) const;
  /// 3D only: construct a vector from shperical coordinates
  Vector& spherical(CoordType r, CoordType theta, CoordType phi);
  /// 3D only: convert a vector to shperical coordinates
  void asSpherical(CoordType& r, CoordType& theta, CoordType& phi) const;

  // FIXME make Cross() a friend function, and make this private
  double _scaleEpsilon(const Vector& v, double epsilon = WFMATH_EPSILON) const
  {return _ScaleEpsilon(m_elem, v.m_elem, dim, epsilon);}

  const CoordType* elements() const {return m_elem;}

 private:
  CoordType m_elem[dim];
  bool m_valid;
};

/// 2D only: get the z component of the cross product of two vectors
CoordType Cross(const Vector<2>& v1, const Vector<2>& v2);
/// 3D only: get the cross product of two vectors
Vector<3> Cross(const Vector<3>& v1, const Vector<3>& v2);

/// Check if two vectors are parallel
/**
 * Returns true if the vectors are parallel. For parallel
 * vectors, same_dir is set to true if they point the same
 * direction, and false if they point opposite directions
 **/
template<const int dim>
bool Parallel(const Vector<dim>& v1, const Vector<dim>& v2, bool& same_dir);

/// Check if two vectors are parallel
/**
 * Convienience wrapper if you don't care about same_dir
 **/
template<const int dim>
bool Parallel(const Vector<dim>& v1, const Vector<dim>& v2);

/// Check if two vectors are perpendicular
template<const int dim>
bool Perpendicular(const Vector<dim>& v1, const Vector<dim>& v2);

} // namespace WFMath

#endif // WFMATH_VECTOR_H
