// polygon_funcs.h (Polygon<> intersection functions)
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
// Created: 2002-2-20

#ifndef WFMATH_POLYGON_INTERSECT_H
#define WFMATH_POLYGON_INTERSECT_H

#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/polygon.h>
#include <wfmath/intersect.h>
#include <wfmath/error.h>

#include <cmath>

#include <cassert>

// FIXME Work is needed on this code. At very least the following notes
// from the original author apply:
// "The Intersect() and Contains() functions involving WFMath::Polygon<>"
// "are still under development, and probably shouldn't be used yet."

namespace WFMath {

template<int dim>
inline Vector<dim> _Poly2Orient<dim>::offset(const Point<dim>& pd, Point<2>& p2) const
{
  assert(m_origin.isValid()); // Check for empty polygon before calling this

  Vector<dim> out = pd - m_origin;

  for(int j = 0; j < 2; ++j) {
    p2[j] = Dot(out, m_axes[j]);
    out -= p2[j] * m_axes[j];
  }

  return out;
}

template<int dim>
inline bool _Poly2Orient<dim>::checkContained(const Point<dim>& pd, Point<2> & p2) const
{
  Vector<dim> off = offset(pd, p2);

  CoordType sqrsum = 0;
  for(int i = 0; i < dim; ++i)
    sqrsum += pd[i] * pd[i];

  return off.sqrMag() < WFMATH_EPSILON * sqrsum;
}

template<>
bool _Poly2Orient<3>::checkIntersectPlane(const AxisBox<3>& b, Point<2>& p2,
					  bool proper) const;

template<int dim>
bool _Poly2Orient<dim>::checkIntersect(const AxisBox<dim>& b, Point<2>& p2,
				       bool proper) const
{
  assert(m_origin.isValid());

  if(!m_axes[0].isValid()) {
    // Single point
    p2[0] = p2[1] = 0;
    return Intersect(b, convert(p2), proper);
  }

  if(m_axes[1].isValid()) {
    // A plane

    // I only know how to do this in 3D, so write a function which will
    // specialize to different dimensions

    return checkIntersectPlane(b, p2, proper);
  }

  // A line

  // This is a modified version of AxisBox<>/Segment<> intersection

  CoordType min = 0, max = 0; // Initialize to avoid compiler warnings
  bool got_bounds = false;

  for(int i = 0; i < dim; ++i) {
    const CoordType dist = (m_axes[0])[i]; // const may optimize away better
    if(dist == 0) {
      if(_Less(m_origin[i], b.lowCorner()[i], proper)
        || _Greater(m_origin[i], b.highCorner()[i], proper))
        return false;
    }
    else {
      CoordType low = (b.lowCorner()[i] - m_origin[i]) / dist;
      CoordType high = (b.highCorner()[i] - m_origin[i]) / dist;
      if(low > high) {
        CoordType tmp = high;
        high = low;
        low = tmp;
      }
      if(got_bounds) {
        if(low > min)
          min = low;
        if(high < max)
          max = high;
      }
      else {
        min = low;
        max = high;
        got_bounds = true;
      }
    }
  }

  assert(got_bounds); // We can't be parallel in _all_ dimensions

  if(_LessEq(min, max, proper)) {
    p2[0] = (max - min) / 2;
    p2[1] = 0;
    return true;
  }
  else
    return false;
}

template<int dim>
int  _Intersect(const _Poly2Orient<dim> &o1, const _Poly2Orient<dim> &o2,
	        _Poly2OrientIntersectData &data)
{
  if(!o1.m_origin.isValid() || !o2.m_origin.isValid()) { // No points
    return -1;
  }

  // Check for single point basis

  if(!o1.m_axes[0].isValid()) {
    if(!o2.checkContained(o1.m_origin, data.p2))
      return -1; // no intersect

    _Poly2OrientIntersectData data;

    data.p1[0] = data.p1[1] = 0;

    return 0; // point intersect
  }

  if(!o2.m_axes[0].isValid()) {
    if(!o1.checkContained(o2.m_origin, data.p1))
      return -1; // no intersect

    data.p2[0] = data.p2[1] = 0;

    return 0; // point intersect
  }

  // Find a common basis for the plane's orientations
  // by projecting out the part of o1's basis that lies
  // in o2's basis

  Vector<dim> basis1, basis2;
  CoordType sqrmag1, sqrmag2;
  int basis_size = 0;

  basis1 = o2.m_axes[0] * Dot(o2.m_axes[0], o1.m_axes[0]);
  if(o2.m_axes[1].isValid())
    basis1 += o2.m_axes[1] * Dot(o2.m_axes[1], o1.m_axes[0]);

  // Don't need to scale, the m_axes are unit vectors
  sqrmag1 = basis1.sqrMag();
  if(sqrmag1 > WFMATH_EPSILON * WFMATH_EPSILON)
    basis_size = 1;

  if(o1.m_axes[1].isValid()) {
    basis2 = o2.m_axes[0] * Dot(o2.m_axes[0], o1.m_axes[1]);
    if(o2.m_axes[1].isValid())
      basis2 += o2.m_axes[1] * Dot(o2.m_axes[1], o1.m_axes[1]);

    // Project out part parallel to basis1
    if(basis_size == 1)
      basis2 -= basis1 * (Dot(basis1, basis2) / sqrmag1);

    sqrmag2 = basis2.sqrMag();
    if(sqrmag2 > WFMATH_EPSILON * WFMATH_EPSILON) {
      if(basis_size++ == 0) {
        basis1 = basis2;
        sqrmag1 = sqrmag2;
      }
    }
  }

  Vector<dim> off = o2.m_origin - o1.m_origin;

  switch(basis_size) {
    case 0:
      {
      // All vectors are orthogonal, check for a common point in the plane
      // This can happen even in 3d for degenerate bases

      data.p1[0] = Dot(o1.m_axes[0], off);
      Vector<dim> off1 = o1.m_axes[0] * data.p1[0];
      if(o1.m_axes[1].isValid()) {
        data.p1[1] = Dot(o1.m_axes[1], off);
        off1 += o1.m_axes[1] * data.p1[1];
      }
      else
        data.p1[1] = 0;

      data.p2[0] = -Dot(o2.m_axes[0], off);
      Vector<dim> off2 = o2.m_axes[0] * data.p2[0];
      if(o1.m_axes[1].isValid()) {
        data.p2[1] = -Dot(o2.m_axes[1], off);
        off2 += o1.m_axes[1] * data.p2[1];
      }
      else
        data.p2[1] = 0;

      if(off1 - off2 != off) // No common point
        return -1;
      else  // Got a point
        return 1;
      }
    case 1:
      {
      // Check for an intersection line

      data.o1_is_line = !o1.m_axes[1].isValid();
      data.o2_is_line = !o2.m_axes[1].isValid();

      if(!o1.m_axes[1].isValid() && !o2.m_axes[1].isValid()) {
        CoordType proj = Dot(off, o2.m_axes[0]);
        if(off != o2.m_axes[0] * proj)
          return -1;

        data.v1[0] = 1;
        data.v1[1] = 0;
        data.p1[0] = data.p1[1] = 0;
        data.v2[0] = (Dot(o1.m_axes[0], o2.m_axes[0]) > 0) ? 1 : -1;
        data.v2[1] = 0;
        data.p2[0] = -proj;
        data.p2[1] = 0;

        return 1;
      }

      if(!o1.m_axes[1].isValid()) {
        data.p2[0] = -Dot(off, o2.m_axes[0]);
        data.p2[1] = -Dot(off, o2.m_axes[1]);

        if(off != - data.p2[0] * o2.m_axes[0] - data.p2[1] * o2.m_axes[1])
          return -1;

        data.v1[0] = 1;
        data.v1[1] = 0;
        data.p1[0] = data.p1[1] = 0;
        data.v2[0] = Dot(o1.m_axes[0], o2.m_axes[0]);
        data.v2[1] = Dot(o1.m_axes[0], o2.m_axes[1]);

        return 1;
      }

      if(!o2.m_axes[1].isValid()) {
        data.p1[0] = Dot(off, o1.m_axes[0]);
        data.p1[1] = Dot(off, o1.m_axes[1]);

        if(off != data.p1[0] * o1.m_axes[0] + data.p1[1] * o1.m_axes[1])
          return -1;

        data.v2[0] = 1;
        data.v2[1] = 0;
        data.p2[0] = data.p2[1] = 0;
        data.v1[0] = Dot(o1.m_axes[0], o2.m_axes[0]);
        data.v1[1] = Dot(o1.m_axes[1], o2.m_axes[0]);

        return 1;
      }

      data.p1[0] = Dot(off, o1.m_axes[0]);
      data.p1[1] = Dot(off, o1.m_axes[1]);
      data.p2[0] = -Dot(off, o2.m_axes[0]);
      data.p2[1] = -Dot(off, o2.m_axes[1]);

      if(off != data.p1[0] * o1.m_axes[0] + data.p1[1] * o1.m_axes[1]
        - data.p2[0] * o2.m_axes[0] - data.p2[1] * o2.m_axes[1])
        return -1;

      basis1 /= std::sqrt(sqrmag1);

      data.v1[0] = Dot(o1.m_axes[0], basis1);
      data.v1[1] = Dot(o1.m_axes[1], basis1);
      data.v2[0] = Dot(o2.m_axes[0], basis1);
      data.v2[1] = Dot(o2.m_axes[1], basis1);

      return 1;
      }
    case 2:
      {
      assert(o1.m_axes[1].isValid() && o2.m_axes[1].isValid());

      // The planes are parallel, check if they are the same plane
      CoordType off_sqr_mag = data.off.sqrMag();

      // Find the offset between the origins in o2's coordnates

      if(off_sqr_mag != 0) { // The offsets aren't identical
        Vector<dim> off_copy = off;

        data.off[0] = Dot(o2.m_axes[0], off);
        off_copy -= o1.m_axes[0] * data.off[0];
        data.off[1] = Dot(o2.m_axes[1], off);
        off_copy -= o1.m_axes[1] * data.off[1];

        if(off_copy.sqrMag() > off_sqr_mag * WFMATH_EPSILON)
          return -1; // The planes are different
      }
      else
        data.off[0] = data.off[1] = 0;

      // Define o2's basis vectors in o1's coordinates
      data.v1[0] = Dot(o2.m_axes[0], o1.m_axes[0]);
      data.v1[1] = Dot(o2.m_axes[0], o1.m_axes[1]);
      data.v2[0] = Dot(o2.m_axes[1], o1.m_axes[0]);
      data.v2[1] = Dot(o2.m_axes[1], o1.m_axes[1]);

      return 2;
      }
    default:
      assert(false);
      return -1;
  }
}

template<int dim>
inline bool Intersect(const Polygon<dim>& r, const Point<dim>& p, bool proper)
{
  Point<2> p2;

  return r.m_poly.numCorners() > 0 && r.m_orient.checkContained(p, p2)
	 && Intersect(r.m_poly, p2, proper);
}

template<int dim>
inline bool Contains(const Point<dim>& p, const Polygon<dim>& r, bool proper)
{
  if(r.m_poly.numCorners() == 0)
    return true;

  if(proper)
    return false;

  for(int i = 1; i < r.m_poly.numCorners(); ++i)
    if(r.m_poly[i] != r.m_poly[0])
      return false;

  Point<2> p2;

  return r.m_orient.checkContained(p, p2) && p2 == r.m_poly[0];
}

template<int dim>
bool Intersect(const Polygon<dim>& p, const AxisBox<dim>& b, bool proper)
{
  int corners = p.m_poly.numCorners();

  if(corners == 0)
    return false;

  Point<2> p2;

  if(!p.m_orient.checkIntersect(b, p2, proper))
    return false;

  Segment<dim> s;
  s.endpoint(0) = p.m_orient.convert(p.m_poly.getCorner(corners-1));
  int next_end = 1;

  for(int i = 0; i < corners; ++i) {
    s.endpoint(next_end) = p.m_orient.convert(p.m_poly.getCorner(i));
    if(Intersect(b, s, proper))
      return true;
    next_end = next_end ? 0 : 1;
  }

  return Contains(p, p2, proper);
}

template<int dim>
bool _PolyContainsBox(const _Poly2Orient<dim> &orient, const Polygon<2> &poly,
		  const Point<dim> &corner, const Vector<dim> &size, bool proper)
{
  int num_dim = 0, nonzero_dim = -1;

  for(int i = 0; i < dim; ++i) {
    if(size[i] == 0)
      continue;
    if(num_dim == 2)
      return false;
    if(nonzero_dim == -1 || fabs(size[nonzero_dim]) < fabs(size[i]));
      nonzero_dim = i;
    ++num_dim;
  }

  Point<2> corner1;

  if(!orient.checkContained(corner, corner1))
    return false;

  if(num_dim == 0)
    return Contains(poly, corner1, proper);

  Point<2> corner2;

  if(!orient.checkContained(corner + size, corner2))
    return false;

  if(num_dim == 1)
    return Contains(poly, Segment<2>(corner1, corner2), proper);

  Point<dim> other_corner = corner;
  other_corner[nonzero_dim] += size[nonzero_dim];

  Point<2> corner3;
  if(!orient.checkContained(other_corner, corner3))
    return false;

  // Create a RotBox<2>

  Vector<2> vec1(corner2 - corner1), vec2(corner3 - corner1);

  RotMatrix<2> m; // A matrix which gives the rotation from the x-axis to vec1

  try {
    m.rotation(Vector<2>(1, 0), vec1);
  }
  catch(ColinearVectors<2>) { // vec1 is parallel to (-1, 0), so we're fine
    m.identity();
  }

  RotBox<2> box(corner1, ProdInv(vec2, m), m);

  return Contains(poly, box, proper);
}

template<int dim>
inline bool Contains(const Polygon<dim>& p, const AxisBox<dim>& b, bool proper)
{
  return _PolyContainsBox(p.m_orient, p.m_poly, b.m_low, b.m_high - b.m_low, proper);
}

template<int dim>
inline bool Contains(const AxisBox<dim>& b, const Polygon<dim>& p, bool proper)
{
  for(int i = 0; i < p.m_poly.numCorners(); ++i)
    if(!Contains(b, p.getCorner(i), proper))
      return false;

  return true;
}

template<int dim>
inline bool Intersect(const Polygon<dim>& p, const Ball<dim>& b, bool proper)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Point<2> c2;
  CoordType dist;

  dist = b.m_radius * b.m_radius - p.m_orient.offset(b.m_center, c2).sqrMag();

  if(_Less(dist, 0, proper))
    return false;

  return Intersect(p.m_poly, Ball<2>(c2, std::sqrt(dist)), proper);
}

template<int dim>
inline bool Contains(const Polygon<dim>& p, const Ball<dim>& b, bool proper)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  if(b.m_radius > 0)
    return false;

  Point<2> c2;

  if(!p.m_orient.checkContained(b.m_center, c2))
    return false;

  return Contains(p.m_poly, c2, proper);
}

template<int dim>
inline bool Contains(const Ball<dim>& b, const Polygon<dim>& p, bool proper)
{
  if(p.m_poly.numCorners() == 0)
    return true;

  Point<2> c2;
  CoordType dist;

  dist = b.m_radius * b.m_radius - p.m_orient.offset(b.m_center, c2).sqrMag();

  if(_Less(dist, 0, proper))
    return false;

  for(int i = 0; i != p.m_poly.numCorners(); ++i)
    if(_Less(dist, SquaredDistance(c2, p.m_poly[i]), proper))
      return false;

  return true;
}

template<int dim>
bool Intersect(const Polygon<dim>& p, const Segment<dim>& s, bool proper)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Point<2> p1, p2;
  CoordType d1, d2;
  Vector<dim> v1, v2;

  v1 = p.m_orient.offset(s.m_p1, p1);
  v2 = p.m_orient.offset(s.m_p2, p2);

  if(Dot(v1, v2) > 0) // Both points on same side of sheet
    return false;

  d1 = v1.mag();
  d2 = v2.mag();
  Point<2> p_intersect;

  if(d1 + d2 == 0) // Avoid divide by zero later
    return Intersect(p.m_poly, Segment<2>(p1, p2), proper);

  for(int i = 0; i < 2; ++i)
    p_intersect[i] = (p1[i] * d2 + p2[i] * d1) / (d1 + d2);

  return Intersect(p.m_poly, p_intersect, proper);
}

template<int dim>
inline bool Contains(const Polygon<dim>& p, const Segment<dim>& s, bool proper)
{
  if(p.m_poly.numCorners() == 0)
    return false;

  Segment<2> s2;

  if(!p.m_orient.checkContained(s.m_p1, s2.endpoint(0)))
    return false;
  if(!p.m_orient.checkContained(s.m_p2, s2.endpoint(1)))
    return false;

  return Contains(p.m_poly, s2, proper);
}

template<int dim>
inline bool Contains(const Segment<dim>& s, const Polygon<dim>& p, bool proper)
{
  if(p.m_poly.numCorners() == 0)
    return true;

  // Expand the basis to include the segment, this deals well with
  // degenerate polygons

  Segment<2> s2;
  _Poly2Orient<dim> orient(p.m_orient);

  for(int i = 0; i < 2; ++i)
    if(!orient.expand(s.endpoint(i), s2.endpoint(i)))
      return false;

  return Contains(s2, p.m_poly, proper);
}

template<int dim>
bool Intersect(const Polygon<dim>& p, const RotBox<dim>& r, bool proper)
{
  int corners = p.m_poly.numCorners();

  if(corners == 0)
    return false;

  _Poly2Orient<dim> orient(p.m_orient);
  // FIXME rotateInverse()
  orient.rotate(r.m_orient.inverse(), r.m_corner0);

  AxisBox<dim> b(r.m_corner0, r.m_corner0 + r.m_size);

  Point<2> p2;

  if(!orient.checkIntersect(b, p2, proper))
    return false;

  Segment<dim> s;
  s.endpoint(0) = orient.convert(p.m_poly.getCorner(corners-1));
  int next_end = 1;

  for(int i = 0; i < corners; ++i) {
    s.endpoint(next_end) = orient.convert(p.m_poly.getCorner(i));
    if(Intersect(b, s, proper))
      return true;
    next_end = next_end ? 0 : 1;
  }

  return Contains(p, p2, proper);
}

template<int dim>
inline bool Contains(const Polygon<dim>& p, const RotBox<dim>& r, bool proper)
{
  _Poly2Orient<dim> orient(p.m_orient);
  orient.rotate(r.m_orient.inverse(), r.m_corner0);

  return _PolyContainsBox(orient, p.m_poly, r.m_corner0, r.m_size, proper);
}

template<int dim>
inline bool Contains(const RotBox<dim>& r, const Polygon<dim>& p, bool proper)
{
  if(p.m_poly.numCorners() == 0)
    return true;

  AxisBox<dim> b(r.m_corner0, r.m_corner0 + r.m_size);

  _Poly2Orient<dim> orient(p.m_orient);
  orient.rotate(r.m_orient.inverse(), r.m_corner0);

  for(int i = 0; i < p.m_poly.numCorners(); ++i)
    if(!Contains(b, orient.convert(p.m_poly[i]), proper))
      return false;

  return true;
}

bool _PolyPolyIntersect(const Polygon<2> &poly1, const Polygon<2> &poly2,
			const int intersect_dim,
			const _Poly2OrientIntersectData &data, bool proper);

template<int dim>
inline bool Intersect(const Polygon<dim>& p1, const Polygon<dim>& p2, bool proper)
{
  _Poly2OrientIntersectData data;

  int intersect_dim = _Intersect(p1.m_orient, p2.m_orient, data);

  return _PolyPolyIntersect(p1.m_poly, p2.m_poly, intersect_dim, data, proper);
}

bool _PolyPolyContains(const Polygon<2> &outer, const Polygon<2> &inner,
		       const int intersect_dim,
		       const _Poly2OrientIntersectData &data, bool proper);

template<int dim>
inline bool Contains(const Polygon<dim>& outer, const Polygon<dim>& inner, bool proper)
{
  if(outer.m_poly.numCorners() == 0)
    return !proper && inner.m_poly.numCorners() == 0;

  if(inner.m_poly.numCorners() == 0)
    return true;

  _Poly2OrientIntersectData data;

  int intersect_dim = _Intersect(outer.m_orient, inner.m_orient, data);

  return _PolyPolyContains(outer.m_poly, inner.m_poly, intersect_dim, data, proper);
}

template<>
bool Intersect(const Polygon<2>& r, const Point<2>& p, bool proper);
template<>
bool Contains(const Point<2>& p, const Polygon<2>& r, bool proper);

template<>
bool Intersect(const Polygon<2>& p, const AxisBox<2>& b, bool proper);
template<>
bool Contains(const Polygon<2>& p, const AxisBox<2>& b, bool proper);
template<>
bool Contains(const AxisBox<2>& b, const Polygon<2>& p, bool proper);

template<>
bool Intersect(const Polygon<2>& p, const Ball<2>& b, bool proper);
template<>
bool Contains(const Polygon<2>& p, const Ball<2>& b, bool proper);
template<>
bool Contains(const Ball<2>& b, const Polygon<2>& p, bool proper);

template<>
bool Intersect(const Polygon<2>& r, const Segment<2>& s, bool proper);
template<>
bool Contains(const Polygon<2>& p, const Segment<2>& s, bool proper);
template<>
bool Contains(const Segment<2>& s, const Polygon<2>& p, bool proper);

template<>
bool Intersect(const Polygon<2>& p, const RotBox<2>& r, bool proper);
template<>
bool Contains(const Polygon<2>& p, const RotBox<2>& r, bool proper);
template<>
bool Contains(const RotBox<2>& r, const Polygon<2>& p, bool proper);

template<>
bool Intersect(const Polygon<2>& p1, const Polygon<2>& p2, bool proper);
template<>
bool Contains(const Polygon<2>& outer, const Polygon<2>& inner, bool proper);

} // namespace WFMath

#endif  // WFMATH_POLYGON_INTERSECT_H
