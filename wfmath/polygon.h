// polygon.h (A 2D polygon embeded in a <dim> dimensional space)
//
//  The WorldForge Project
//  Copyright (C) 2002  The WorldForge Project
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

#ifndef WFMATH_POLYGON_H
#define WFMATH_POLYGON_H

#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>
#include <wfmath/rotbox.h>
#include <wfmath/intersect_decls.h>

#include <vector>
#include <limits>

namespace WFMath {

template<int dim> class Polygon;

template<int dim>
std::ostream& operator<<(std::ostream& os, const Polygon<dim>& r);
template<int dim>
std::istream& operator>>(std::istream& is, Polygon<dim>& r);

/// The 2D specialization of the Polygon<> template
template<>
class Polygon<2>
{
 public:
  Polygon() : m_points() {}
  Polygon(const Polygon& p) : m_points(p.m_points) {}
  /// Construct a polygon from an object passed by Atlas
  explicit Polygon(const AtlasInType& a) : m_points() {fromAtlas(a);}

  ~Polygon() {}

  friend std::ostream& operator<< <2>(std::ostream& os, const Polygon& p);
  friend std::istream& operator>> <2>(std::istream& is, Polygon& p);

  
  /// Create an Atlas object from the box
  AtlasOutType toAtlas() const;
  /// Set the box's value to that given by an Atlas object
  void fromAtlas(const AtlasInType& a);
  
  Polygon& operator=(const Polygon& p)
  {m_points = p.m_points; return *this;}

  bool isEqualTo(const Polygon& p, CoordType epsilon = numeric_constants<CoordType>::epsilon()) const;

  bool operator==(const Polygon& p) const	{return isEqualTo(p);}
  bool operator!=(const Polygon& p) const	{return !isEqualTo(p);}

  bool isValid() const;

  // Descriptive characteristics

  size_t numCorners() const {return m_points.size();}
  Point<2> getCorner(size_t i) const {return m_points[i];}
  Point<2> getCenter() const {return Barycenter(m_points);}

  // For a Polygon<2>, addCorner() and moveCorner() always succeed.
  // The return values are present for the sake of a unified template
  // interface, and the epsilon argument is ignored

  // Add before i'th corner, zero is beginning, numCorners() is end
  bool addCorner(size_t i, const Point<2>& p, CoordType = numeric_constants<CoordType>::epsilon())
  {m_points.insert(m_points.begin() + i, p); return true;}

  // Remove the i'th corner
  void removeCorner(size_t i) {m_points.erase(m_points.begin() + i);}

  // Move the i'th corner to p
  bool moveCorner(size_t i, const Point<2>& p, CoordType = numeric_constants<CoordType>::epsilon())
  {m_points[i] = p; return true;}

  // Remove all points
  void clear()	{m_points.clear();}

  const Point<2>& operator[](size_t i) const {return m_points[i];}
  Point<2>& operator[](size_t i)		  {return m_points[i];}

  void resize(std::vector<Point<2> >::size_type size) {m_points.resize(size);}

  // Movement functions

  Polygon& shift(const Vector<2>& v);
  Polygon& moveCornerTo(const Point<2>& p, size_t corner)
  {return shift(p - getCorner(corner));}
  Polygon& moveCenterTo(const Point<2>& p)
  {return shift(p - getCenter());}

  Polygon& rotateCorner(const RotMatrix<2>& m, size_t corner)
  {rotatePoint(m, getCorner(corner)); return *this;}
  Polygon& rotateCenter(const RotMatrix<2>& m)
  {rotatePoint(m, getCenter()); return *this;}
  Polygon& rotatePoint(const RotMatrix<2>& m, const Point<2>& p);

  // Intersection functions

  AxisBox<2> boundingBox() const {return BoundingBox(m_points);}
  Ball<2> boundingSphere() const {return BoundingSphere(m_points);}
  Ball<2> boundingSphereSloppy() const {return BoundingSphereSloppy(m_points);}

  Polygon toParentCoords(const Point<2>& origin,
      const RotMatrix<2>& rotation = RotMatrix<2>().identity()) const;
  Polygon toParentCoords(const AxisBox<2>& coords) const;
  Polygon toParentCoords(const RotBox<2>& coords) const;

  // toLocal is just like toParent, expect we reverse the order of
  // translation and rotation and use the opposite sense of the rotation
  // matrix

  Polygon toLocalCoords(const Point<2>& origin,
      const RotMatrix<2>& rotation = RotMatrix<2>().identity()) const;
  Polygon toLocalCoords(const AxisBox<2>& coords) const;
  Polygon toLocalCoords(const RotBox<2>& coords) const;

  friend bool Intersect<2>(const Polygon& r, const Point<2>& p, bool proper);
  friend bool Contains<2>(const Point<2>& p, const Polygon& r, bool proper);

  friend bool Intersect<2>(const Polygon& p, const AxisBox<2>& b, bool proper);
  friend bool Contains<2>(const Polygon& p, const AxisBox<2>& b, bool proper);
  friend bool Contains<2>(const AxisBox<2>& b, const Polygon& p, bool proper);

  friend bool Intersect<2>(const Polygon& p, const Ball<2>& b, bool proper);
  friend bool Contains<2>(const Polygon& p, const Ball<2>& b, bool proper);
  friend bool Contains<2>(const Ball<2>& b, const Polygon& p, bool proper);

  friend bool Intersect<2>(const Polygon& r, const Segment<2>& s, bool proper);
  friend bool Contains<2>(const Polygon& p, const Segment<2>& s, bool proper);
  friend bool Contains<2>(const Segment<2>& s, const Polygon& p, bool proper);

  friend bool Intersect<2>(const Polygon& p, const RotBox<2>& r, bool proper);
  friend bool Contains<2>(const Polygon& p, const RotBox<2>& r, bool proper);
  friend bool Contains<2>(const RotBox<2>& r, const Polygon& p, bool proper);

  friend bool Intersect<2>(const Polygon& p1, const Polygon& p2, bool proper);
  friend bool Contains<2>(const Polygon& outer, const Polygon& inner, bool proper);

private:
  std::vector<Point<2> > m_points;
  typedef std::vector<Point<2> >::iterator theIter;
  typedef std::vector<Point<2> >::const_iterator theConstIter;

};

// Helper classes, to keep track of the orientation of
// a 2D polygon in dim dimensions

typedef enum {
  _WFMATH_POLY2REORIENT_NONE,
  _WFMATH_POLY2REORIENT_CLEAR_AXIS2,
  _WFMATH_POLY2REORIENT_CLEAR_BOTH_AXES,
  _WFMATH_POLY2REORIENT_MOVE_AXIS2_TO_AXIS1,
  _WFMATH_POLY2REORIENT_SCALE1_CLEAR2
} _Poly2ReorientType;

// Reorient a 2D polygon to match a change in the basis
// used by _Poly2Orient
class _Poly2Reorient
{
public:
  _Poly2Reorient(_Poly2ReorientType type, CoordType scale = 0.0)
  : m_type(type), m_scale(scale) {}
  ~_Poly2Reorient() {}

  void reorient(Polygon<2>& poly, size_t skip = std::numeric_limits<size_t>::max()) const;

private:
  _Poly2ReorientType m_type;
  CoordType m_scale;
};

template<int dim> class _Poly2Orient;

struct _Poly2OrientIntersectData {
  int dim;
  Point<2> p1, p2;
  Vector<2> v1, v2, off;
  bool o1_is_line, o2_is_line;
};

// Finds the intersection of the two planes, returns the
// dimension of the intersection space, the rest of the arguments
// are various information returned depending on the dimension of
// the intersection
template<int dim>
int  _Intersect(const _Poly2Orient<dim> &, const _Poly2Orient<dim> &,
    _Poly2OrientIntersectData &);

// Keep track of the orientation of a 2D polygon in dim dimensions
template<int dim>
class _Poly2Orient
{
public:
  _Poly2Orient() : m_origin() {}
  _Poly2Orient(const _Poly2Orient& p) : m_origin() {operator=(p);}
  ~_Poly2Orient() {}

  _Poly2Orient& operator=(const _Poly2Orient& p);

  // Convert a point in the 2D polygon to a point in dim dimensional space
  Point<dim> convert(const Point<2>& p) const;

  // Try to convert a point from dim dimensions into 2D, expanding the
  // basis if necessary. Returns true on success. On failure, the
  // basis is unchanged.
  bool expand(const Point<dim>& pd, Point<2>& p2, CoordType epsilon = numeric_constants<CoordType>::epsilon());

  // Reduce the basis to the minimum necessary to span the points in
  // poly (with the exception of skip). Returns _Poly2Reorient, which needs
  // to be used to reorient the points to match the new basis.
  _Poly2Reorient reduce(const Polygon<2>& poly, size_t skip = std::numeric_limits<size_t>::max());

  void shift(const Vector<dim>& v) {if(m_origin.isValid()) m_origin += v;}
  void rotate(const RotMatrix<dim>& m, const Point<dim>& p);
  // Rotates about the point which corresponds to "p" in the oriented plane
  void rotate2(const RotMatrix<dim>& m, const Point<2>& p);

//3D only
  void rotate(const Quaternion& q, const Point<3>& p);
  // Rotates about the point which corresponds to "p" in the oriented plane
  void rotate2(const Quaternion& q, const Point<2>& p);

  _Poly2Orient toParentCoords(const Point<dim>& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
  {_Poly2Orient p(*this); p.m_origin = m_origin.toParentCoords(origin, rotation);
    p.m_axes[0].rotate(rotation); p.m_axes[1].rotate(rotation); return p;}
  _Poly2Orient toParentCoords(const AxisBox<dim>& coords) const
  {_Poly2Orient p(*this); p.m_origin = m_origin.toParentCoords(coords); return p;}
  _Poly2Orient toParentCoords(const RotBox<dim>& coords) const
  {_Poly2Orient p(*this); p.m_origin = m_origin.toParentCoords(coords);
    p.m_axes[0].rotate(coords.orientation());
    p.m_axes[1].rotate(coords.orientation()); return p;}

  // toLocal is just like toParent, expect we reverse the order of
  // translation and rotation and use the opposite sense of the rotation
  // matrix

  _Poly2Orient toLocalCoords(const Point<dim>& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
  {_Poly2Orient p(*this); p.m_origin = m_origin.toLocalCoords(origin, rotation);
    p.m_axes[0] = rotation * p.m_axes[0];
    p.m_axes[1] = rotation * p.m_axes[1]; return p;}
  _Poly2Orient toLocalCoords(const AxisBox<dim>& coords) const
  {_Poly2Orient p(*this); p.m_origin = m_origin.toLocalCoords(coords); return p;}
  _Poly2Orient toLocalCoords(const RotBox<dim>& coords) const
  {_Poly2Orient p(*this); p.m_origin = m_origin.toLocalCoords(coords);
    p.m_axes[0] = coords.orientation() * p.m_axes[0];
    p.m_axes[1] = coords.orientation() * p.m_axes[1]; return p;}

  // 3D only
  _Poly2Orient<3> toParentCoords(const Point<3>& origin, const Quaternion& rotation) const
  {_Poly2Orient p(*this); p.m_origin = m_origin.toParentCoords(origin, rotation);
    p.m_axes[0].rotate(rotation); p.m_axes[0].rotate(rotation); return p;}
  _Poly2Orient<3> toLocalCoords(const Point<3>& origin, const Quaternion& rotation) const
  {_Poly2Orient p(*this); p.m_origin = m_origin.toLocalCoords(origin, rotation);
    p.m_axes[0].rotate(rotation.inverse());
    p.m_axes[0].rotate(rotation.inverse()); return p;}

  // Gives the offset from pd to the space spanned by
  // the basis, and puts the nearest point in p2.
  Vector<dim> offset(const Point<dim>& pd, Point<2>& p2) const;

  // Like offset, but returns true if the point is in the plane
  bool checkContained(const Point<dim>& pd, Point<2> & p2) const;

  // Check if the AxisBox intersects the spanned space, and if so
  // return a point in the intersection.
  bool checkIntersect(const AxisBox<dim>& b, Point<2>& p2, bool proper) const;

  friend int  _Intersect<dim>(const _Poly2Orient<dim> &, const _Poly2Orient<dim> &,
            _Poly2OrientIntersectData &);

private:
  // special case of the above when both axes are valid
  bool checkIntersectPlane(const AxisBox<dim>& b, Point<2>& p2, bool proper) const;

  Point<dim> m_origin;
  Vector<dim> m_axes[2]; // Normalized to unit length
};

/// A polygon, all of whose points lie in a plane, embedded in dim dimensions
template<int dim = 3>
class Polygon
{
public:
  Polygon() : m_orient(), m_poly() {}
  Polygon(const Polygon& p) : m_orient(p.m_orient), m_poly(p.m_poly) {}

  ~Polygon() {}

  friend std::ostream& operator<< <dim>(std::ostream& os, const Polygon& p);
  friend std::istream& operator>> <dim>(std::istream& is, Polygon& p);

  Polygon& operator=(const Polygon& p)
  {m_orient = p.m_orient; m_poly = p.m_poly; return *this;}

  bool isEqualTo(const Polygon& p2, CoordType epsilon = numeric_constants<CoordType>::epsilon()) const;

  bool operator==(const Polygon& p) const	{return isEqualTo(p);}
  bool operator!=(const Polygon& p) const	{return !isEqualTo(p);}

  bool isValid() const {return m_poly.isValid();}

  // Descriptive characteristics

  size_t numCorners() const {return m_poly.numCorners();}
  Point<dim> getCorner(size_t i) const {return m_orient.convert(m_poly[i]);}
  Point<dim> getCenter() const {return m_orient.convert(m_poly.getCenter());}

  // The failure of the following functions does not invalidate the
  // polygon, but merely leaves it unchaged.

  // Add before i'th corner, zero is beginning, numCorners() is end
  // Only succeeds if p lies in a plane with all current points
  bool addCorner(size_t i, const Point<dim>& p, CoordType epsilon = numeric_constants<CoordType>::epsilon());

  // Remove the i'th corner
  void removeCorner(size_t i);

  // Move the i'th corner to p, only succeeds if new location
  // lies in the same plane as all the other points. Note that,
  // under certain circumstances, this plane may not contain the
  // original location of the point.
  bool moveCorner(size_t i, const Point<dim>& p, CoordType epsilon = numeric_constants<CoordType>::epsilon());

  // Remove all points
  void clear()	{m_poly.clear(); m_orient = _Poly2Orient<dim>();}

  // Movement functions

  Polygon& shift(const Vector<dim>& v)
  {m_orient.shift(v); return *this;}
  Polygon& moveCornerTo(const Point<dim>& p, size_t corner)
  {return shift(p - getCorner(corner));}
  Polygon& moveCenterTo(const Point<dim>& p)
  {return shift(p - getCenter());}

  Polygon& rotateCorner(const RotMatrix<dim>& m, size_t corner)
  {m_orient.rotate2(m, m_poly[corner]); return *this;}
  Polygon& rotateCenter(const RotMatrix<dim>& m)
  {if(m_poly.numCorners() > 0)
    m_orient.rotate2(m, m_poly.getCenter());
  return *this;}
  Polygon& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p)
  {m_orient.rotate(m, p); return *this;}

  // 3D rotation functions
  Polygon<3>& rotateCorner(const Quaternion& q, size_t corner)
  {m_orient.rotate2(q, m_poly[corner]); return *this;}
  Polygon<3>& rotateCenter(const Quaternion& q)
  {if(m_poly.numCorners() > 0)
    m_orient.rotate2(q, m_poly.getCenter());
  return *this;}
  Polygon<3>& rotatePoint(const Quaternion& q, const Point<3>& p)
  {m_orient.rotate(q, p); return *this;}

  // Intersection functions

  AxisBox<dim> boundingBox() const;
  Ball<dim> boundingSphere() const;
  Ball<dim> boundingSphereSloppy() const;

  Polygon toParentCoords(const Point<dim>& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
        {Polygon p(*this); p.m_orient = m_orient.toParentCoords(origin, rotation); return p;}
  Polygon toParentCoords(const AxisBox<dim>& coords) const
        {Polygon p(*this); p.m_orient = m_orient.toParentCoords(coords); return p;}
  Polygon toParentCoords(const RotBox<dim>& coords) const
        {Polygon p(*this); p.m_orient = m_orient.toParentCoords(coords); return p;}

  // toLocal is just like toParent, expect we reverse the order of
  // translation and rotation and use the opposite sense of the rotation
  // matrix

  Polygon toLocalCoords(const Point<dim>& origin,
      const RotMatrix<dim>& rotation = RotMatrix<dim>().identity()) const
        {Polygon p(*this); p.m_orient = m_orient.toLocalCoords(origin, rotation); return p;}
  Polygon toLocalCoords(const AxisBox<dim>& coords) const
        {Polygon p(*this); p.m_orient = m_orient.toLocalCoords(coords); return p;}
  Polygon toLocalCoords(const RotBox<dim>& coords) const
        {Polygon p(*this); p.m_orient = m_orient.toLocalCoords(coords); return p;}

  // 3D only
  Polygon<3> toParentCoords(const Point<3>& origin, const Quaternion& rotation) const
        {Polygon<3> p(*this); p.m_orient = m_orient.toParentCoords(origin, rotation); return p;}
  Polygon<3> toLocalCoords(const Point<3>& origin, const Quaternion& rotation) const
        {Polygon<3> p(*this); p.m_orient = m_orient.toLocalCoords(origin, rotation); return p;}

  friend bool Intersect<dim>(const Polygon& r, const Point<dim>& p, bool proper);
  friend bool Contains<dim>(const Point<dim>& p, const Polygon& r, bool proper);

  friend bool Intersect<dim>(const Polygon& p, const AxisBox<dim>& b, bool proper);
  friend bool Contains<dim>(const Polygon& p, const AxisBox<dim>& b, bool proper);
  friend bool Contains<dim>(const AxisBox<dim>& b, const Polygon& p, bool proper);

  friend bool Intersect<dim>(const Polygon& p, const Ball<dim>& b, bool proper);
  friend bool Contains<dim>(const Polygon& p, const Ball<dim>& b, bool proper);
  friend bool Contains<dim>(const Ball<dim>& b, const Polygon& p, bool proper);

  friend bool Intersect<dim>(const Polygon& r, const Segment<dim>& s, bool proper);
  friend bool Contains<dim>(const Polygon& p, const Segment<dim>& s, bool proper);
  friend bool Contains<dim>(const Segment<dim>& s, const Polygon& p, bool proper);

  friend bool Intersect<dim>(const Polygon& p, const RotBox<dim>& r, bool proper);
  friend bool Contains<dim>(const Polygon& p, const RotBox<dim>& r, bool proper);
  friend bool Contains<dim>(const RotBox<dim>& r, const Polygon& p, bool proper);

  friend bool Intersect<dim>(const Polygon& p1, const Polygon& p2, bool proper);
  friend bool Contains<dim>(const Polygon& outer, const Polygon& inner, bool proper);

 private:

  _Poly2Orient<dim> m_orient;
  Polygon<2> m_poly;
};

} // namespace WFMath

#endif  // WFMATH_POLYGON_H
