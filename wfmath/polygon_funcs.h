// polygon_funcs.h (line polygon implementation)
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

#ifndef WFMATH_POLYGON_FUNCS_H
#define WFMATH_POLYGON_FUNCS_H

#include <wfmath/polygon.h>

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/ball.h>

#include <cmath>

#include <cassert>
#include <limits>

namespace WFMath {

template<int dim>
inline _Poly2Orient<dim>& _Poly2Orient<dim>::operator=(const _Poly2Orient<dim>& a)
{
  m_origin = a.m_origin;

  for(int i = 0; i < 2; ++i)
    m_axes[i] = a.m_axes[i];

  return *this;
}

template<int dim>
inline bool Polygon<dim>::isEqualTo(const Polygon<dim>& p, CoordType epsilon) const
{
  // The same polygon can be expressed in different ways in the interal
  // format, so we have to call getCorner();

  size_t size = m_poly.numCorners();
  if(size != p.m_poly.numCorners())
    return false;

  for(size_t i = 0; i < size; ++i)
    if(!Equal(getCorner(i), p.getCorner(i), epsilon))
      return false;

  return true;
}

template<int dim>
inline Point<dim> _Poly2Orient<dim>::convert(const Point<2>& p) const
{
  assert(m_origin.isValid());

  Point<dim> out = m_origin;

  for(int j = 0; j < 2; ++j) {
    if(m_axes[j].isValid())
      out += m_axes[j] * p[j];
    else
      assert(p[j] == 0);
  }

  out.setValid(p.isValid());

  return out;
}

template<int dim>
bool _Poly2Orient<dim>::expand(const Point<dim>& pd, Point<2>& p2, CoordType epsilon)
{
  p2[0] = p2[1] = 0; // initialize
  p2.setValid();

  if(!m_origin.isValid()) { // Adding to an empty list
    m_origin = pd;
    m_origin.setValid();
    return true;
  }

  Vector<dim> shift = pd - m_origin, start_shift = shift;

  CoordType bound = (CoordType)(shift.sqrMag() * epsilon);

  int j = 0;

  while(true) {
    if(Dot(shift, start_shift) <= bound) // shift is effectively zero
      return true;

    if(j == 2) { // Have two axes, shift doesn't lie in their plane
      p2.setValid(false);
      return false;
    }

    if(!m_axes[j].isValid()) { // Didn't span this previously, now we do
      p2[j] = shift.mag();
      m_axes[j] = shift / p2[j];
      m_axes[j].setValid();
      return true;
   }

   p2[j] = Dot(shift, m_axes[j]);
   shift -= m_axes[j] * p2[j]; // shift is now perpendicular to m_axes[j]
   ++j;
  }
}

template<int dim>
_Poly2Reorient _Poly2Orient<dim>::reduce(const Polygon<2>& poly, size_t skip)
{
  if(poly.numCorners() <= ((skip == 0) ? 1 : 0)) { // No corners left
    m_origin.setValid(false);
    m_axes[0].setValid(false);
    m_axes[1].setValid(false);
    return _WFMATH_POLY2REORIENT_NONE;
  }

  assert(m_origin.isValid());

  if(!m_axes[0].isValid())
    return _WFMATH_POLY2REORIENT_NONE;

  // Check that we still span both axes

  bool still_valid[2] = {false,}, got_ratio = false;
  CoordType ratio = std::numeric_limits<CoordType>::max();
  CoordType size = std::numeric_limits<CoordType>::max();
  CoordType epsilon;
  size_t i, end = poly.numCorners();

  // scale epsilon
  for(i = 0; i < end; ++i) {
    if(i == skip)
      continue;
    const Point<2> &p = poly[i];
    CoordType x = std::fabs(p[0]),
              y = std::fabs(p[1]),
              max = (x > y) ? x : y;
    if(i == 0 || max < size)
      size = max;
  }
  int exponent;
  (void) std::frexp(size, &exponent);
  epsilon = std::ldexp(WFMATH_EPSILON, exponent);

  i = 0;
  if(skip == 0)
    ++i;
  assert(i != end);
  Point<2> first_point = poly[i];
  first_point.setValid(); // in case someone stuck invalid points in the poly

  while(++i != end) {
    if(i == skip)
      continue;

    Vector<2> diff = poly[i] - first_point;
    if(diff.sqrMag() < epsilon * epsilon) // No addition to span
      continue;
    if(!m_axes[1].isValid()) // We span 1D
      return _WFMATH_POLY2REORIENT_NONE;
    for(int j = 0; j < 2; ++j) {
      if(fabs(diff[j]) < epsilon) {
        assert(diff[j ? 0 : 1] >= epsilon); // because diff != 0
        if(still_valid[j ? 0 : 1] || got_ratio) // We span a 2D space
          return _WFMATH_POLY2REORIENT_NONE;
        still_valid[j] = true;
      }
    }
    // The point has both elements nonzero
    if(still_valid[0] || still_valid[1]) // We span a 2D space
      return _WFMATH_POLY2REORIENT_NONE;
    CoordType new_ratio = diff[1] / diff[0];
    if(!got_ratio) {
      ratio = new_ratio;
      got_ratio = true;
      continue;
    }
    if(!Equal(ratio, new_ratio)) // We span a 2D space
      return _WFMATH_POLY2REORIENT_NONE;
  }

  // Okay, we don't span both vectors. What now?

  if(still_valid[0]) {
    assert(m_axes[1].isValid());
    assert(!still_valid[1]);
    assert(!got_ratio);
    // This is easy, m_axes[1] is just invalid
    m_origin += m_axes[1] * first_point[1];
    m_axes[1].setValid(false);
    return _WFMATH_POLY2REORIENT_CLEAR_AXIS2;
  }

  if(still_valid[1]) {
    assert(m_axes[1].isValid());
    assert(!got_ratio);
    // This is a little harder, m_axes[0] is invalid, must swap axes
    m_origin += m_axes[0] * first_point[0];
    m_axes[0] = m_axes[1];
    m_axes[1].setValid(false);
    return _WFMATH_POLY2REORIENT_MOVE_AXIS2_TO_AXIS1;
  }

  // The !m_axes[1].isValid() case reducing to a point falls into here
  if(!got_ratio) { // Nothing's valid, all points are equal
    m_origin += m_axes[0] * first_point[0];
    if(m_axes[1].isValid())
      m_origin += m_axes[1] * first_point[1];
    m_axes[0].setValid(false);
    m_axes[1].setValid(false);
    return _WFMATH_POLY2REORIENT_CLEAR_BOTH_AXES;
  }

  assert(m_axes[1].isValid());

  // All the points are colinear, along some line which is not parallel
  // to either of the original axes

  Vector<dim> new0;
  new0 = m_axes[0] + m_axes[1] * ratio;
  CoordType norm = new0.mag();
  new0 /= norm;

//  Vector diff = m_axes[0] * first_point[0] + m_axes[1] * first_point[1];
//  // Causes Dot(diff, m_axes[0]) to vanish, so the point on the line
//  // with x coordinate zero is the new origin
//  diff -= new0 * (first_point[0] * norm);
//  m_origin += diff;
  // or, equivalently
    m_origin += m_axes[1] * (first_point[1] - ratio * first_point[0]);

  m_axes[0] = new0;
  m_axes[1].setValid(false);
  return _Poly2Reorient(_WFMATH_POLY2REORIENT_SCALE1_CLEAR2, norm);
}

template<int dim>
inline void _Poly2Orient<dim>::rotate(const RotMatrix<dim>& m, const Point<dim>& p)
{
  m_origin.rotate(m, p);

  for(int j = 0; j < 2; ++j)
    m_axes[j] = Prod(m_axes[j], m);
}

template<int dim>
void _Poly2Orient<dim>::rotate2(const RotMatrix<dim>& m, const Point<2>& p)
{
  assert(m_origin.isValid());

  if(!m_axes[0].isValid()) {
    assert(p[0] == 0 && p[1] == 0);
    return;
  }

  Vector<dim> shift = m_axes[0] * p[0];
  m_axes[0] = Prod(m_axes[0], m);

  if(m_axes[1].isValid()) {
    shift += m_axes[1] * p[1];
    m_axes[1] = Prod(m_axes[1], m);
  }
  else
    assert(p[1] == 0);

  m_origin += shift - Prod(shift, m);
}

template<>
inline void _Poly2Orient<3>::rotate(const Quaternion& q, const Point<3>& p)
{
  m_origin.rotate(q, p);

  for(int j = 0; j < 2; ++j)
    m_axes[j].rotate(q);
}

template<>
inline void _Poly2Orient<3>::rotate2(const Quaternion& q, const Point<2>& p)
{
  assert(m_origin.isValid());

  if(!m_axes[0].isValid()) {
    assert(p[0] == 0 && p[1] == 0);
    return;
  }

  Vector<3> shift = m_axes[0] * p[0];
  m_axes[0].rotate(q);

  if(m_axes[1].isValid()) {
    shift += m_axes[1] * p[1];
    m_axes[1].rotate(q);
  }
  else
    assert(p[1] == 0);

  m_origin += shift - shift.rotate(q);
}

template<int dim>
inline bool Polygon<dim>::addCorner(size_t i, const Point<dim>& p, CoordType epsilon)
{
  Point<2> p2;
  bool succ = m_orient.expand(p, p2, epsilon);
  if(succ)
    m_poly.addCorner(i, p2, epsilon);
  return succ;
}

template<int dim>
inline void Polygon<dim>::removeCorner(size_t i)
{
  m_poly.removeCorner(i);
  _Poly2Reorient r = m_orient.reduce(m_poly);
  r.reorient(m_poly);
}

template<int dim>
inline bool Polygon<dim>::moveCorner(size_t i, const Point<dim>& p, CoordType epsilon)
{
  _Poly2Orient<dim> try_orient = m_orient;
  _Poly2Reorient r = try_orient.reduce(m_poly, i);
  Point<2> p2;

  if(!try_orient.expand(p, p2, epsilon))
    return false;

  r.reorient(m_poly, i);
  m_poly[i] = p2;
  m_orient = try_orient;

  return true;
}

template<int dim>
AxisBox<dim> Polygon<dim>::boundingBox() const
{
  assert(m_poly.numCorners() > 0);

  Point<dim> min = m_orient.convert(m_poly[0]), max = min;
  bool valid = min.isValid();

  for(size_t i = 1; i != m_poly.numCorners(); ++i) {
    Point<dim> p = m_orient.convert(m_poly[i]);
    valid = valid && p.isValid();
    for(int j = 0; j < dim; ++j) {
      if(p[j] < min[j])
        min[j] = p[j];
      if(p[j] > max[j])
        max[j] = p[j];
    }
  }

  min.setValid(valid);
  max.setValid(valid);

  return AxisBox<dim>(min, max, true);
}

template<int dim>
inline Ball<dim> Polygon<dim>::boundingSphere() const
{
  Ball<2> b = m_poly.boundingSphere();

  return Ball<dim>(m_orient.convert(b.center()), b.radius());
}

template<int dim>
inline Ball<dim> Polygon<dim>::boundingSphereSloppy() const
{
  Ball<2> b = m_poly.boundingSphereSloppy();

  return Ball<dim>(m_orient.convert(b.center()), b.radius());
}

} // namespace WFMath

#endif  // WFMATH_POLYGON_FUNCS_H
