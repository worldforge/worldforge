// -*-C++-*-
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

#ifndef WFMATH_SEGMENT_H
#define WFMATH_SEGMENT_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/axisbox.h>
#include <wfmath/intersect_decls.h>

namespace WF { namespace Math {

template<const int dim> class Segment;

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Segment<dim>& s);

template<const int dim>
class Segment
{
 public:
  Segment() {}
  Segment(const Point<dim>& p1, const Point<dim>& p2) : m_p1(p1), m_p2(p2) {}
  Segment(const Segment& s) : m_p1(s.m_p1), m_p2(s.m_p2) {}

  ~Segment() {}

  friend std::ostream& operator<< <dim>(std::ostream& os, const Segment& s);
  bool fromStream(std::istream& is);

  Segment& operator=(const Segment& s);

  bool isEqualTo(const Segment& s, double tolerance = WFMATH_EPSILON) const
	{return m_p1.isEqualTo(s.m_p1, tolerance)
	     && m_p2.isEqualTo(s.m_p2, tolerance);}

  bool operator==(const Segment& b) const	{return isEqualTo(b);}
  bool operator!=(const Segment& b) const	{return !isEqualTo(b);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the segment.
  bool operator< (const Segment& b) const;

  // Descriptive characteristics

  int numCorners() const {return 2;}
  Point<dim> getCorner(int i) const;
  Point<dim> getCenter() const;

  Segment& setCorner(const Point<dim>& p, int i);

  // Movement functions

  Segment& shift(const Vector<dim>& v)
	{m_p1 += v; m_p2 += v; return *this;}
  Segment& moveCornerTo(const Point<dim>& p, int corner)
	{return shift(p - getCorner(corner));}
  Segment& moveCenterTo(const Point<dim>& p)
	{return shift(p - getCenter());}

  Segment& rotateCorner(const RotMatrix<dim>& m, int corner)
	{rotatePoint(m, getCorner(corner)); return *this;}
  Segment& rotateCenter(const RotMatrix<dim>& m)
	{rotatePoint(m, getCenter()); return *this;}
  Segment<dim>& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p)
	{m_p1.rotate(m, p); m_p2.rotate(m, p); return *this;}

  // Intersection functions

  AxisBox<dim> boundingBox() const {return AxisBox<dim>(m_p1, m_p2);}

  friend bool Intersect<dim>(const Segment& s, const Point<dim>& p);
  friend bool IntersectProper<dim>(const Segment& s, const Point<dim>& p);
  friend bool Contains<dim>(const Point<dim>& p, const Segment& s);

  friend bool Intersect<dim>(const Segment& s, const AxisBox<dim>& b);
  friend bool IntersectProper<dim>(const Segment& s, const AxisBox<dim>& b);
  friend bool Contains<dim>(const AxisBox<dim>& b, const Segment& s);
  friend bool ContainsProper<dim>(const AxisBox<dim>& b, const Segment& s);

  friend bool Intersect<dim>(const Segment& s, const Ball<dim>& b);
  friend bool IntersectProper<dim>(const Segment& s, const Ball<dim>& b);
  friend bool Contains<dim>(const Ball<dim>& b, const Segment& s);
  friend bool ContainsProper<dim>(const Ball<dim>& b, const Segment& s);

  friend bool Intersect<dim>(const Segment& s1, const Segment& s2);
  friend bool IntersectProper<dim>(const Segment& s1, const Segment& s2);
  friend bool Contains<dim>(const Segment& s1, const Segment& s2);
  friend bool ContainsProper<dim>(const Segment& s1, const Segment& s2);

  friend bool Intersect<dim>(const RotBox<dim>& r, const Segment& s);
  friend bool IntersectProper<dim>(const RotBox<dim>& r, const Segment& s);
  friend bool Contains<dim>(const RotBox<dim>& r, const Segment& s);
  friend bool ContainsProper<dim>(const RotBox<dim>& r, const Segment& s);
  friend bool Contains<dim>(const Segment& s, const RotBox<dim>& r);
  friend bool ContainsProper<dim>(const Segment& s, const RotBox<dim>& r);

 private:

  Point<dim> m_p1, m_p2;
};

}} // namespace WF::Math

#include <wfmath/segment_funcs.h>

#endif  // WFMATH_SEGMENT_H
