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
#include <wfmath/intersect_decls.h>

#include <iosfwd>

namespace WFMath {

template<const int dim>
bool Intersection(const AxisBox<dim>& a1, const AxisBox<dim>& a2, AxisBox<dim>& out);
template<const int dim>
AxisBox<dim> Union(const AxisBox<dim>& a1, const AxisBox<dim>& a2);

template<const int dim>
std::ostream& operator<<(std::ostream& os, const AxisBox<dim>& m);
template<const int dim>
std::istream& operator>>(std::istream& is, AxisBox<dim>& m);

#ifndef WFMATH_NO_TEMPLATES_AS_TEMPLATE_PARAMETERS
/// Get the axis-aligned bounding box for a set of boxes
template<const int dim, template<class, class> class container>
AxisBox<dim> BoundingBox(const container<AxisBox<dim>, std::allocator<AxisBox<dim> > >& c);

/// Get the axis-aligned bounding box for a set of points
template<const int dim, template<class, class> class container>
AxisBox<dim> BoundingBox(const container<Point<dim>, std::allocator<Point<dim> > >& c);
#endif

/// A dim dimensional axis-aligned box
/**
 * This class implements the full shape interface, as described in
 * the fake class Shape, with the exception of the rotation functions.
 **/
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
  explicit AxisBox(const AtlasInType& a);

  friend std::ostream& operator<< <dim>(std::ostream& os, const AxisBox& a);
  friend std::istream& operator>> <dim>(std::istream& is, AxisBox& a);

  /// Create an Atlas object from the box
  AtlasOutType toAtlas() const;
  /// Set the box's value to that given by an Atlas object
  void fromAtlas(const AtlasInType& a);

  AxisBox& operator=(const AxisBox& a)
  {m_low = a.m_low; m_high = a.m_high; return *this;}

  bool isEqualTo(const AxisBox& b, double epsilon = WFMATH_EPSILON) const;
  bool operator==(const AxisBox& a) const	{return isEqualTo(a);}
  bool operator!=(const AxisBox& a) const	{return !isEqualTo(a);}

  bool isValid() const {return m_low.isValid() && m_high.isValid();}

  // Descriptive characteristics

  int numCorners() const {return 1 << dim;}
  Point<dim> getCorner(int i) const;
  Point<dim> getCenter() const {return Midpoint(m_low, m_high);}

  /// Get a reference to corner 0
  const Point<dim>& lowCorner() const {return m_low;}
  Point<dim>& lowCorner() {return m_low;}
  /// Get a reference to corner (2^dim)-1
  const Point<dim>& highCorner() const {return m_high;}
  Point<dim>& highCorner() {return m_high;}

  /// Get the lower bound of the box on the i'th axis
  CoordType lowerBound(const int axis) const	{return m_low[axis];}
  /// Get the upper bound of the box on the i'th axis
  CoordType upperBound(const int axis) const	{return m_high[axis];}

  /// Set the box to have opposite corners p1 and p2
  /**
   * The 'ordered' variable may be set to true if p1[i] <= p2[i] for all
   * i. It is always safe to leave 'ordered' as false, it is a speed
   * optimization primarily intended for use inside the library.
   **/
  AxisBox& setCorners(const Point<dim>& p1, const Point<dim>& p2,
  bool ordered = false);

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

  AxisBox toParentCoords(const Point<dim>& origin) const
        {return AxisBox(m_low.toParentCoords(origin), m_high.toParentCoords(origin), true);}
  AxisBox toParentCoords(const AxisBox<dim>& coords) const
        {return AxisBox(m_low.toParentCoords(coords), m_high.toParentCoords(coords), true);}

  // toLocal is just like toParent, expect we reverse the order of
  // translation and rotation and use the opposite sense of the rotation
  // matrix

  AxisBox toLocalCoords(const Point<dim>& origin) const
        {return AxisBox(m_low.toLocalCoords(origin), m_high.toLocalCoords(origin), true);}
  AxisBox toLocalCoords(const AxisBox<dim>& coords) const
        {return AxisBox(m_low.toLocalCoords(coords), m_high.toLocalCoords(coords), true);}

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

#endif  // WFMATH_AXIS_BOX_H
