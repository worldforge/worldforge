// -*-C++-*-
// segment.h (A line segment)
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

#ifndef WFMATH_SEGMENT_H
#define WFMATH_SEGMENT_H

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/const.h>
#include <wfmath/shape.h>
#include <wfmath/axisbox.h>

namespace WF { namespace Math {

template<const int dim>
class Segment : public RotShape<dim>
{
 public:
  Segment() {m_p1.origin(); m_p2.origin();}
  Segment(const Point<dim>& p1, const Point<dim>& p2);
  Segment(const Segment<dim>& s) : RotShape<dim>(s), m_p1(s.m_p1), m_p2(s.m_p2) {}

  virtual ~Segment() {}

  virtual std::string toString() const;
  bool fromString(const std::string& s);

  Segment<dim>& operator=(const Segment<dim>& s);

#ifndef WFMATH_NO_DYNAMIC_CAST
  virtual bool isEqualTo(const Shape<dim>& s, double tolerance = WFMATH_EPSILON) const
    {
      const Segment<dim> *seg = dynamic_cast<const Segment*>(&s);
      return (seg && isEqualTo(*seg, tolerance));
    }
#endif // WFMATH_NO_DYNAMIC_CAST
  bool isEqualTo(const Segment<dim>& s, double tolerance = WFMATH_EPSILON) const
	{return m_p1.isEqualTo(s.m_p1, tolerance)
	     && m_p2.isEqualTo(s.m_p2, tolerance);}

  bool operator==(const Segment<dim>& b) const	{return isEqualTo(b);}
  bool operator!=(const Segment<dim>& b) const	{return !isEqualTo(b);}

  // WARNING! This operator is for sorting only. It does not
  // reflect any property of the segment.
  bool operator< (const Segment<dim>& b) const;

  // Descriptive characteristics

  virtual int numCorners() const {return 2;}
  virtual Point<dim> getCorner(int i) const;
  Segment<dim>& setCorner(const Point<dim>& p, int i);

  // Movement functions

  virtual Shape<dim>& shift(const Vector<dim>& v)
	{Shape<dim>::m_center += v; m_p1 += v; m_p2 += v; return *this;}
  virtual RotShape<dim>& rotatePoint(const RotMatrix<dim>& m, const Point<dim>& p)
	{Shape<dim>::m_center.rotate(m, p); m_p1.rotate(m, p);
	 m_p2.rotate(m, p); return *this;}

  // Intersection functions

  virtual AxisBox<dim> boundingBox() const
	{return AxisBox<dim>(m_p1, m_p2);}
  virtual AxisBox<dim> enclosedBox() const
	{return AxisBox<dim>(Shape<dim>::m_center, Shape<dim>::m_center);}

  virtual bool contains(const Point<dim>& p) const;
  virtual bool containsProper(const Point<dim>& p) const;
  virtual bool contains(const AxisBox<dim>& b) const;
  virtual bool containsProper(const AxisBox<dim>& b) const;
  virtual bool isContainedBy(const AxisBox<dim>& b) const
	{return b.contains(m_p1) && b.contains(m_p2);}
  virtual bool isContainedByProper(const AxisBox<dim>& b) const
	{return b.containsProper(m_p1) && b.containsProper(m_p2);}
  virtual bool intersects(const AxisBox<dim>& b) const;
  virtual bool intersectsProper(const AxisBox<dim>& b) const;
  virtual int isSubContainedBy(const AxisBox<dim>& b) const;
  virtual int isSubContainedByProper(const AxisBox<dim>& b) const;

 private:

  Point<dim> m_p1, m_p2;
};

}} // namespace WF::Math

#include <wfmath/segment_funcs.h>

#endif  // WFMATH_SEGMENT_H
