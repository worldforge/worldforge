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

// Author: Ron Steinke

// This class is called Ball<> instead of Sphere to be more in tune
// with the usual mathematical naming conventions, where a ball is
// a filled object, while a sphere is just the outer shell. It also
// helps that a Ball<n> corresponds to an n-ball, while a Shpere<n>
// would correspond to an (n-1)-sphere.

#ifndef WFMATH_BALL_H
#define WFMATH_BALL_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/intersect_decls.h>

namespace WFMath {

template<const int dim> class Ball;

template<const int dim, template<class> class container>
Ball<dim> BoundingSphere(const container<Point<dim> >& c);
template<const int dim, template<class> class container>
Ball<dim> BoundingSphereSloppy(const container<Point<dim> >& c);

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Ball<dim>& m);
template<const int dim>
std::istream& operator>>(std::istream& is, Ball<dim>& m);

template<const int dim>
class Ball
{
 public:
  Ball() {}
  Ball(const Point<dim>& center, CoordType radius)
	: m_center(center), m_radius(radius) {}
  Ball(const Ball& b) : m_center(b.m_center), m_radius(b.m_radius) {}

  ~Ball() {}

  friend std::ostream& operator<< <dim>(std::ostream& os, const Ball& b);
  friend std::istream& operator>> <dim>(std::istream& is, Ball& b);

  Ball& operator=(const Ball& b)
	{m_radius = b.m_radius; m_center = b.m_center; return *this;}

  bool isEqualTo(const Ball& b, double tolerance = WFMATH_EPSILON) const
	{return m_center.isEqualTo(b.m_center, tolerance)
	     && IsFloatEqual(m_radius, b.m_radius, tolerance);}

  bool operator==(const Ball& b) const	{return isEqualTo(b);}
  bool operator!=(const Ball& b) const	{return !isEqualTo(b);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the ball.
  bool operator< (const Ball& b) const;

  // Descriptive characteristics

  int numCorners() const {return 0;}
  // This next function exists so that Ball can be used by code
  // that finds the number of corners with numCorners(), and does something
  // with each corner with getCorner(). No idea how useful that is, but
  // it's not a particularly complicated function to write.
  Point<dim> getCorner(int i) const
	{assert(false); return Point<dim>();}
  Point<dim> getCenter() const {return m_center;}

  const Point<dim>& center() const {return m_center;}
  Point<dim>& center() {return m_center;}
  const CoordType& radius() const {return m_radius;}
  CoordType& radius() {return m_radius;}

  // Movement functions

  Ball& shift(const Vector<dim>& v) {m_center += v; return *this;}
  Ball& moveCornerTo(const Point<dim>& p, int corner)
	{assert(false); return *this;}
  Ball& moveCenterTo(const Point<dim>& p) {m_center = p; return *this;}

  Ball& rotateCorner(const RotMatrix<dim>& m, int corner)
	{assert(false); return *this;}
  Ball& rotateCenter(const RotMatrix<dim>& m) {return *this;}
  Ball& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p)
	{m_center.rotate(m, p); return *this;}

  // Intersection functions

  AxisBox<dim> boundingBox() const;
  Ball boundingSphere() const		{return *this;}
  Ball boundingSphereSloppy() const	{return *this;}

  friend bool Intersect<dim>(const Ball& b, const Point<dim>& p);
  friend bool IntersectProper<dim>(const Ball& b, const Point<dim>& p);
  friend bool Contains<dim>(const Point<dim>& p, const Ball& b);

  friend bool Intersect<dim>(const Ball& b, const AxisBox<dim>& a);
  friend bool IntersectProper<dim>(const Ball& b, const AxisBox<dim>& a);
  friend bool Contains<dim>(const Ball& b, const AxisBox<dim>& a);
  friend bool ContainsProper<dim>(const Ball& b, const AxisBox<dim>& a);
  friend bool Contains<dim>(const AxisBox<dim>& a, const Ball& b);
  friend bool ContainsProper<dim>(const AxisBox<dim>& a, const Ball& b);

  friend bool Intersect<dim>(const Ball& b1, const Ball& b2);
  friend bool IntersectProper<dim>(const Ball& b1, const Ball& b2);
  friend bool Contains<dim>(const Ball& outer, const Ball& inner);
  friend bool ContainsProper<dim>(const Ball& outer, const Ball& inner);

  friend bool Intersect<dim>(const Segment<dim>& s, const Ball& b);
  friend bool IntersectProper<dim>(const Segment<dim>& s, const Ball& b);
  friend bool Contains<dim>(const Segment<dim>& s, const Ball& b);
  friend bool ContainsProper<dim>(const Segment<dim>& s, const Ball& b);

  friend bool Intersect<dim>(const RotBox<dim>& r, const Ball& b);
  friend bool IntersectProper<dim>(const RotBox<dim>& r, const Ball& b);
  friend bool Contains<dim>(const RotBox<dim>& r, const Ball& b);
  friend bool ContainsProper<dim>(const RotBox<dim>& r, const Ball& b);
  friend bool Contains<dim>(const Ball& b, const RotBox<dim>& r);
  friend bool ContainsProper<dim>(const Ball& b, const RotBox<dim>& r);

  friend bool Intersect<dim>(const Polygon<dim>& p, const Ball& b);
  friend bool IntersectProper<dim>(const Polygon<dim>& p, const Ball& b);
  friend bool Contains<dim>(const Polygon<dim>& p, const Ball& b);
  friend bool ContainsProper<dim>(const Polygon<dim>& p, const Ball& b);
  friend bool Contains<dim>(const Ball& b, const Polygon<dim>& p);
  friend bool ContainsProper<dim>(const Ball& b, const Polygon<dim>& p);

 private:

  Point<dim> m_center;
  CoordType m_radius;
};

} // namespace WFMath

#include <wfmath/ball_funcs.h>

#endif  // WFMATH_BALL_H
