// rotbox.h (A box with arbitrary orientation)
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

#ifndef WFMATH_ROT_BOX_H
#define WFMATH_ROT_BOX_H

#include <wfmath/point.h>
#include <wfmath/rotmatrix.h>
#include <wfmath/intersect_decls.h>

namespace WFMath {

template<int dim>
std::ostream& operator<<(std::ostream& os, const RotBox<dim>& r);
template<int dim>
std::istream& operator>>(std::istream& is, RotBox<dim>& r);

/// A dim dimensional box, lying at an arbitrary angle
/**
 * This class implements the full shape interface, as described in
 * the fake class Shape.
 **/
template<int dim = 3>
class RotBox
{
 public:
  /// construct an uninitialized box
  RotBox() : m_corner0(), m_size(), m_orient() {}
  /// construct a box from the given parameters
  /**
   * p gives corner 0 of the box, size gives the offset from corner
   * 0 to the opposite corner (corner 2^dim - 1), orientation gives
   * the rotation of the box relative to the coordinate axes
   **/
  RotBox(const Point<dim>& p, const Vector<dim>& size,
  const RotMatrix<dim>& orientation) : m_corner0(p), m_size(size),
    m_orient(orientation) {}
  /// construct a copy of the box
  RotBox(const RotBox& b) : m_corner0(b.m_corner0), m_size(b.m_size),
    m_orient(b.m_orient) {}
  /// Construct a rotbox from an object passed by Atlas
  explicit RotBox(const AtlasInType& a);

  ~RotBox() {}

  /// Create an Atlas object from the box
  AtlasOutType toAtlas() const;
  /// Set the box's value to that given by an Atlas object
  void fromAtlas(const AtlasInType& a);

  friend std::ostream& operator<< <dim>(std::ostream& os, const RotBox& r);
  friend std::istream& operator>> <dim>(std::istream& is, RotBox& r);

  RotBox& operator=(const RotBox& s);

  bool isEqualTo(const RotBox& b, double epsilon = WFMATH_EPSILON) const;

  bool operator==(const RotBox& b) const	{return isEqualTo(b);}
  bool operator!=(const RotBox& b) const	{return !isEqualTo(b);}

  bool isValid() const {return m_corner0.isValid() && m_size.isValid()
  && m_orient.isValid();}

  // Descriptive characteristics

  int numCorners() const {return 1 << dim;}
  Point<dim> getCorner(int i) const;
  Point<dim> getCenter() const {return m_corner0 + Prod(m_size / 2, m_orient);}

  /// returns the base corner of the box
  const Point<dim>& corner0() const		{return m_corner0;}
  /// returns the base corner of the box
  Point<dim>& corner0()				{return m_corner0;}
  /// returns the size of the box
  const Vector<dim>& size() const		{return m_size;}
  /// returns the size of the box
  Vector<dim>& size()				{return m_size;}
  /// returns the orientation of the box
  const RotMatrix<dim>& orientation() const	{return m_orient;}
  /// returns the orientation of the box
  RotMatrix<dim>& orientation()			{return m_orient;}

  // Movement functions

  RotBox& shift(const Vector<dim>& v)
  {m_corner0 += v; return *this;}
  RotBox& moveCornerTo(const Point<dim>& p, int corner)
  {return shift(p - getCorner(corner));}
  RotBox& moveCenterTo(const Point<dim>& p)
  {return shift(p - getCenter());}

  RotBox& rotateCorner(const RotMatrix<dim>& m, int corner)
  {rotatePoint(m, getCorner(corner)); return *this;}
  RotBox& rotateCenter(const RotMatrix<dim>& m)
  {rotatePoint(m, getCenter()); return *this;}
  RotBox& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p)
  {m_orient = Prod(m_orient, m); m_corner0.rotate(m, p); return *this;}

  // 3D rotation functions
  RotBox& rotateCorner(const Quaternion& q, int corner);
  RotBox& rotateCenter(const Quaternion& q);
  RotBox& rotatePoint(const Quaternion& q, const Point<dim>& p);

  // Intersection functions

  AxisBox<dim> boundingBox() const;
  Ball<dim> boundingSphere() const
  {return Ball<dim>(getCenter(), m_size.mag() / 2);}
  Ball<dim> boundingSphereSloppy() const
  {return Ball<dim>(getCenter(), m_size.sqrMag() / 2);}

  RotBox toParentCoords(const Point<dim>& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
        {return RotBox(m_corner0.toParentCoords(origin, rotation), m_size,
    m_orient * rotation);}
  RotBox toParentCoords(const AxisBox<dim>& coords) const
        {return RotBox(m_corner0.toParentCoords(coords), m_size, m_orient);}
  RotBox toParentCoords(const RotBox<dim>& coords) const
        {return RotBox(m_corner0.toParentCoords(coords), m_size,
    m_orient * coords.m_orient);}

  // toLocal is just like toParent, expect we reverse the order of
  // translation and rotation and use the opposite sense of the rotation
  // matrix

  RotBox toLocalCoords(const Point<dim>& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
        {return RotBox(m_corner0.toLocalCoords(origin, rotation), m_size,
    rotation * m_orient);}
  RotBox toLocalCoords(const AxisBox<dim>& coords) const
        {return RotBox(m_corner0.toLocalCoords(coords), m_size, m_orient);}
  RotBox toLocalCoords(const RotBox<dim>& coords) const
        {return RotBox(m_corner0.toLocalCoords(coords), m_size,
    coords.m_orient * m_orient);}

  // 3D only
  RotBox toParentCoords(const Point<dim>& origin, const Quaternion& rotation) const;
  RotBox toLocalCoords(const Point<dim>& origin, const Quaternion& rotation) const;

  friend bool Intersect<dim>(const RotBox& r, const Point<dim>& p, bool proper);
  friend bool Contains<dim>(const Point<dim>& p, const RotBox& r, bool proper);

  friend bool Intersect<dim>(const RotBox& r, const AxisBox<dim>& b, bool proper);
  friend bool Contains<dim>(const RotBox& r, const AxisBox<dim>& b, bool proper);
  friend bool Contains<dim>(const AxisBox<dim>& b, const RotBox& r, bool proper);

  friend bool Intersect<dim>(const RotBox& r, const Ball<dim>& b, bool proper);
  friend bool Contains<dim>(const RotBox& r, const Ball<dim>& b, bool proper);
  friend bool Contains<dim>(const Ball<dim>& b, const RotBox& r, bool proper);

  friend bool Intersect<dim>(const RotBox& r, const Segment<dim>& s, bool proper);
  friend bool Contains<dim>(const RotBox& r, const Segment<dim>& s, bool proper);
  friend bool Contains<dim>(const Segment<dim>& s, const RotBox& r, bool proper);

  friend bool Intersect<dim>(const RotBox& r1, const RotBox& r2, bool proper);
  friend bool Contains<dim>(const RotBox& outer, const RotBox& inner, bool proper);

  friend bool Intersect<dim>(const Polygon<dim>& p, const RotBox& r, bool proper);
  friend bool Contains<dim>(const Polygon<dim>& p, const RotBox& r, bool proper);
  friend bool Contains<dim>(const RotBox& r, const Polygon<dim>& p, bool proper);

 private:

  Point<dim> m_corner0;
  Vector<dim> m_size;
  RotMatrix<dim> m_orient;
};

} // namespace WFMath

#endif  // WFMATH_ROT_BOX_H
