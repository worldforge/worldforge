// -*-C++-*-
// shape.h (A general base class for shapes)
//
//  The WorldForge Project
//  Copyright (C) 2000, 2001  The WorldForge Project
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

// This class borrows heavily from the base shape class in libCoal,
// plus certain intersection ideas from stage/shepherd/sylvanus


#ifndef WFMATH_SHAPE_H
#define WFMATH_SHAPE_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/matrix.h>
#include <wfmath/axisbox.h>
#include <wfmath/intersect_decls.h>

namespace WF { namespace Math {

template<const int dim>
class Shape
{
 public:
  // The first things in the Shape class are the functions required
  // by CLASS_LAYOUT for all classes

  Shape() {}
  Shape(const Shape<dim>& s) {}

  ~Shape() {}

  std::string toString() const;
  bool fromString(const std::string& s);

  Shape& operator=(const Shape& a);

  bool isEqualTo(const Shape& s, double tolerance = WFMATH_EPSILON) const;
  bool operator==(const Shape& s) const	{return isEqualTo(s);}
  bool operator!=(const Shape& s) const	{return !isEqualTo(s);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the shape.
  bool operator< (const Shape& a) const;

  // Now we begin with the functions in the shape interface

  // Descriptive characteristics

  int numCorners() const; // The number of corners, returns zero for Ball<>
  Point<dim> getCorner(int i) const; // Must have i >= 0 && i < numCorners()
  Point<dim> getCenter() const; // Returns the barycenter of the object

  // Movement functions

  Shape& shift(const Vector<dim>& v); // Move the shape a certain distance

  Shape& moveCornerTo(const Point<dim>& p, int corner)
	{return shift(p - getCorner(corner));}

  Shape& moveCenterTo(const Point<dim>& p)
	{return shift(p - getCenter());}


  // Rotate while holding a corner fixed
  Shape& rotateCorner(const RotMatrix<dim>& m, int corner)
	{return rotatePoint(m, getCorner(corner));}

  // Rotate while holding the center fixed
  Shape& rotateCenter(const RotMatrix<dim>& m)
	{return rotatePoint(m, getCenter());}

  // Rotate while holding a certain point fixed
  Shape& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p);

  // Intersection functions

  AxisBox<dim> boundingBox() const;

  // The following will be defined in intersect.h. The declarations
  // of the friend templates are placed in intersect_decls.h. The
  // difference between Intersect() and IntersectProper() is that
  // Intersect() returns true if only the boundaries of the shapes
  // intersect while IntersectProper() returns false. The same is
  // true of Contains() and ContainsProper(), e.g. an object
  // contains itself (Contains(foo, foo) == true), but does
  // not contain itself properly (ContainsProper(foo, foo) == false).
  // The Intersect() functions are symmetric in their arguments,
  // while for the Contains() functions the first shape is the outer
  // one and the second is the inner one.

  friend bool Intersect<dim>(Shape<dim>& s, Foo<dim>& f);
  friend bool IntersectProper<dim>(Shape<dim>& s, Foo<dim>& f);
  friend bool Contains<dim>(Shape<dim>& s, Foo<dim>& f);
  friend bool ContainsProper<dim>(Shape<dim>& s, Foo<dim>& f);
  friend bool Contains<dim>(Foo<dim& f, Shape<dim>& s);
  friend bool ContainsProper<dim>(Foo<dim& f, Shape<dim>& s);

};

}} // namespace WF::Math

#endif  // WFMATH_SHAPE_H
