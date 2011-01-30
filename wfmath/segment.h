// segment.h (A line segment)
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

#ifndef WFMATH_SEGMENT_H
#define WFMATH_SEGMENT_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/rotbox.h>
#include <wfmath/ball.h>
#include <wfmath/intersect_decls.h>

#include <cassert>

namespace WFMath {

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Segment<dim>& s);
template<const int dim>
std::istream& operator>>(std::istream& is, Segment<dim>& s);

/// A line segment embedded in dim dimensions
/**
 * This class implements the full shape interface, as described in
 * the fake class Shape.
 **/
template<const int dim>
class Segment
{
 public:
  /// construct an uninitialized segment
  Segment() {}
  /// construct a segment with endpoints p1 and p2
  Segment(const Point<dim>& p1, const Point<dim>& p2) : m_p1(p1), m_p2(p2) {}
  /// construct a copy of a segment
  Segment(const Segment& s) : m_p1(s.m_p1), m_p2(s.m_p2) {}

  ~Segment() {}

  friend std::ostream& operator<< <dim>(std::ostream& os, const Segment& s);
  friend std::istream& operator>> <dim>(std::istream& is, Segment& s);

  Segment& operator=(const Segment& s)
	{m_p1 = s.m_p1; m_p2 = s.m_p2; return *this;}

  bool isEqualTo(const Segment& s, double epsilon = WFMATH_EPSILON) const;

  bool operator==(const Segment& b) const	{return isEqualTo(b);}
  bool operator!=(const Segment& b) const	{return !isEqualTo(b);}

  bool isValid() const {return m_p1.isValid() && m_p2.isValid();}

  // Descriptive characteristics

  int numCorners() const {return 2;}
  Point<dim> getCorner(int i) const {assert(i == 0 || i == 1); return i ? m_p2 : m_p1;}
  Point<dim> getCenter() const {return Midpoint(m_p1, m_p2);}

  /// get one end of the segment
  const Point<dim>& endpoint(const int i) const	{return i ? m_p2 : m_p1;}
  /// get one end of the segment
  Point<dim>& endpoint(const int i)		{return i ? m_p2 : m_p1;}

  // Movement functions

  Segment& shift(const Vector<dim>& v)
	{m_p1 += v; m_p2 += v; return *this;}
  Segment& moveCornerTo(const Point<dim>& p, int corner);
  Segment& moveCenterTo(const Point<dim>& p)
	{return shift(p - getCenter());}

  Segment& rotateCorner(const RotMatrix<dim>& m, int corner);
  Segment& rotateCenter(const RotMatrix<dim>& m)
	{rotatePoint(m, getCenter()); return *this;}
  Segment<dim>& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p)
	{m_p1.rotate(m, p); m_p2.rotate(m, p); return *this;}

  // 3D rotation functions
  Segment& rotateCorner(const Quaternion& q, int corner);
  Segment& rotateCenter(const Quaternion& q);
  Segment& rotatePoint(const Quaternion& q, const Point<dim>& p);

  // Intersection functions

  AxisBox<dim> boundingBox() const {return AxisBox<dim>(m_p1, m_p2);}
  Ball<dim> boundingSphere() const
	{return Ball<dim>(getCenter(), Distance(m_p1, m_p2) / 2);}
  Ball<dim> boundingSphereSloppy() const
	{return Ball<dim>(getCenter(), SloppyDistance(m_p1, m_p2) / 2);}

  Segment toParentCoords(const Point<dim>& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
        {return Segment(m_p1.toParentCoords(origin, rotation),
		m_p2.toParentCoords(origin, rotation));}
  Segment toParentCoords(const AxisBox<dim>& coords) const
        {return Segment(m_p1.toParentCoords(coords), m_p2.toParentCoords(coords));}
  Segment toParentCoords(const RotBox<dim>& coords) const
        {return Segment(m_p1.toParentCoords(coords), m_p2.toParentCoords(coords));}

  // toLocal is just like toParent, expect we reverse the order of
  // translation and rotation and use the opposite sense of the rotation
  // matrix

  Segment toLocalCoords(const Point<dim>& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
        {return Segment(m_p1.toLocalCoords(origin, rotation),
		m_p2.toLocalCoords(origin, rotation));}
  Segment toLocalCoords(const AxisBox<dim>& coords) const
        {return Segment(m_p1.toLocalCoords(coords), m_p2.toLocalCoords(coords));}
  Segment toLocalCoords(const RotBox<dim>& coords) const
        {return Segment(m_p1.toLocalCoords(coords), m_p2.toLocalCoords(coords));}

  // 3D only
  Segment toParentCoords(const Point<dim>& origin,
                         const Quaternion& rotation) const;
  Segment toLocalCoords(const Point<dim>& origin,
                        const Quaternion& rotation) const;

  friend bool Intersect<dim>(const Segment& s, const Point<dim>& p, bool proper);
  friend bool Contains<dim>(const Point<dim>& p, const Segment& s, bool proper);

  friend bool Intersect<dim>(const Segment& s, const AxisBox<dim>& b, bool proper);
  friend bool Contains<dim>(const AxisBox<dim>& b, const Segment& s, bool proper);

  friend bool Intersect<dim>(const Segment& s, const Ball<dim>& b, bool proper);
  friend bool Contains<dim>(const Ball<dim>& b, const Segment& s, bool proper);

  friend bool Intersect<dim>(const Segment& s1, const Segment& s2, bool proper);
  friend bool Contains<dim>(const Segment& s1, const Segment& s2, bool proper);

  friend bool Intersect<dim>(const RotBox<dim>& r, const Segment& s, bool proper);
  friend bool Contains<dim>(const RotBox<dim>& r, const Segment& s, bool proper);
  friend bool Contains<dim>(const Segment& s, const RotBox<dim>& r, bool proper);

  friend bool Intersect<dim>(const Polygon<dim>& r, const Segment& s, bool proper);
  friend bool Contains<dim>(const Polygon<dim>& p, const Segment& s, bool proper);
  friend bool Contains<dim>(const Segment& s, const Polygon<dim>& p, bool proper);

 private:

  Point<dim> m_p1, m_p2;
};

} // namespace WFMath

#endif  // WFMATH_SEGMENT_H
