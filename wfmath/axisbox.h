// -*-C++-*-
// axisbox.h (An axis-aligned box)
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

// The implementation of this class is based on the geometric
// parts of the Tree and Placement classes from stage/shepherd/sylvanus

#ifndef WFMATH_AXIS_BOX_H
#define WFMATH_AXIS_BOX_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/intersect_decls.h>

namespace WF { namespace Math {

template<const int dim> class AxisBox;
template<const int dim> class Ball;

template<const int dim>
bool Intersection(const AxisBox<dim>& a1, const AxisBox<dim>& a2, AxisBox<dim>& out);
template<const int dim>
AxisBox<dim> Union(const AxisBox<dim>& a1, const AxisBox<dim>& a2);

template<const int dim>
std::ostream& operator<<(std::ostream& os, const AxisBox<dim>& m);
template<const int dim>
std::istream& operator>>(std::istream& is, AxisBox<dim>& m);

template<const int dim, template<class> class container>
AxisBox<dim> BoundingBox(const container<AxisBox<dim> >& c);

template<const int dim>
class AxisBox
{
 public:
  AxisBox() {}
  AxisBox(const Point<dim>& p1, const Point<dim>& p2, bool ordered = false)
    {setCorners(p1, p2, ordered);}
  AxisBox(const AxisBox& a) : m_low(a.m_low), m_high(a.m_high) {}

  ~AxisBox() {}

  friend std::ostream& operator<< <dim>(std::ostream& os, const AxisBox& a);
  friend std::istream& operator>> <dim>(std::istream& is, AxisBox& a);

  AxisBox& operator=(const AxisBox& a)
	{m_low = a.m_low; m_high = a.m_high; return *this;}

  bool isEqualTo(const AxisBox& a, double tolerance = WFMATH_EPSILON) const
	{return m_low.isEqualTo(a.m_low, tolerance)
	     && m_high.isEqualTo(a.m_high, tolerance);}

  bool operator==(const AxisBox& a) const	{return isEqualTo(a);}
  bool operator!=(const AxisBox& a) const	{return !isEqualTo(a);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the box.
  bool operator< (const AxisBox& a) const
	{return m_low < a.m_low || (m_low != a.m_low && m_high < a.m_high);}

  // Descriptive characteristics

  int numCorners() const {return 1 << dim;}
  Point<dim> getCorner(int i) const;
  Point<dim> getCenter() const {return Midpoint(m_low, m_high);}

  const Point<dim>& lowCorner() const	{return m_low;}
  const Point<dim>& highCorner() const	{return m_high;}

  CoordType lowerBound(const int axis) const	{return m_low[axis];}
  CoordType upperBound(const int axis) const	{return m_high[axis];}

  AxisBox& setCorners(const Point<dim>& p1, const Point<dim>& p2, bool ordered = false);

  // Movement functions

  AxisBox& shift(const Vector<dim>& v)
	{m_low += v; m_high += v; return *this;}
  AxisBox& moveCornerTo(const Point<dim>& p, int corner)
	{return shift(p - getCorner(corner));}
  AxisBox& moveCenterTo(const Point<dim>& p)
	{return shift(p - getCenter());}

  // No rotation functions, this shape can't rotate

  // Intersection functions

  AxisBox boundingBox() const {return *this;}
  Ball<dim> boundingSphere() const;
  Ball<dim> boundingSphereSloppy() const;

  friend bool Intersection<dim>(const AxisBox& a1, const AxisBox& a2, AxisBox& out);
  friend AxisBox Union<dim>(const AxisBox& a1, const AxisBox& a2);

  friend bool Intersect<dim>(const AxisBox& b, const Point<dim>& p);
  friend bool IntersectProper<dim>(const AxisBox& b, const Point<dim>& p);
  friend bool Contains<dim>(const Point<dim>& p, const AxisBox& b);

  friend bool Intersect<dim>(const AxisBox& b1, const AxisBox& b2);
  friend bool IntersectProper<dim>(const AxisBox& b1, const AxisBox& b2);
  friend bool Contains<dim>(const AxisBox& outer, const AxisBox& inner);
  friend bool ContainsProper<dim>(const AxisBox& outer, const AxisBox& inner);

  friend bool Intersect<dim>(const Ball<dim>& b, const AxisBox& a);
  friend bool IntersectProper<dim>(const Ball<dim>& b, const AxisBox& a);
  friend bool Contains<dim>(const Ball<dim>& b, const AxisBox& a);
  friend bool ContainsProper<dim>(const Ball<dim>& b, const AxisBox& a);
  friend bool Contains<dim>(const AxisBox& a, const Ball<dim>& b);
  friend bool ContainsProper<dim>(const AxisBox& a, const Ball<dim>& b);

  friend bool Intersect<dim>(const Segment<dim>& s, const AxisBox& b);
  friend bool IntersectProper<dim>(const Segment<dim>& s, const AxisBox& b);
  friend bool Contains<dim>(const Segment<dim>& s, const AxisBox& b);
  friend bool ContainsProper<dim>(const Segment<dim>& s, const AxisBox& b);

  friend bool Intersect<dim>(const RotBox<dim>& r, const AxisBox& b);
  friend bool IntersectProper<dim>(const RotBox<dim>& r, const AxisBox& b);
  friend bool Contains<dim>(const RotBox<dim>& r, const AxisBox& b);
  friend bool ContainsProper<dim>(const RotBox<dim>& r, const AxisBox& b);
  friend bool Contains<dim>(const AxisBox& b, const RotBox<dim>& r);
  friend bool ContainsProper<dim>(const AxisBox& b, const RotBox<dim>& r);

 private:

  Point<dim> m_low, m_high;
};

}} // namespace WF::Math

#include <wfmath/axisbox_funcs.h>

#endif  // WFMATH_AXIS_BOX_H
