// point.h (point class copied from libCoal, subsequently modified)
//
//  The WorldForge Project
//  Copyright (C) 2000, 2001, 2002  The WorldForge Project
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
//

// Author: Ron Steinke

#ifndef WFMATH_POINT_H
#define WFMATH_POINT_H

#include <wfmath/const.h>

#include <memory>
#include <iosfwd>

#include <cmath>

namespace WFMath {

template<int dim>
Point<dim>& operator+=(Point<dim>& p, const Vector<dim>& v);
template<int dim>
Point<dim>& operator-=(Point<dim>& p, const Vector<dim>& v);

template<int dim>
Vector<dim> operator-(const Point<dim>& c1, const Point<dim>& c2);
template<int dim>
Point<dim> operator+(const Point<dim>& c, const Vector<dim>& v);
template<int dim>
Point<dim> operator+(const Vector<dim>& v, const Point<dim>& c);
template<int dim>
Point<dim> operator-(const Point<dim>& c, const Vector<dim>& v);

template<int dim>
CoordType SquaredDistance(const Point<dim>& p1, const Point<dim>& p2);
template<int dim>
CoordType Distance(const Point<dim>& p1, const Point<dim>& p2)
  {return std::sqrt(SquaredDistance(p1, p2));}
template<int dim>
CoordType SloppyDistance(const Point<dim>& p1, const Point<dim>& p2)
  {return (p1 - p2).sloppyMag();}

/// Find the center of a set of points, all weighted equally
template<int dim, template<class, class> class container>
Point<dim> Barycenter(const container<Point<dim>, std::allocator<Point<dim> > >& c);
/// Find the center of a set of points with the given weights
/**
 * If the number of points and the number of weights are not equal,
 * the excess of either is ignored. The weights (or that subset
 * which is used, if there are more weights than points), must not
 * sum to zero.
 **/
template<int dim, template<class, class> class container,
      template<class, class> class container2>
Point<dim> Barycenter(const container<Point<dim>, std::allocator<Point<dim> > >& c,
          const container2<CoordType, std::allocator<CoordType> >& weights);

// This is used a couple of places in the library
template<int dim>
Point<dim> Midpoint(const Point<dim>& p1, const Point<dim>& p2,
        CoordType dist = 0.5);

template<int dim>
std::ostream& operator<<(std::ostream& os, const Point<dim>& m);
template<int dim>
std::istream& operator>>(std::istream& is, Point<dim>& m);

template<typename Shape>
class ZeroPrimitive;

/// A dim dimensional point
/**
 * This class implements the full shape interface, as described in
 * the fake class Shape.
 **/
template<int dim = 3>
class Point
{
 friend class ZeroPrimitive<Point<dim> >;
 public:
  /// Construct an uninitialized point
  Point () : m_valid(false) {}
  /// Construct a copy of a point
  Point (const Point& p);
  /// Construct a point from an object passed by Atlas
  explicit Point (const AtlasInType& a);
  /// Construct a point from a vector.
  explicit Point(const Vector<dim>& vector);

  /**
   * @brief Provides a global instance preset to zero.
   */
  static const Point<dim>& ZERO();

  friend std::ostream& operator<< <dim>(std::ostream& os, const Point& p);
  friend std::istream& operator>> <dim>(std::istream& is, Point& p);

  /// Create an Atlas object from the point
  AtlasOutType toAtlas() const;
  /// Set the point's value to that given by an Atlas object
  void fromAtlas(const AtlasInType& a);

  Point& operator= (const Point& rhs);

  bool isEqualTo(const Point &p, CoordType epsilon = numeric_constants<CoordType>::epsilon()) const;
  bool operator== (const Point& rhs) const	{return isEqualTo(rhs);}
  bool operator!= (const Point& rhs) const	{return !isEqualTo(rhs);}

  bool isValid() const {return m_valid;}
  /// make isValid() return true if you've initialized the point by hand
  void setValid(bool valid = true) {m_valid = valid;}

  /// Set point to (0,0,...,0)
  Point& setToOrigin();

  // Operators

  // Documented in vector.h
  friend Vector<dim> operator-<dim>(const Point& c1, const Point& c2);
  friend Point operator+<dim>(const Point& c, const Vector<dim>& v);
  friend Point operator-<dim>(const Point& c, const Vector<dim>& v);
  friend Point operator+<dim>(const Vector<dim>& v, const Point& c);

  friend Point& operator+=<dim>(Point& p, const Vector<dim>& rhs);
  friend Point& operator-=<dim>(Point& p, const Vector<dim>& rhs);

  /// Rotate about point p
  Point& rotate(const RotMatrix<dim>& m, const Point& p)
  {return (*this = p + Prod(*this - p, m));}

  // Functions so that Point<> has the generic shape interface

  size_t numCorners() const {return 1;}
  Point<dim> getCorner(size_t) const { return *this;}
  Point<dim> getCenter() const {return *this;}

  Point shift(const Vector<dim>& v) {return *this += v;}
  Point moveCornerTo(const Point& p, size_t)
  {return operator=(p);}
  Point moveCenterTo(const Point& p) {return operator=(p);}

  Point& rotateCorner(const RotMatrix<dim>&, size_t)
  {return *this;}
  Point& rotateCenter(const RotMatrix<dim>&) {return *this;}
  Point& rotatePoint(const RotMatrix<dim>& m, const Point& p) {return rotate(m, p);}

  // 3D rotation functions
  Point& rotate(const Quaternion& q, const Point& p);
  Point& rotateCorner(const Quaternion&, size_t)
  { return *this;}
  Point& rotateCenter(const Quaternion&) {return *this;}
  Point& rotatePoint(const Quaternion& q, const Point& p);

  // The implementations of these lie in axisbox_funcs.h and
  // ball_funcs.h, to reduce include dependencies
  AxisBox<dim> boundingBox() const;
  Ball<dim> boundingSphere() const;
  Ball<dim> boundingSphereSloppy() const;

  Point toParentCoords(const Point& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
  {return origin + (*this - Point().setToOrigin()) * rotation;}
  Point toParentCoords(const AxisBox<dim>& coords) const;
  Point toParentCoords(const RotBox<dim>& coords) const;

  // toLocal is just like toParent, expect we reverse the order of
  // translation and rotation and use the opposite sense of the rotation
  // matrix

  Point toLocalCoords(const Point& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
  {return Point().setToOrigin() + rotation * (*this - origin);}
  Point toLocalCoords(const AxisBox<dim>& coords) const;
  Point toLocalCoords(const RotBox<dim>& coords) const;

  // 3D only
  Point toParentCoords(const Point& origin, const Quaternion& rotation) const;
  Point toLocalCoords(const Point& origin, const Quaternion& rotation) const;

  // Member access

  /// Access the i'th coordinate of the point
  CoordType operator[](const int i) const {return m_elem[i];}
  /// Access the i'th coordinate of the point
  CoordType& operator[](const int i)	  {return m_elem[i];}

  /// Get the square of the distance from p1 to p2
  friend CoordType SquaredDistance<dim>(const Point& p1, const Point& p2);

// FIXME instatiation problem when declared as friend
//  template<template<class> class container>
//  friend Point Barycenter(const container<Point>& c);

  /// Find a point on the line containing p1 and p2, by default the midpoint
  /**
   * The default value of 0.5 for dist gives the midpoint. A value of 0 gives
   * p1, and 1 gives p2. Values of dist outside the [0, 1] range are allowed,
   * and give points on the line which are not on the segment bounded by
   * p1 and p2.
   **/
  friend Point<dim> Midpoint<dim>(const Point& p1, const Point& p2, CoordType dist);

  // 2D/3D stuff

  /// 2D only: construct a point from its (x, y) coordinates
  Point (CoordType x, CoordType y); // 2D only
  /// 3D only: construct a point from its (x, y, z) coordinates
  Point (CoordType x, CoordType y, CoordType z); // 3D only

  // Label the first three components of the vector as (x,y,z) for
  // 2D/3D convienience

  /// access the first component of a point
  CoordType x() const	{return m_elem[0];}
  /// access the first component of a point
  CoordType& x()	{return m_elem[0];}
  /// access the second component of a point
  CoordType y() const	{return m_elem[1];}
  /// access the second component of a point
  CoordType& y()	{return m_elem[1];}
  /// access the third component of a point
  CoordType z() const	{return m_elem[2];}
  /// access the third component of a point
  CoordType& z()	{return m_elem[2];}

  /// 2D only: construct a vector from polar coordinates
  Point& polar(CoordType r, CoordType theta);
  /// 2D only: convert a vector to polar coordinates
  void asPolar(CoordType& r, CoordType& theta) const;

  /// 3D only: construct a vector from polar coordinates
  Point& polar(CoordType r, CoordType theta, CoordType z);
  /// 3D only: convert a vector to polar coordinates
  void asPolar(CoordType& r, CoordType& theta, CoordType& z) const;
  /// 3D only: construct a vector from spherical coordinates
  Point& spherical(CoordType r, CoordType theta, CoordType phi);
  /// 3D only: convert a vector to spherical coordinates
  void asSpherical(CoordType& r, CoordType& theta, CoordType& phi) const;

  const CoordType* elements() const {return m_elem;}

 private:
  CoordType m_elem[dim];
  bool m_valid;
};

} // namespace WFMath

#endif  // WFMATH_POINT_H
