// shape.h (A general base class for shapes)
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
//

// Author: Ron Steinke

// This class borrows heavily from the base shape class in libCoal,
// plus certain intersection ideas from stage/shepherd/sylvanus


#ifndef WFMATH_SHAPE_H
#define WFMATH_SHAPE_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/rotmatrix.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/intersect_decls.h>

namespace WFMath {

/// A fake class which documents the generic parts of the WFMath interface
/**
 * This fake class documents two parts of the WFMath generic
 * class interface. With a few exceptions (e.g. classes derived
 * from std::exception), every class in WFMath implements
 * the part of the interface labeled as 'generic'. The 'shape'
 * interface is implemented by several classes, which identify
 * themselves in their own documentation. Every class which
 * implements the 'shape' interface also implements the 'generic'
 * interface. Classes will not generally document their
 * generic and shape interface functions.
 **/
template<const int dim>
class Shape
{
 public:
  // The first things in the Shape class are the functions required
  // by CLASS_LAYOUT for all classes

  ///
  Shape() {}
  ///
  Shape(const Shape<dim>& s) {}
  ///
  ~Shape() {}

  /// generic: Print an instance to a stream
  friend std::ostream& operator<< <dim>(std::ostream& os, const Shape& s);
  /// generic: Parse an instance from a stream
  friend std::istream& operator>> <dim>(std::istream& is, Shape& s);

  ///
  Shape& operator=(const Shape& a);

  /// generic: check if two classes are equal, up to a given tolerance
  bool isEqualTo(const Shape& s, double tolerance = WFMATH_EPSILON) const;
  /// generic: check if two classes are equal, up to tolerance WFMATH_EPSILON
  bool operator==(const Shape& s) const	{return isEqualTo(s);}
  /// generic: check if two classes are not equal, up to tolerance WFMATH_EPSILON
  bool operator!=(const Shape& s) const	{return !isEqualTo(s);}

  /// generic: returns true if the class instance has been initialized
  bool isValid() const {return m_valid;}

  // Now we begin with the functions in the shape interface

  // Descriptive characteristics

  /// shape: return the number of corners in the shape.
  /**
   * For many shape classes, this is a fixed constant
   **/
  size_t numCorners() const; // The number of corners, returns zero for Ball<>
  /// shape: return the position of the i'th corner, where 0 <= i < numCorners()
  Point<dim> getCorner(size_t i) const; // Must have i >= 0 && i < numCorners()
  /// shape: return the position of the center of the shape
  Point<dim> getCenter() const; // Returns the barycenter of the object

  // Movement functions

  /// shape: move the shape by an amount given by the Vector v
  Shape& shift(const Vector<dim>& v); // Move the shape a certain distance
  /// shape: move the shape, moving the given corner to the Point p
  /**
   * The corner in question is getCorner(corner).
   **/
  Shape& moveCornerTo(const Point<dim>& p, size_t corner)
	{return shift(p - getCorner(corner));}
  /// shape: move the shape, moving the center to the Point p
  /**
   * The center is defined by getCenter()
   **/
  Shape& moveCenterTo(const Point<dim>& p)
	{return shift(p - getCenter());}


  /// shape: rotate the shape while holding the given corner fixed
  /**
   * The corner in question is getCorner(corner).
   **/
  Shape& rotateCorner(const RotMatrix<dim>& m, size_t corner)
	{return rotatePoint(m, getCorner(corner));}
  /// shape: rotate the shape while holding the center fixed
  /**
   * The center is defined by getCenter()
   **/
  Shape& rotateCenter(const RotMatrix<dim>& m)
	{return rotatePoint(m, getCenter());}
  /// shape: rotate the shape while holding the Point p fixed.
  /**
   * Note that p can be any Point, it does not have to lie within
   * the shape.
   **/
  Shape& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p);

  // Intersection functions

  /// shape: return the minimal axis-aligned bounding box
  AxisBox<dim> boundingBox() const;
  /// shape: return the minimal bounding sphere
  Ball<dim> boundingSphere() const;
  /// shape: return an approximate bounding sphere, guaranteed
  /// to contain the minimal bounding sphere
  /**
   * boundingSphereSloppy() uses
   * SloppyDistance() instead of Distance() to calculate it's
   * radius, except in cases like Point<> and Ball<> where it
   * would be silly. Thus, the result of boundingSphereSloppy()
   * is guaranteed to contain the result of boundingSphere().
   **/
  Ball<dim> boundingSphereSloppy() const;

  /// shape: Returns true if the two shapes intersect.
  /**
   * If the 'proper' argument is true, shapes which only touch on their
   * boundary do not count as intersecting. If it is false, they do.
   * This function is symmetric in its first two arguments
   * (Intersect(a, b, proper) == Intersect(b, a, proper)).
   * The two shapes do
   * not have to be the same class, but must have the same dimension.
   **/
  friend bool Intersect<dim>(Shape<dim>& s1, Shape<dim>& s2, bool proper);
  /// shape: Returns true if the first shape contains the second.
  /**
   * If the 'proper' argument is true, the inner shape is not contained
   * if it touches the boundary of the outer shape. Otherwise, it
   * does. Therefore, any shape contains itself
   * (Contains(foo, foo, false) == true), but no shape contains itself
   * properly (Contains(foo, foo, true) == false). Because of this,
   * an empty shape (e.g. a Polygon with zero corners)
   * is properly contained by any other shape. A Point, or any single
   * point shape (e.g. a Segment where the endpoints are equal)
   * properly contains an empty shape, and contains (but not properly)
   * any other single point shape which occupies the same point.
   * The two shapes do
   * not have to be the same class, but must have the same dimension.
   **/
  friend bool Contains<dim>(Shape<dim>& s1, Shape<dim>& s2, bool proper);

 private:
  bool m_valid;
};

//#include<wfmath/shape_funcs.h>

} // namespace WFMath

#endif  // WFMATH_SHAPE_H
