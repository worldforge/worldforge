// -*-C++-*-
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

#ifndef WFMATH_ROT_BOX_H
#define WFMATH_ROT_BOX_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/matrix.h>
#include <wfmath/const.h>
#include <wfmath/axisbox.h>
#include <wfmath/intersect_decls.h>

namespace WF { namespace Math {

template<const int dim>
class RotBox
{
 public:
  RotBox() {}
  RotBox(const Point<dim>& p, const Vector<dim>& size,
	 const RotMatrix<dim>& orientation) : m_corner0(p), m_size(size),
		m_orient(orientation) {}
  RotBox(const RotBox& b) : m_corner0(b.m_corner0), m_size(b.m_size),
		m_orient(b.m_orient) {}

  ~RotBox() {}

  std::string toString() const;
  bool fromString(const std::string& s);

  RotBox& operator=(const RotBox& s);

  bool isEqualTo(const RotBox& s, double tolerance = WFMATH_EPSILON) const;

  bool operator==(const RotBox& b) const	{return isEqualTo(b);}
  bool operator!=(const RotBox& b) const	{return !isEqualTo(b);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the box.
  bool operator< (const RotBox& b) const;

  // Descriptive characteristics

  int numCorners() const {return 1 << dim;}
  Point<dim> getCorner(int i) const;
  Point<dim> getCenter() const {return m_corner0 + Prod(m_orient, m_size / 2);}

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
	{m_orient = Prod(m, m_orient); m_corner0.rotate(m, p); return *this;}

  // Intersection functions

  AxisBox<dim> boundingBox() const;

  friend bool Intersect<dim>(const RotBox& r, const Point<dim>& p);
  friend bool IntersectProper<dim>(const RotBox& r, const Point<dim>& p);
  friend bool Contains<dim>(const Point<dim>& p, const RotBox& r);

 private:

  Point<dim> m_corner0;
  Vector<dim> m_size;
  RotMatrix<dim> m_orient;
};

}} // namespace WF::Math

#include <wfmath/rotbox_funcs.h>

#endif  // WFMATH_ROT_BOX_H
