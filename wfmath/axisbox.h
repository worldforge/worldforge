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

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/shape.h>

namespace WF { namespace Math {

template<const int dim> class AxisBox;

template<const int dim>
bool Intersect(const AxisBox<dim>& a1, const AxisBox<dim>& a2, AxisBox<dim>& out);
template<const int dim>
AxisBox<dim> Union(const AxisBox<dim>& a1, const AxisBox<dim>& a2);

template<const int dim>
class AxisBox : public Shape<dim>
{
 public:
  AxisBox() {}
  AxisBox(const Point<dim>& p1, const Point<dim>& p2) {setCorners(p1, p2);}
  AxisBox(const AxisBox<dim>& a) : Shape<dim>(a), m_low(a.m_low), m_high(a.m_high) {}

  virtual ~AxisBox() {}

  virtual std::string toString() const;
  bool fromString(const std::string& s);

  AxisBox<dim>& operator=(const AxisBox<dim>& a);

#ifndef WFMATH_NO_DYNAMIC_CAST
  virtual bool isEqualTo(const Shape<dim>& s, double tolerance = WFMATH_EPSILON) const
    {
      AxisBox<dim> *a = dynamic_cast<AxisBox*>(&s);
      return (a && isEqualTo(*a, tolerance));
    }
#endif // WFMATH_NO_DYNAMIC_CAST
  bool isEqualTo(const AxisBox<dim>& a, double tolerance = WFMATH_EPSILON) const
	{return m_low.isEqualTo(a.m_low, tolerance)
	     && m_high.isEqualTo(a.m_high, tolerance);}

  bool operator==(const AxisBox<dim>& a) const	{return isEqualTo(a);}
  bool operator!=(const AxisBox<dim>& a) const	{return !isEqualTo(a);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the box.
  bool operator< (const AxisBox<dim>& a) const;

  // Descriptive characteristics

  virtual int numCorners() const {return 1 << dim;}
  virtual Point<dim> getCorner(int i) const;
  AxisBox<dim> quadrant(int i) const;

  // For the same value of 'axis', the value returned by lowerBound()
  // is guaranteed to less than or equal to upperBound(), unless
  // the AxisBox<> is in an undefined state (created by zero-argument
  // constructor, or set by a call to fromString(), Intersect(), or
  // fromAtlas() which returned false).

  CoordType lowerBound(const int axis) const	{return m_low[axis];}
  CoordType upperBound(const int axis) const	{return m_high[axis];}

  AxisBox<dim>& setCorners(const Point<dim>& p1, const Point<dim>& p2);

  // Movement functions

  virtual AxisBox<dim>& shift(const Vector<dim>& v);

  // Intersection functions

  virtual AxisBox<dim> boundingBox() const {return *this;}
  virtual AxisBox<dim> enclosedBox() const {return *this;}

  friend bool Intersect<dim>(const AxisBox<dim>& a1, const AxisBox<dim>& a2,
			     AxisBox<dim>& out);
  friend AxisBox<dim> Union<dim>(const AxisBox<dim>& a1, const AxisBox<dim>& a2);

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

  Point<dim> m_low, m_high;
};

}} // namespace WF::Math

#include <wfmath/axisbox_funcs.h>

#endif  // WFMATH_AXIS_BOX_H
