// -*-C++-*-
// ball.h (A n-dimensional ball)
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

// This class is called Ball<> instead of Sphere to be more in tune
// with the usual mathematical naming conventions, where a ball is
// a filled object, while a sphere is just the outer shell. It also
// helps that a Ball<n> corresponds to an n-ball, while a Shpere<n>
// would correspond to an (n-1)-sphere.

#ifndef WFMATH_BALL_H
#define WFMATH_BALL_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/shape.h>
#include <wfmath/axisbox.h>

namespace WF { namespace Math {

template<const int dim>
class Ball : public RotShape<dim>
{
 public:
  Ball() : m_radius(0) {}
  Ball(const Point<dim>& center, double radius)
	: RotShape<dim>(center), m_radius(radius) {}
  Ball(const Ball<dim>& b) : RotShape<dim>(b), m_radius(b.m_radius) {}

  virtual ~Ball() {}

  virtual std::string toString() const;
  bool fromString(const std::string& s);

  Ball<dim>& operator=(const Ball<dim>& b);

#ifndef WFMATH_NO_DYNAMIC_CAST
  virtual bool isEqualTo(const Shape<dim>& s, double tolerance = WFMATH_EPSILON) const
    {
      const Ball<dim> *b = dynamic_cast<const Ball*>(&s);
      return (b && isEqualTo(*b, tolerance));
    }
#endif // WFMATH_NO_DYNAMIC_CAST
  bool isEqualTo(const Ball<dim>& b, double tolerance = WFMATH_EPSILON) const
	{return Shape<dim>::m_center.isEqualTo(b.Shape<dim>::m_center, tolerance)
	     && IsFloatEqual(m_radius, b.m_radius, tolerance);}

  bool operator==(const Ball<dim>& b) const	{return isEqualTo(b);}
  bool operator!=(const Ball<dim>& b) const	{return !isEqualTo(b);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the ball.
  bool operator< (const Ball<dim>& b) const;

  // Descriptive characteristics

  virtual int numCorners() const {return 0;}
  virtual Point<dim> getCorner(int i) const {assert(false);}

  void setCenter(const Point<dim>& p) {Shape<dim>::m_center = p;}

  const CoordType& radius() const {return m_radius;}
  CoordType& radius() {return m_radius;}

  // Movement functions

  virtual Shape<dim>& shift(const Vector<dim>& v)
	{Shape<dim>::m_center += v; return *this;}
  virtual RotShape<dim>& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p)
	{Shape<dim>::m_center.rotate(m, p); return *this;}

  // Intersection functions

  virtual AxisBox<dim> boundingBox() const;
  virtual AxisBox<dim> enclosedBox() const;

  virtual bool contains(const Point<dim>& p) const;
  virtual bool containsProper(const Point<dim>& p) const;
  virtual bool contains(const AxisBox<dim>& b) const;
  virtual bool containsProper(const AxisBox<dim>& b) const;
  virtual bool isContainedBy(const AxisBox<dim>& b) const;
  virtual bool isContainedByProper(const AxisBox<dim>& b) const;
  virtual bool intersects(const AxisBox<dim>& b) const;
  virtual bool intersectsProper(const AxisBox<dim>& b) const;
  virtual int isSubContainedBy(const AxisBox<dim>& b) const;
  virtual int isSubContainedByProper(const AxisBox<dim>& b) const;

 private:

  CoordType m_radius;
};

}} // namespace WF::Math

#include <wfmath/ball_funcs.h>

#endif  // WFMATH_BALL_H
