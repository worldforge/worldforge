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

// DESCRIPTION:
// A single coordinate
//

// This class borrows heavily from the base shape class in libCoal,
// plus certain intersection ideas from stage/shepherd/sylvanus


#ifndef WFMATH_SHAPE_H
#define WFMATH_SHAPE_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/matrix.h>

namespace WF { namespace Math {

template<const int dim> class AxisBox;

template<const int dim>
class Shape
{
 public:
  Shape() {m_center.origin();}
  Shape(const Point<dim>& p) : m_center(p) {}
  Shape(const Shape<dim>& s) : m_center(s.m_center) {}

  virtual ~Shape() {}

  virtual std::string toString() const = 0;
  // There's no virtual fromString() method, because using it requires
  // knowing what sort of data to pass, and that's class dependent.
  // The utility in the virtual toString() is not having to write
  // operator<<() separately for each derived class.

  // Can't assign different types of shapes to one another,
  // no operator=() in the base class.

#ifndef WFMATH_NO_DYNAMIC_CAST
  virtual bool isEqualTo(const Shape<dim>& s,
			 double tolerance = WFMATH_EPSILON) const = 0;
  bool operator==(const Shape<dim>& s) const	{return isEqualTo(s);}
  bool operator!=(const Shape<dim>& s) const	{return !isEqualTo(s);}
#endif // WFMATH_NO_DYNAMIC_CAST

  // Without an operator=(), you can't make a map<Shape<dim>, foo>, and so
  // we don't need an operator<().

  // Descriptive characteristics

  virtual int numCorners() const = 0;
  virtual Point<dim> getCorner(int i) const = 0;
  Point<dim> getCenter() const {return m_center;}

  // Movement functions

  virtual Shape<dim>& shift(const Vector<dim>& v) = 0;
  Shape<dim>& moveCornerTo(const Point<dim>& p, int corner)
	{return shift(p - getCorner(corner));}
  Shape<dim>& moveCenterTo(const Point<dim>& p)
	{return shift(p - m_center);}

  // Intersection functions

  virtual AxisBox<dim> boundingBox() const = 0;
  virtual AxisBox<dim> enclosedBox() const = 0;

  // The following functions check the intersection of
  // the shape and a passed Point<>, AxisBox<>, etc,. of
  // appripriate dimension. The difference between contains()
  // and containsProper() is the difference between
  // < and <=. That is, given an AxisBox a, a.contains(a)
  // will return true, but a.containsProper(a) will return
  // false, since the intersection of the boundaries of the two
  // boxes means that the containment is not "proper". This
  // is similar to, but not quite the same as, the difference
  // between a subset and a proper subset in set theory.

  virtual bool contains(const Point<dim>& p) const = 0;
  virtual bool containsProper(const Point<dim>& p) const = 0;
  virtual bool contains(const AxisBox<dim>& b) const = 0;
  virtual bool containsProper(const AxisBox<dim>& b) const = 0;
  virtual bool isContainedBy(const AxisBox<dim>& b) const = 0;
  virtual bool isContainedByProper(const AxisBox<dim>& b) const = 0;
  virtual bool intersects(const AxisBox<dim>& b) const = 0;

  // The following functions assume that the shape isContainedBy()
  // (or isContainedByProper()) the AxisBox in question. They determine
  // if the shape is wholy contained in one of the "quadrants" of the
  // AxisBox, where "quadrant" i is an AxisBox defined by the
  // points b.getCorner(i) and b.getCenter(). If this is true,
  // the corner index i is returned. If not, the functions return -1.

  // These are mainly here because they will be (FIXME) used by
  // stage/sys/shepherd/sylvanus.

  virtual int isSubContainedBy(const AxisBox<dim>& b) const = 0;
  virtual int isSubContainedByProper(const AxisBox<dim>& b) const = 0;

 protected:

  Point<dim> m_center;
};

template<const int dim>
class RotShape : public Shape<dim> // a shape that can be rotated.
{
 public:
  RotShape() {}
  RotShape(const Point<dim>& p) : Shape<dim>(p) {}
  RotShape(const RotShape<dim>& s) : Shape<dim>(s) {}

  virtual ~RotShape() {}

  // All this class does is add a few functions to the Shape<> interface

  // Rotate while holding a corner fixed
  RotShape<dim>& rotateCorner(const RotMatrix<dim>& m, int corner)
	{return rotatePoint(m, Shape<dim>::getCorner(corner));}
  // Rotate while holding the center fixed
  RotShape<dim>& rotateCenter(const RotMatrix<dim>& m)
	{return rotatePoint(m, Shape<dim>::m_center);}
  // Rotate while holding a certain point fixed
  virtual RotShape<dim>& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p) = 0;
};

}} // namespace WF::Math

#endif  // WFMATH_SHAPE_H
