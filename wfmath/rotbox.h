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
#include <wfmath/shape.h>
#include <wfmath/axisbox.h>

namespace WF { namespace Math {

template<const int dim>
class RotBox : public RotShape<dim>
{
 public:
  RotBox() {m_corner0.origin(); m_size.zero();}
  RotBox(const Point<dim>& p, const Vector<dim>& size,
	 const RotMatrix<dim>& orientation);
  RotBox(const RotBox<dim>& b) : Shape<dim>(b), m_corner0(b.m_corner0),
	m_size(b.m_size), m_orient(b.m_orient) {}

  virtual ~RotBox() {}

  virtual std::string toString() const;
  bool fromString(const std::string& s);

  RotBox<dim>& operator=(const RotBox<dim>& s);

#ifndef WFMATH_NO_DYNAMIC_CAST
  virtual bool isEqualTo(const Shape<dim>& s, double tolerance = WFMATH_EPSILON) const
    {
      const RotBox<dim> *b = dynamic_cast<const RotBox*>(&s);
      return (b && isEqualTo(*b, tolerance));
    }
#endif // WFMATH_NO_DYNAMIC_CAST
  bool isEqualTo(const RotBox<dim>& s, double tolerance = WFMATH_EPSILON) const;

  bool operator==(const RotBox<dim>& b) const	{return isEqualTo(b);}
  bool operator!=(const RotBox<dim>& b) const	{return !isEqualTo(b);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the box.
  bool operator< (const RotBox<dim>& b) const;

  // Descriptive characteristics

  virtual int numCorners() const {return 1 << dim;}
  virtual Point<dim> getCorner(int i) const;

  // Movement functions

  virtual Shape<dim>& shift(const Vector<dim>& v);
  virtual RotShape<dim>& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p);

  // Intersection functions

  virtual AxisBox<dim> boundingBox() const;
  virtual AxisBox<dim> enclosedBox() const;

  virtual bool contains(const Point<dim>& p) const;
  virtual bool containsProper(const Point<dim>& p) const;
  virtual bool contains(const AxisBox<dim>& b) const;
  virtual bool containsProper(const AxisBox<dim>& b) const;
  virtual bool isContainedBy(const AxisBox<dim>& b) const;
  virtual bool isContainedByProper(const AxisBox<dim>& b) const;
  virtual bool intersects(const AxisBox<dim>& b) const;
  virtual bool intersectsProper(const AxisBox<dim>& b) const;
  virtual int isSubContainedBy(const AxisBox<dim>& b) const;
  virtual int isSubContainedByProper(const AxisBox<dim>& b) const;

 private:

  Point<dim> m_corner0;
  Vector<dim> m_size;
  RotMatrix<dim> m_orient;
};

}} // namespace WF::Math

#include <wfmath/rotbox_funcs.h>

#endif  // WFMATH_ROT_BOX_H
