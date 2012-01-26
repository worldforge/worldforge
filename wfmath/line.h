// line.h (A segmented line in n-dimensional space)
//
//  The WorldForge Project
//  Copyright (C) 2012  The WorldForge Project
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

#ifndef WFMATH_LINE_H
#define WFMATH_LINE_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/rotmatrix.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/intersect_decls.h>

#include <vector>

namespace WFMath {

/// A dim dimensional line
/**
 * This class implements the full shape interface, as described in
 * the fake class Shape.
 **/
template<int dim = 3>
class Line
{
 public:
  ///
  Line() {}
  ///
  Line(const Line<dim>& s) {}
  ///
  explicit Line(const AtlasInType& a);
  ///
  ~Line() {}

  /// Create an Atlas object from the line
  AtlasOutType toAtlas() const;
  /// Set the line's value to that given by an Atlas object
  void fromAtlas(const AtlasInType& a);

  ///
  Line& operator=(const Line& a);

  /// generic: check if two classes are equal, up to a given tolerance
  bool isEqualTo(const Line& s, double tolerance = WFMATH_EPSILON) const;
  /// generic: check if two classes are equal, up to tolerance WFMATH_EPSILON
  bool operator==(const Line& s) const	{return isEqualTo(s);}
  /// generic: check if two classes are not equal, up to tolerance WFMATH_EPSILON
  bool operator!=(const Line& s) const	{return !isEqualTo(s);}

  /// generic: returns true if the class instance has been initialized
  bool isValid() const {return m_points.size() > 1;}

  // Now we begin with the functions in the shape interface

  // Descriptive characteristics

  /// shape: return the number of corners in the shape.
  /**
   * For many shape classes, this is a fixed constant
   **/
  size_t numCorners() const {return m_points.size();}
  /// shape: return the position of the i'th corner, where 0 <= i < numCorners()
  Point<dim> getCorner(size_t i) const {return m_points[i];}
  /// shape: return the position of the center of the shape
  Point<dim> getCenter() const {return Barycenter(m_points);}

  // Add before i'th corner, zero is beginning, numCorners() is end
  bool addCorner(int i, const Point<dim>& p, double = WFMATH_EPSILON)
  {m_points.insert(m_points.begin() + i, p); return true;}

  // Remove the i'th corner
  void removeCorner(int i) {m_points.erase(m_points.begin() + i);}

  // Movement functions

  /// shape: move the shape by an amount given by the Vector v
  Line& shift(const Vector<dim>& v); // Move the shape a certain distance
  /// shape: move the shape, moving the given corner to the Point p
  /**
   * The corner in question is getCorner(corner).
   **/
  Line& moveCornerTo(const Point<dim>& p, size_t corner)
	{return shift(p - getCorner(corner));}
  /// shape: move the shape, moving the center to the Point p
  /**
   * The center is defined by getCenter()
   **/
  Line& moveCenterTo(const Point<dim>& p)
	{return shift(p - getCenter());}


  /// shape: rotate the shape while holding the given corner fixed
  /**
   * The corner in question is getCorner(corner).
   **/
  Line& rotateCorner(const RotMatrix<dim>& m, size_t corner)
	{return rotatePoint(m, getCorner(corner));}
  /// shape: rotate the shape while holding the center fixed
  /**
   * The center is defined by getCenter()
   **/
  Line& rotateCenter(const RotMatrix<dim>& m)
	{return rotatePoint(m, getCenter());}
  /// shape: rotate the shape while holding the Point p fixed.
  /**
   * Note that p can be any Point, it does not have to lie within
   * the shape.
   **/
  Line& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p);

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

 private:
  std::vector<Point<dim> > m_points;
  typedef typename std::vector<Point<dim> >::iterator iterator;
  typedef typename std::vector<Point<dim> >::const_iterator const_iterator;
  typedef typename std::vector<Point<dim> >::size_type size_type;
};

} // namespace WFMath

#endif  // WFMATH_LINE_H
