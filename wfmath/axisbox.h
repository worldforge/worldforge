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

// Author: Ron Steinke

// The implementation of this class is based on the geometric
// parts of the Tree and Placement classes from stage/shepherd/sylvanus

#ifndef WFMATH_AXIS_BOX_H
#define WFMATH_AXIS_BOX_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/intersect_decls.h>

namespace WFMath {

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

#ifndef WFMATH_NO_TEMPLATES_AS_TEMPLATE_PARAMETERS
template<const int dim, template<class> class container>
AxisBox<dim> BoundingBox(const container<AxisBox<dim> >& c);

template<const int dim, template<class> class container>
AxisBox<dim> BoundingBox(const container<Point<dim> >& c);
#endif

/// A dim dimensional axis-aligned box
template<const int dim>
class AxisBox
{
 public:
  /// Construct an uninitialized box
  AxisBox() {}
  /// Construct a box with opposite corners p1 and p2
  AxisBox(const Point<dim>& p1, const Point<dim>& p2, bool ordered = false)
    {setCorners(p1, p2, ordered);}
  /// Construct a copy of a box
  AxisBox(const AxisBox& a) : m_low(a.m_low), m_high(a.m_high) {}
  /// Construct a box from an object passed by Atlas
  explicit AxisBox(const Atlas::Message::Object& a) {fromAtlas(a);}

  /// Print a box to a stream
  friend std::ostream& operator<< <dim>(std::ostream& os, const AxisBox& a);
  /// Parse a box from a stream
  friend std::istream& operator>> <dim>(std::istream& is, AxisBox& a);

  /// Create an Atlas object from the box
  Atlas::Message::Object toAtlas() const;
  /// Set the box's value to that given by an Atlas object
  void fromAtlas(const Atlas::Message::Object& a);

  /// Assign one box to another
  AxisBox& operator=(const AxisBox& a)
	{m_low = a.m_low; m_high = a.m_high; return *this;}

  /// Test two boxes for equality, up to a given precision
  bool isEqualTo(const AxisBox& b, double epsilon = WFMATH_EPSILON) const;

  /// Check if two boxes are equal.
  bool operator==(const AxisBox& a) const	{return isEqualTo(a);}
  /// Check if two boxes are not equal.
  bool operator!=(const AxisBox& a) const	{return !isEqualTo(a);}

  bool isValid() const {return m_low.isValid() && m_high.isValid();}

  /// WARNING! This operator is for sorting only.
  bool operator< (const AxisBox& a) const
	{return m_low < a.m_low || (m_low != a.m_low && m_high < a.m_high);}

  // Descriptive characteristics

  /// Shape:
  int numCorners() const {return 1 << dim;}
  /// Shape:
  Point<dim> getCorner(int i) const;
  /// Shape:
  Point<dim> getCenter() const {return Midpoint(m_low, m_high);}

  /// Get a reference to corner 0
  const Point<dim>& lowCorner() const	{return m_low;}
  /// Get a reference to corner (2^dim)-1
  const Point<dim>& highCorner() const	{return m_high;}

  /// Get the lower bound of the box on the i'th axis
  CoordType lowerBound(const int axis) const	{return m_low[axis];}
  /// Get the upper bound of the box on the i'th axis
  CoordType upperBound(const int axis) const	{return m_high[axis];}

  /// Set the box to have opposite corners p1 and p2
  AxisBox& setCorners(const Point<dim>& p1, const Point<dim>& p2, bool ordered = false);

  // Movement functions

  /// Shape:
  AxisBox& shift(const Vector<dim>& v)
	{m_low += v; m_high += v; return *this;}
  /// Shape:
  AxisBox& moveCornerTo(const Point<dim>& p, int corner)
	{return shift(p - getCorner(corner));}
  /// Shape:
  AxisBox& moveCenterTo(const Point<dim>& p)
	{return shift(p - getCenter());}

  // No rotation functions, this shape can't rotate

  // Intersection functions

  /// Shape:
  AxisBox boundingBox() const {return *this;}
  /// Shape:
  Ball<dim> boundingSphere() const;
  /// Shape:
  Ball<dim> boundingSphereSloppy() const;

  /// Return true if the boxes intersect, and set 'out' to their intersection
  friend bool Intersection<dim>(const AxisBox& a1, const AxisBox& a2, AxisBox& out);
  /// Get the minimal box that contains a1 and a2
  friend AxisBox Union<dim>(const AxisBox& a1, const AxisBox& a2);

  friend bool Intersect<dim>(const AxisBox& b, const Point<dim>& p, bool proper);
  friend bool Contains<dim>(const Point<dim>& p, const AxisBox& b, bool proper);

  friend bool Intersect<dim>(const AxisBox& b1, const AxisBox& b2, bool proper);
  friend bool Contains<dim>(const AxisBox& outer, const AxisBox& inner, bool proper);

  friend bool Intersect<dim>(const Ball<dim>& b, const AxisBox& a, bool proper);
  friend bool Contains<dim>(const Ball<dim>& b, const AxisBox& a, bool proper);
  friend bool Contains<dim>(const AxisBox& a, const Ball<dim>& b, bool proper);

  friend bool Intersect<dim>(const Segment<dim>& s, const AxisBox& b, bool proper);
  friend bool Contains<dim>(const Segment<dim>& s, const AxisBox& b, bool proper);

  friend bool Intersect<dim>(const RotBox<dim>& r, const AxisBox& b, bool proper);
  friend bool Contains<dim>(const RotBox<dim>& r, const AxisBox& b, bool proper);
  friend bool Contains<dim>(const AxisBox& b, const RotBox<dim>& r, bool proper);

  friend bool Intersect<dim>(const Polygon<dim>& p, const AxisBox& b, bool proper);
  friend bool Contains<dim>(const Polygon<dim>& p, const AxisBox& b, bool proper);
  friend bool Contains<dim>(const AxisBox& b, const Polygon<dim>& p, bool proper);

 private:

  Point<dim> m_low, m_high;
};

} // namespace WFMath

#include <wfmath/axisbox_funcs.h>

#endif  // WFMATH_AXIS_BOX_H
