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

namespace WFMath {

template<const int dim> class RotMatrix;
template<const int dim> class Vector;
template<const int dim> class Point;
class Quaternion;

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
Vector<dim>& operator+=(Vector<dim>& v1, const Vector<dim>& v2);
template<const int dim>
Vector<dim>& operator-=(Vector<dim>& v1, const Vector<dim>& v2);
template<const int dim>
Vector<dim>& operator*=(Vector<dim>& v, CoordType d);
template<const int dim>
Vector<dim>& operator/=(Vector<dim>& v, CoordType d);

template<const int dim>
CoordType Dot(const Vector<dim>& v1, const Vector<dim>& v2);

template<const int dim>
CoordType Angle(const Vector<dim>& v, const Vector<dim>& u);

// The following are defined in rotmatrix_funcs.h
template<const int dim> // m * v
Vector<dim> Prod(const RotMatrix<dim>& m, const Vector<dim>& v);
template<const int dim> // m^-1 * v
Vector<dim> InvProd(const RotMatrix<dim>& m, const Vector<dim>& v);
template<const int dim> // v * m
Vector<dim> Prod(const Vector<dim>& v, const RotMatrix<dim>& m);
template<const int dim> // v * m^-1
Vector<dim> ProdInv(const Vector<dim>& v, const RotMatrix<dim>& m);

template<const int dim>
Vector<dim> operator*(const RotMatrix<dim>& m, const Vector<dim>& v);
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

template<const int dim>
class Vector {
 public:
  Vector() {}
  Vector(const Vector& v);
  explicit Vector(const Atlas::Message::Object& a) {fromAtlas(a);}

  ~Vector() {}

  friend std::ostream& operator<< <dim>(std::ostream& os, const Vector& v);
  friend std::istream& operator>> <dim>(std::istream& is, Vector& v);

  Atlas::Message::Object toAtlas() const;
  void fromAtlas(const Atlas::Message::Object& a);

  Vector& operator=(const Vector& v);

  bool isEqualTo(const Vector& v, double epsilon = WFMATH_EPSILON) const;

  bool operator==(const Vector& v) const {return isEqualTo(v);}
  bool operator!=(const Vector& v) const {return !isEqualTo(v);}

  Vector& zero();

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the vector.
  bool operator< (const Vector& v) const;

  // Math operators

  friend Vector& operator+=<dim>(Vector& v1, const Vector& v2);
  friend Vector& operator-=<dim>(Vector& v1, const Vector& v2);
  friend Vector& operator*=<dim>(Vector& v, CoordType d);
  friend Vector& operator/=<dim>(Vector& v, CoordType d);

  friend Vector operator+<dim>(const Vector& v1, const Vector& v2);
  friend Vector operator-<dim>(const Vector& v1, const Vector& v2);
  friend Vector operator-<dim>(const Vector& v); // Unary minus
  friend Vector operator*<dim>(CoordType d, const Vector& v);
  friend Vector operator*<dim>(const Vector& v, CoordType d);
  friend Vector operator/<dim>(const Vector& v, CoordType d);

  friend Vector Prod<dim>	(const RotMatrix<dim>& m,
				 const Vector& v);
  friend Vector InvProd<dim>	(const RotMatrix<dim>& m,
				 const Vector& v);

  // Don't do range checking, it'll slow things down, and people
  // should be able to figure it out on their own
  CoordType operator[](const int i) const {assert(0 <= i && i < dim); return m_elem[i];}
  CoordType& operator[](const int i)	  {assert(0 <= i && i < dim); return m_elem[i];}

  friend Vector operator-<dim>(const Point<dim>& c1, const Point<dim>& c2);
  friend Point<dim> operator+<dim>(const Point<dim>& c, const Vector& v);
  friend Point<dim> operator-<dim>(const Point<dim>& c, const Vector& v);
  friend Point<dim> operator+<dim>(const Vector& v, const Point<dim>& c);

  friend Point<dim>& operator+=<dim>(Point<dim>& p, const Vector& rhs);
  friend Point<dim>& operator-=<dim>(Point<dim>& p, const Vector& rhs);

  friend CoordType Dot<dim>(const Vector& v1, const Vector& v2);
  friend CoordType Angle<dim>(const Vector& v, const Vector& u);

  CoordType sqrMag() const;
  CoordType mag() const		{return sqrt(sqrMag());}
  Vector& normalize(CoordType norm = 1.0)
	{CoordType themag = mag(); assert(themag > 0); return (*this *= norm / themag);}

  // The sloppyMag() function gives a value between
  // the true magnitude and sloppyMagMax multiplied by the
  // true magnitude. sloppyNorm() uses sloppyMag() to normalize
  // the vector. This is currently only implemented for
  // dim = {1, 2, 3}. For all current implementations,
  // sloppyMagMax is greater than or equal to one.
  // The constant sloppyMagMaxSqrt is provided for those
  // who want to most closely approximate the true magnitude,
  // without caring whether it's too low or too high.

  CoordType sloppyMag() const;
  Vector& sloppyNorm(CoordType norm);

  // Can't seem to implement these as constants, implementing
  // inline lookup functions instead.
  static const CoordType sloppyMagMax();
  static const CoordType sloppyMagMaxSqrt();

  // Rotate the vector in the (axis1,axis2) plane by the angle theta

  Vector& rotate(int axis1, int axis2, CoordType theta);

  // Same thing, but the axes are defined by two vectors. If the
  // vectors are parallel, this throws a ColinearVectors error.

  Vector& rotate(const Vector& v1, const Vector& v2, CoordType theta);

  // mirror image functions

  Vector& mirror(const int i) {assert(0 <= i && i < dim); m_elem[i] *= -1; return *this;}
  Vector& mirror(const Vector& v)
	{return operator-=(2 * v * Dot(v, *this) / v.sqrMag());}
  Vector& mirror()		{return operator*=(-1);}

  // Specialized 2D/3D stuff starts here

  // The following functions are defined only for
  // two dimensional (rotate(CoordType), Vector<>(CoordType, CoordType))
  // and three dimensional (the rest of them) vectors.
  // Attempting to call these on any other vector will
  // result in a linker error.

  Vector(CoordType x, CoordType y);
  Vector(CoordType x, CoordType y, CoordType z);

  Vector<2>& rotate(CoordType theta);

  Vector<3>& rotateX(CoordType theta);
  Vector<3>& rotateY(CoordType theta);
  Vector<3>& rotateZ(CoordType theta);

  Vector<3>& rotate(const Vector<3>& axis, CoordType theta);
  Vector<3>& rotate(const Quaternion& q);

  // Label the first three components of the vector as (x,y,z) for
  // 2D/3D convienience

  CoordType x() const	{assert(dim > 0); return m_elem[0];}
  CoordType& x()	{assert(dim > 0); return m_elem[0];}
  CoordType y() const	{assert(dim > 1); return m_elem[1];}
  CoordType& y()	{assert(dim > 1); return m_elem[1];}
  CoordType z() const	{assert(dim > 2); return m_elem[2];}
  CoordType& z()	{assert(dim > 2); return m_elem[2];}

  // Don't need asserts here, they're taken care of in the general function
  Vector& mirrorX()	{return mirror(0);}
  Vector& mirrorY()	{return mirror(1);}
  Vector& mirrorZ()	{return mirror(2);}

  Vector<2>& polar(CoordType r, CoordType theta);
  void asPolar(CoordType& r, CoordType& theta) const;

  Vector<3>& polar(CoordType r, CoordType theta, CoordType z);
  void asPolar(CoordType& r, CoordType& theta, CoordType& z) const;
  Vector<3>& spherical(CoordType r, CoordType theta, CoordType phi);
  void asSpherical(CoordType& r, CoordType& theta, CoordType& phi) const;

  // FIXME make Cross() a friend function, and make this private
  double _scaleEpsilon(const Vector& v, double epsilon = WFMATH_EPSILON) const
	{return _ScaleEpsilon(m_elem, v.m_elem, dim, epsilon);}

 private:
  CoordType m_elem[dim];
};

CoordType Cross(const Vector<2>& v1, const Vector<2>& v2);
Vector<3> Cross(const Vector<3>& v1, const Vector<3>& v2);

template<const int dim>
bool Parallel(const Vector<dim>& v1, const Vector<dim>& v2);
template<const int dim>
bool Perpendicular(const Vector<dim>& v1, const Vector<dim>& v2);

} // namespace WFMath

#include <wfmath/vector_funcs.h>

#endif // WFMATH_VECTOR_H
