// -*-C++-*-
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

#include <math.h>
#include <wfmath/const.h>
#include <wfmath/string_funcs.h>

namespace WF { namespace Math {

template<const int size> class RotMatrix;
template<const int len> class Vector;
template<const int dim> class Point;

template<const int len>
Vector<len> operator*(const double& d, const Vector<len>& v);
template<const int len>
double Dot(const Vector<len>& v1, const Vector<len>& v2);

template<const int len>
double Angle(const Vector<len>& v, const Vector<len>& u);

// The following are defined in matrix_funcs.h
template<const int size> // m * v
Vector<size> Prod(const RotMatrix<size>& m, const Vector<size>& v);
template<const int size> // m^-1 * v
Vector<size> InvProd(const RotMatrix<size>& m, const Vector<size>& v);
template<const int size> // v * m
Vector<size> Prod(const Vector<size>& v, const RotMatrix<size>& m);
template<const int size> // v * m^-1
Vector<size> ProdInv(const Vector<size>& v, const RotMatrix<size>& m);

template<const int len>
Vector<len> operator-(const Point<len>& c1, const Point<len>& c2);
template<const int len>
Point<len> operator+(const Point<len>& c, const Vector<len>& v);
template<const int len>
Point<len> operator-(const Point<len>& c, const Vector<len>& v);
template<const int len>
Point<len> operator+(const Vector<len>& v, const Point<len>& c);

const double SloppyMagMaxTable[] = {1, 1,
  1.082392200292393968799446410733,
  1.145934719303161490541433900265,
};

// Note for people trying to compute the above numbers
// more accurately:

// The worst value for len == 2 occurs when the ratio of the components
// of the vector is sqrt(2) - 1. The value above is equal to sqrt(4 - 2 * sqrt(2)).

// The worst value for len == 3 occurs when the two smaller components
// are equal, and their ratio with the large component is the
// (unique, real) solution to the equation q x^3 + (q-1) x + p == 0,
// where p = sqrt(2) - 1, and q = sqrt(3) + 1 - 2 * sqrt(2).
// Running the script bc_sloppy_mag_3 provided with the WFMath source
// will calculate the above number.

#define SLOPPY_MAG_MAX(len) ((len) < sizeof(SloppyMagMaxTable)/sizeof(double) \
				   ? SloppyMagMaxTable[len] : 0)

const double SloppyMagMaxSqrtTable[] = {1, 1,
  1.040380795811030899095785063701,
  1.070483404496847625250328653179,
};

#define SLOPPY_MAG_MAX_SQRT(len) ((len) < sizeof(SloppyMagMaxSqrtTable)/sizeof(double) \
				        ? SloppyMagMaxSqrtTable[len] : 0)

template<const int len>
class Vector {
 public:
  Vector() {}
  Vector(const Vector<len>& v);

  // Non-virtual destructor, does nothing, no need to write it

  std::string toString() const		{return StringFromCoordList(m_elem, len);}
  bool fromString(const std::string& s) {return StringToCoordList(s, m_elem, len);}

  Vector<len>& operator=(const double d[len]);
  Vector<len>& operator=(const Vector<len>& v);

  bool operator==(const Vector<len>& v) const;
  bool operator!=(const Vector<len>& v) const {return !(*this == v);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the vector.
  bool operator< (const Vector<len>& v) const;

  Vector<len> operator+(const Vector<len>& v) const;
  Vector<len> operator-(const Vector<len>& v) const;
  Vector<len> operator*(const double& d) const;
  Vector<len> operator/(const double& d) const;

  Vector<len> operator-() const; // Unary minus

  Vector<len>& operator+=(const Vector<len>& v);
  Vector<len>& operator-=(const Vector<len>& v);
  Vector<len>& operator*=(const double& d);
  Vector<len>& operator/=(const double& d);

// FIXME this has problems
//  friend Vector<len> operator*<len>(const double& d, const Vector<len>& v);

  friend Vector<len> Prod<len>		(const RotMatrix<len>& m,
					 const Vector<len>& v);
  friend Vector<len> InvProd<len>	(const RotMatrix<len>& m,
					 const Vector<len>& v);

// FIXME same problem as operator*
//  friend Vector<len> operator-<len> (const Point<len>& c1, const Point<len>& c2);
//  friend Point<len> operator+<len> (const Point<len>& c, const Vector<len>& v);
//  friend Point<len> operator-<len> (const Point<len>& c, const Vector<len>& v);
//  friend Point<len> operator+<len> (const Vector<len>& v, const Point<len>& c);

  friend double Dot<len>(const Vector<len>& v1, const Vector<len>& v2);
  friend double Angle<len>(const Vector<len>& v, const Vector<len>& u);

  // Don't do range checking, it'll slow things down, and people
  // should be able to figure it out on their own
  const CoordType& operator[](const int i) const	{return m_elem[i];}
  CoordType& operator[](const int i)		{return m_elem[i];}

  double sqrMag() const;
  double mag() const		{return sqrt(sqrMag());}
  Vector<len>& normalize(double norm) {return (*this *= norm / mag());}

  // The sloppyMag() function gives a value between
  // the true magnitude and sloppyMagMax multiplied by the
  // true magnitude. sloppyNorm() uses sloppyMag() to normalize
  // the vector. This is currently only implemented for
  // len = {1, 2, 3}. For all current implementations,
  // sloppyMagMax is greater than or equal to one.
  // The constant sloppyMagMaxSqrt is provided for those
  // who want to most closely approximate the true magnitude,
  // without carring whether it's too low or too high.
  double sloppyMag() const;
  Vector<len>& sloppyNorm(double norm);
  // Can't seem to implement these as constants, implementing
  // inline lookup functions instead.
  static const double sloppyMagMax() {return SLOPPY_MAG_MAX(len);}
  static const double sloppyMagMaxSqrt() {return SLOPPY_MAG_MAX_SQRT(len);}

  Vector<len>& zero();

  // Rotate the vector in the (axis1,axis2) plane by the angle theta

  Vector<len>& rotate(int axis1, int axis2, double theta);

  // Same thing, but the axes are defined by two vectors. If the
  // vectors are parallel, this throws a BadRotationPlane error.

  Vector<len>& rotate(const Vector<len>& v1, const Vector<len>& v2, double theta);

  // Specialized 2D/3D stuff starts here

  // The following functions are defined only for
  // two dimensional (rotate(double), Vector<>(double, double))
  // and three dimensional (the rest of them) vectors.
  // Attempting to call these on any other vector will
  // result in a linker error.

  Vector(const CoordType& x, const CoordType& y);
  Vector(const CoordType& x, const CoordType& y, const CoordType& z);

  Vector<2>& rotate(double theta);

  Vector<3>& rotateX(double theta);
  Vector<3>& rotateY(double theta);
  Vector<3>& rotateZ(double theta);

  Vector<3>& rotate(const Vector<3>& axis, double theta);

  // Label the first three components of the vector as (x,y,z) for
  // 2D/3D convienience

  const CoordType& x() const	{return m_elem[0];}
  CoordType& x()			{return m_elem[0];}
  const CoordType& y() const	{return m_elem[1];}
  CoordType& y()			{return m_elem[1];}
  const CoordType& z() const	{return m_elem[2];}
  CoordType& z()			{return m_elem[2];}

 private:
  CoordType m_elem[len];
};

template<> inline double Vector<1>::sloppyMag() const	{return fabs(m_elem[0]);}
template<> double Vector<2>::sloppyMag() const;
template<> double Vector<3>::sloppyMag() const;

template<> inline Vector<2>::Vector(const CoordType& x, const CoordType& y)
	{m_elem[0] = x; m_elem[1] = y;}
template<> inline Vector<3>::Vector(const CoordType& x, const CoordType& y,
				    const CoordType& z)
	{m_elem[0] = x; m_elem[1] = y; m_elem[2] = z;}

template<> inline Vector<2>& Vector<2>::rotate(double theta)
	{return rotate(0, 1, theta);}

template<> inline Vector<3>& Vector<3>::rotateX(double theta)
	{return rotate(1, 2, theta);}
template<> inline Vector<3>& Vector<3>::rotateY(double theta)
	{return rotate(2, 0, theta);}
template<> inline Vector<3>& Vector<3>::rotateZ(double theta)
	{return rotate(0, 1, theta);}

inline double Cross(const Vector<2>& v1, const Vector<2>& v2)
	{return v1[0] * v2[1] - v2[0] * v1[1];}
Vector<3> Cross(const Vector<3>& v1, const Vector<3>& v2);

}} // namespace WF::Math

#endif // WFMATH_VECTOR_H
