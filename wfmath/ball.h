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

#ifndef WFMATH_BALL_H
#define WFMATH_BALL_H

#include <wfmath/point.h>
#include <wfmath/intersect_decls.h>

namespace WFMath {

template<int dim> class Ball;

/// get the minimal bounding sphere for a set of points
template<int dim, template<class, class> class container>
Ball<dim> BoundingSphere(const container<Point<dim>, std::allocator<Point<dim> > >& c);
/// get a bounding sphere for a set of points
template<int dim, template<class, class> class container>
Ball<dim> BoundingSphereSloppy(const container<Point<dim>, std::allocator<Point<dim> > >& c);

template<int dim>
std::ostream& operator<<(std::ostream& os, const Ball<dim>& m);
template<int dim>
std::istream& operator>>(std::istream& is, Ball<dim>& m);

/// A dim dimensional ball
/**
 * This class implements the full shape interface, as described in
 * the fake class Shape.
 *
 * This class is called Ball<> instead of Sphere to be more in tune
 * with the usual mathematical naming conventions, where a ball is
 * a filled object, while a sphere is just the outer shell. It also
 * helps that a Ball<n> corresponds to an n-ball, while a Sphere<n>
 * would correspond to an (n-1)-sphere.
 **/
template<int dim = 3>
class Ball
{
 public:
  /// construct an uninitialized ball
  Ball() : m_center(), m_radius(0.f) {}
  /// construct a ball with the given center and radius
  Ball(const Point<dim>& center, CoordType radius)
  : m_center(center), m_radius(radius) { if (radius < 0) m_center.setValid(false); }
  /// construct a copy of a ball
  Ball(const Ball& b) : m_center(b.m_center), m_radius(b.m_radius) {}
  /// Construct a ball from an object passed by Atlas
  explicit Ball(const AtlasInType& a);

  ~Ball() {}

  friend std::ostream& operator<< <dim>(std::ostream& os, const Ball& b);
  friend std::istream& operator>> <dim>(std::istream& is, Ball& b);

  /// Create an Atlas object from the box
  AtlasOutType toAtlas() const;
  /// Set the box's value to that given by an Atlas object
  void fromAtlas(const AtlasInType& a);

  Ball& operator=(const Ball& b)
  {m_radius = b.m_radius; m_center = b.m_center; return *this;}

  bool isEqualTo(const Ball& b, CoordType epsilon = numeric_constants<CoordType>::epsilon()) const;

  bool operator==(const Ball& b) const	{return isEqualTo(b);}
  bool operator!=(const Ball& b) const	{return !isEqualTo(b);}

  bool isValid() const {return m_center.isValid();}

  // Descriptive characteristics

  size_t numCorners() const {return 0;}
  // This next function exists so that Ball can be used by code
  // that finds the number of corners with numCorners(), and does something
  // with each corner with getCorner(). No idea how useful that is, but
  // it's not a particularly complicated function to write.
  Point<dim> getCorner(size_t) const {return m_center;}
  Point<dim> getCenter() const {return m_center;}

  /// get the center of the ball
  const Point<dim>& center() const {return m_center;}
  /// get the center of the ball
  Point<dim>& center() {return m_center;}
  /// get the radius of the ball
  CoordType radius() const {return m_radius;}
  /// get the radius of the ball
  CoordType& radius() {return m_radius;}

  // Movement functions

  Ball& shift(const Vector<dim>& v) {m_center += v; return *this;}
  Ball& moveCornerTo(const Point<dim>&, size_t) {return *this;}
  Ball& moveCenterTo(const Point<dim>& p) {m_center = p; return *this;}

  Ball& rotateCorner(const RotMatrix<dim>&, size_t) {return *this;}
  Ball& rotateCenter(const RotMatrix<dim>&) {return *this;}
  Ball& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p)
  {m_center.rotate(m, p); return *this;}

  // 3D rotation function
  Ball& rotateCorner(const Quaternion&, size_t corner);
  Ball& rotateCenter(const Quaternion&);
  Ball& rotatePoint(const Quaternion& q, const Point<dim>& p);

  // Intersection functions

  AxisBox<dim> boundingBox() const;
  Ball boundingSphere() const		{return *this;}
  Ball boundingSphereSloppy() const	{return *this;}

  Ball toParentCoords(const Point<dim>& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
        {return Ball(m_center.toParentCoords(origin, rotation), m_radius);}
  Ball toParentCoords(const AxisBox<dim>& coords) const
        {return Ball(m_center.toParentCoords(coords), m_radius);}
  Ball toParentCoords(const RotBox<dim>& coords) const
        {return Ball(m_center.toParentCoords(coords), m_radius);}

  // toLocal is just like toParent, expect we reverse the order of
  // translation and rotation and use the opposite sense of the rotation
  // matrix

  Ball toLocalCoords(const Point<dim>& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
        {return Ball(m_center.toLocalCoords(origin, rotation), m_radius);}
  Ball toLocalCoords(const AxisBox<dim>& coords) const
        {return Ball(m_center.toLocalCoords(coords), m_radius);}
  Ball toLocalCoords(const RotBox<dim>& coords) const
        {return Ball(m_center.toLocalCoords(coords), m_radius);}

  // 3D only
  Ball toParentCoords(const Point<dim>& origin, const Quaternion& rotation) const;
  Ball toLocalCoords(const Point<dim>& origin, const Quaternion& rotation) const;

  friend bool Intersect<dim>(const Ball& b, const Point<dim>& p, bool proper);
  friend bool Contains<dim>(const Point<dim>& p, const Ball& b, bool proper);

  friend bool Intersect<dim>(const Ball& b, const AxisBox<dim>& a, bool proper);
  friend bool Contains<dim>(const Ball& b, const AxisBox<dim>& a, bool proper);
  friend bool Contains<dim>(const AxisBox<dim>& a, const Ball& b, bool proper);

  friend bool Intersect<dim>(const Ball& b1, const Ball& b2, bool proper);
  friend bool Contains<dim>(const Ball& outer, const Ball& inner, bool proper);

  friend bool Intersect<dim>(const Segment<dim>& s, const Ball& b, bool proper);
  friend bool Contains<dim>(const Segment<dim>& s, const Ball& b, bool proper);

  friend bool Intersect<dim>(const RotBox<dim>& r, const Ball& b, bool proper);
  friend bool Contains<dim>(const RotBox<dim>& r, const Ball& b, bool proper);
  friend bool Contains<dim>(const Ball& b, const RotBox<dim>& r, bool proper);

  friend bool Intersect<dim>(const Polygon<dim>& p, const Ball& b, bool proper);
  friend bool Contains<dim>(const Polygon<dim>& p, const Ball& b, bool proper);
  friend bool Contains<dim>(const Ball& b, const Polygon<dim>& p, bool proper);

 private:

  Point<dim> m_center;
  CoordType m_radius;
};

} // namespace WFMath

#endif  // WFMATH_BALL_H
