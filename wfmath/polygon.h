// -*-C++-*-
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

#ifndef WFMATH_POLYGON_H
#define WFMATH_POLYGON_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/rotmatrix.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/intersect_decls.h>

#include <vector>

namespace WFMath {

template<const int dim> class Polygon;

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Polygon<dim>& r);
template<const int dim>
std::istream& operator>>(std::istream& is, Polygon<dim>& r);

// The 2D specialization

template<>
class Polygon<2>
{
 public:
  Polygon() {}
  Polygon(const Polygon& p) : m_points(p.m_points) {}

  ~Polygon() {}

  friend std::ostream& operator<< <2>(std::ostream& os, const Polygon& p);
  friend std::istream& operator>> <2>(std::istream& is, Polygon& p);

  Polygon& operator=(const Polygon& p)
	{m_points = p.m_points; return *this;}

  bool isEqualTo(const Polygon& p, double tolerance = WFMATH_EPSILON) const;

  bool operator==(const Polygon& p) const	{return isEqualTo(p);}
  bool operator!=(const Polygon& p) const	{return !isEqualTo(p);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the polygon.
  bool operator< (const Polygon& p) const;

  // Descriptive characteristics

  int numCorners() const {return m_points.size();}
  Point<2> getCorner(int i) const {return m_points[i];}
  Point<2> getCenter() const {return Barycenter(m_points);}

  // For a Polygon<2>, addCorner() and moveCorner() always succeed.
  // The return values are present for the sake of a unified template
  // interface, and the epsilon argument is ignored

  // Add before i'th corner, zero is beginning, numCorners() is end
  bool addCorner(int i, const Point<2>& p, double epsilon = WFMATH_EPSILON)
	{m_points.insert(m_points.begin() + i, p); return true;}

  // Remove the i'th corner
  void removeCorner(int i) {m_points.erase(m_points.begin() + i);}

  // Move the i'th corner to p
  bool moveCorner(int i, const Point<2>& p, double epsilon = WFMATH_EPSILON)
	{m_points[i] = p; return true;}

  // Remove all points
  void clear()	{m_points.clear();}

  const Point<2>& operator[](int i) const {return m_points[i];}
  Point<2>& operator[](int i)		  {return m_points[i];}

  void resize(unsigned int size) {m_points.resize(size);}

  // Movement functions

  Polygon& shift(const Vector<2>& v);
  Polygon& moveCornerTo(const Point<2>& p, int corner)
	{return shift(p - getCorner(corner));}
  Polygon& moveCenterTo(const Point<2>& p)
	{return shift(p - getCenter());}

  Polygon& rotateCorner(const RotMatrix<2>& m, int corner)
	{rotatePoint(m, getCorner(corner)); return *this;}
  Polygon& rotateCenter(const RotMatrix<2>& m)
	{rotatePoint(m, getCenter()); return *this;}
  Polygon& rotatePoint(const RotMatrix<2>& m, const Point<2>& p);

  // Intersection functions

  AxisBox<2> boundingBox() const {return BoundingBox(m_points);}
  Ball<2> boundingSphere() const {return BoundingSphere(m_points);}
  Ball<2> boundingSphereSloppy() const {return BoundingSphereSloppy(m_points);}

  friend bool Intersect<2>(const Polygon& r, const Point<2>& p);
  friend bool IntersectProper<2>(const Polygon& r, const Point<2>& p);
  friend bool Contains<2>(const Point<2>& p, const Polygon& r);

  friend bool Intersect<2>(const Polygon& p, const AxisBox<2>& b);
  friend bool IntersectProper<2>(const Polygon& p, const AxisBox<2>& b);
  friend bool Contains<2>(const Polygon& p, const AxisBox<2>& b);
  friend bool ContainsProper<2>(const Polygon& p, const AxisBox<2>& b);
  friend bool Contains<2>(const AxisBox<2>& b, const Polygon& p);
  friend bool ContainsProper<2>(const AxisBox<2>& b, const Polygon& p);

  friend bool Intersect<2>(const Polygon& p, const Ball<2>& b);
  friend bool IntersectProper<2>(const Polygon& p, const Ball<2>& b);
  friend bool Contains<2>(const Polygon& p, const Ball<2>& b);
  friend bool ContainsProper<2>(const Polygon& p, const Ball<2>& b);
  friend bool Contains<2>(const Ball<2>& b, const Polygon& p);
  friend bool ContainsProper<2>(const Ball<2>& b, const Polygon& p);

  friend bool Intersect<2>(const Polygon& r, const Segment<2>& s);
  friend bool IntersectProper<2>(const Polygon& p, const Segment<2>& s);
  friend bool Contains<2>(const Polygon& p, const Segment<2>& s);
  friend bool ContainsProper<2>(const Polygon& p, const Segment<2>& s);
  friend bool Contains<2>(const Segment<2>& s, const Polygon& p);
  friend bool ContainsProper<2>(const Segment<2>& s, const Polygon& p);

  friend bool Intersect<2>(const Polygon& p, const RotBox<2>& r);
  friend bool IntersectProper<2>(const Polygon& p, const RotBox<2>& r);
  friend bool Contains<2>(const Polygon& p, const RotBox<2>& r);
  friend bool ContainsProper<2>(const Polygon& p, const RotBox<2>& r);
  friend bool Contains<2>(const RotBox<2>& r, const Polygon& p);
  friend bool ContainsProper<2>(const RotBox<2>& r, const Polygon& p);

  friend bool Intersect<2>(const Polygon& p1, const Polygon& p2);
  friend bool IntersectProper<2>(const Polygon& p1, const Polygon& p2);
  friend bool Contains<2>(const Polygon& outer, const Polygon& inner);
  friend bool ContainsProper<2>(const Polygon& outer, const Polygon& inner);

 private:
  vector<Point<2> > m_points;
  typedef vector<Point<2> >::iterator theIter;
  typedef vector<Point<2> >::const_iterator theConstIter;

};

// Helper classes, to keep track of the orientation of
// a 2D polygon in dim dimensions

typedef enum {
  _WFMATH_POLY2REORIENT_NONE,
  _WFMATH_POLY2REORIENT_CLEAR_AXIS1,
  _WFMATH_POLY2REORIENT_CLEAR_AXIS2,
  _WFMATH_POLY2REORIENT_CLEAR_BOTH_AXES,
  _WFMATH_POLY2REORIENT_MOVE_AXIS2_TO_AXIS1,
  _WFMATH_POLY2REORIENT_SCALE1_CLEAR2, // Will be needed if we normalize the axes
} _Poly2ReorientType;

// Reorient a 2D polygon to match a change in the basis
// used by _Poly2Orient
class _Poly2Reorient
{
 public:
  _Poly2Reorient(_Poly2ReorientType type, CoordType scale = 0.0)
	: m_type(type), m_scale(scale) {}
  ~_Poly2Reorient() {}

  void reorient(Polygon<2>& poly, int skip = -1) const;

 private:
  _Poly2ReorientType m_type;
  CoordType m_scale;
};

// Keep track of the orientation of a 2D polygon in dim dimensions
template<const int dim>
class _Poly2Orient
{
 public:
  _Poly2Orient()
	{m_origin_valid = false; m_axes_valid[0] = false; m_axes_valid[1] = false;}
  _Poly2Orient(const _Poly2Orient& p)	{operator=(p);}
  ~_Poly2Orient() {}

  _Poly2Orient& operator=(const _Poly2Orient& p);

  // Convert a point in the 2D polygon to a point in dim dimensional space
  Point<dim> convert(const Point<2>& p) const;

  // Try to convert a point from dim dimensions into 2D, expanding the
  // basis if necessary. Returns true on success. On failure, the
  // basis is unchanged.
  bool expand(const Point<dim>& pd, Point<2>& p2, double epsilon = WFMATH_EPSILON);

  // Reduce the basis to the minimum necessary to span the points in
  // poly (with the exception of skip). Returns _Poly2Reorient, which needs
  // to be used to reorient the points to match the new basis.
  _Poly2Reorient reduce(const Polygon<2>& poly, int skip = -1);

  void shift(const Vector<dim>& v) {if(m_origin_valid) m_origin += v;}
  void rotate(const RotMatrix<dim>& m, const Point<dim>& p);
  // Rotates about the point which corresponds to "p" in the oriented plane
  void rotate(const RotMatrix<dim>& m, const Point<2>& p);

  // Gives the offset from pd to the space spanned by
  // the basis, and puts the nearest point in p2.
  Vector<dim> offset(const Point<dim>& pd, Point<2>& p2) const;

  // Check if the AxisBox intersects the spanned space, and if so
  // return a point in the intersection.
  bool checkIntersect(const AxisBox<dim>& b, Point<2>& p2) const;
  bool checkIntersectProper(const AxisBox<dim>& b, Point<2>& p2) const;

 private:
  // special case of the above when both axes are valid
  bool checkIntersectPlane(const AxisBox<dim>& b, Point<2>& p2) const;

  Point<dim> m_origin;
  Vector<dim> m_axes[2]; // Normalized to unit length
  bool m_origin_valid, m_axes_valid[2];
};

template<const int dim>
class Polygon
{
 public:
  Polygon() {}
  Polygon(const Polygon& p) : m_orient(p.m_orient), m_poly(p.m_poly) {}

  ~Polygon() {}

  friend std::ostream& operator<< <dim>(std::ostream& os, const Polygon& p);
  friend std::istream& operator>> <dim>(std::istream& is, Polygon& p);

  Polygon& operator=(const Polygon& p)
	{m_orient = p.m_orient; m_poly = p.m_poly; return *this;}

  bool isEqualTo(const Polygon& p, double tolerance = WFMATH_EPSILON) const;

  bool operator==(const Polygon& p) const	{return isEqualTo(p);}
  bool operator!=(const Polygon& p) const	{return !isEqualTo(p);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the polygon.
  bool operator< (const Polygon& p) const;

  // Descriptive characteristics

  int numCorners() const {return m_poly.numCorners();}
  Point<dim> getCorner(int i) const {return m_orient.convert(m_poly[i]);}
  Point<dim> getCenter() const {return m_orient.convert(m_poly.getCenter());}

  // The failure of the following functions does not invalidate the
  // polygon, but merely leaves it unchaged.

  // Add before i'th corner, zero is beginning, numCorners() is end
  // Only succeeds if p lies in a plane with all current points
  bool addCorner(int i, const Point<dim>& p, double epsilon = WFMATH_EPSILON);

  // Remove the i'th corner
  void removeCorner(int i);

  // Move the i'th corner to p, only succeeds if new location
  // lies in the same plane as all the other points. Note that,
  // under certain circumstances, this plane may not contain the
  // original location of the point.
  bool moveCorner(int i, const Point<dim>& p, double epsilon = WFMATH_EPSILON);

  // Remove all points
  void clear()	{m_poly.clear(); m_orient = _Poly2Orient<dim>();}

  // Movement functions

  Polygon& shift(const Vector<dim>& v)
	{m_orient.shift(v); return *this;}
  Polygon& moveCornerTo(const Point<dim>& p, int corner)
	{return shift(p - getCorner(corner));}
  Polygon& moveCenterTo(const Point<dim>& p)
	{return shift(p - getCenter());}

  Polygon& rotateCorner(const RotMatrix<dim>& m, int corner)
	{m_orient.rotate(m, m_poly[corner]); return *this;}
  Polygon& rotateCenter(const RotMatrix<dim>& m)
	{if(m_poly.numCorners() > 0)
		m_orient.rotate(m, m_poly.getCenter());
	 return *this;}
  Polygon& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p)
	{m_orient.rotate(m, p); return *this;}

  // Intersection functions

  AxisBox<dim> boundingBox() const;
  Ball<dim> boundingSphere() const;
  Ball<dim> boundingSphereSloppy() const;

  friend bool Intersect<dim>(const Polygon& r, const Point<dim>& p);
  friend bool IntersectProper<dim>(const Polygon& r, const Point<dim>& p);
  friend bool Contains<dim>(const Point<dim>& p, const Polygon& r);

  friend bool Intersect<dim>(const Polygon& p, const AxisBox<dim>& b);
  friend bool IntersectProper<dim>(const Polygon& p, const AxisBox<dim>& b);
  friend bool Contains<dim>(const Polygon& p, const AxisBox<dim>& b);
  friend bool ContainsProper<dim>(const Polygon& p, const AxisBox<dim>& b);
  friend bool Contains<dim>(const AxisBox<dim>& b, const Polygon& p);
  friend bool ContainsProper<dim>(const AxisBox<dim>& b, const Polygon& p);

  friend bool Intersect<dim>(const Polygon& p, const Ball<dim>& b);
  friend bool IntersectProper<dim>(const Polygon& p, const Ball<dim>& b);
  friend bool Contains<dim>(const Polygon& p, const Ball<dim>& b);
  friend bool ContainsProper<dim>(const Polygon& p, const Ball<dim>& b);
  friend bool Contains<dim>(const Ball<dim>& b, const Polygon& p);
  friend bool ContainsProper<dim>(const Ball<dim>& b, const Polygon& p);

  friend bool Intersect<dim>(const Polygon& r, const Segment<dim>& s);
  friend bool IntersectProper<dim>(const Polygon& p, const Segment<dim>& s);
  friend bool Contains<dim>(const Polygon& p, const Segment<dim>& s);
  friend bool ContainsProper<dim>(const Polygon& p, const Segment<dim>& s);
  friend bool Contains<dim>(const Segment<dim>& s, const Polygon& p);
  friend bool ContainsProper<dim>(const Segment<dim>& s, const Polygon& p);

  friend bool Intersect<dim>(const Polygon& p, const RotBox<dim>& r);
  friend bool IntersectProper<dim>(const Polygon& p, const RotBox<dim>& r);
  friend bool Contains<dim>(const Polygon& p, const RotBox<dim>& r);
  friend bool ContainsProper<dim>(const Polygon& p, const RotBox<dim>& r);
  friend bool Contains<dim>(const RotBox<dim>& r, const Polygon& p);
  friend bool ContainsProper<dim>(const RotBox<dim>& r, const Polygon& p);

  friend bool Intersect<dim>(const Polygon& p1, const Polygon& p2);
  friend bool IntersectProper<dim>(const Polygon& p1, const Polygon& p2);
  friend bool Contains<dim>(const Polygon& outer, const Polygon& inner);
  friend bool ContainsProper<dim>(const Polygon& outer, const Polygon& inner);

 private:

  _Poly2Orient<dim> m_orient;
  Polygon<2> m_poly;
};

} // namespace WFMath

#include <wfmath/polygon_funcs.h>

#endif  // WFMATH_POLYGON_H
