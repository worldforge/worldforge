// stream.h (Functions in the WFMath library that use streams)
//
//  The WorldForge Project
//  Copyright (C) 2001,2002  The WorldForge Project
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

// Author: Ron Steinke
// Created: 2001-12-7

#ifndef WFMATH_STREAM_H
#define WFMATH_STREAM_H

#include <wfmath/vector.h>
#include <wfmath/rotmatrix.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/segment.h>
#include <wfmath/rotbox.h>
#include <wfmath/polygon.h>
#include <wfmath/error.h>
#include <string>
#include <iostream>
#include <list> // For Polygon<>::operator>>()

#include <cassert>

namespace WFMath {

// sstream vs. strstream compatibility wrapper

namespace _IOWrapper {

  // Need separate read/write classes, since one is const C& and the other is C&

  class BaseRead {
   public:
    virtual ~BaseRead() {}

    virtual void read(std::istream& is) = 0;
  };

  class BaseWrite {
   public:
    virtual ~BaseWrite() {}

    virtual void write(std::ostream& os) const = 0;
  };

  template<class C>
  class ImplRead : public BaseRead {
   public:
    ImplRead(C& c) : m_data(c) {}
    virtual ~ImplRead() {}

    virtual void read(std::istream& is) {is >> m_data;}

   private:
    C &m_data;
  };

  template<class C>
  class ImplWrite : public BaseWrite {
   public:
    ImplWrite(const C& c) : m_data(c) {}
    virtual ~ImplWrite() {}

    virtual void write(std::ostream& os) const {os << m_data;}

   private:
    const C &m_data;
  };

  std::string ToStringImpl(const BaseWrite& b, std::streamsize precision);
  void FromStringImpl(BaseRead& b, const std::string& s, std::streamsize precision);
}

/// Output a WFMath type as a string
/**
 * This uses operator<<() in its backend.
 **/
template<class C>
inline std::string ToString(const C& c, std::streamsize precision = 6)
{
  return _IOWrapper::ToStringImpl(_IOWrapper::ImplWrite<C>(c), precision);
}

/// Parse a WFMath type from a string
/**
 * This uses operator>>() in its backend.
 **/
template<class C>
inline void FromString(C& c, const std::string& s, std::streamsize = 6)
{
  _IOWrapper::ImplRead<C> i(c);
  _IOWrapper::FromStringImpl(i, s, 6);
}

void _ReadCoordList(std::istream& is, CoordType* d, const int num);
void _WriteCoordList(std::ostream& os, const CoordType* d, const int num);
CoordType _GetEpsilon(std::istream& is);

template<int dim>
inline std::ostream& operator<<(std::ostream& os, const Vector<dim>& v)
{
  _WriteCoordList(os, v.m_elem, dim);
  return os;
}

template<int dim>
inline std::istream& operator>>(std::istream& is, Vector<dim>& v)
{
  _ReadCoordList(is, v.m_elem, dim);
  v.m_valid = true;
  return is;
}

template<int dim>
inline std::ostream& operator<<(std::ostream& os, const RotMatrix<dim>& m)
{
  os << '(';

  for(int i = 0; i < dim; ++i) {
    _WriteCoordList(os, m.m_elem[i], dim);
    os << (i < (dim - 1) ? ',' : ')');
  }

  return os;
}

template<int dim>
inline std::istream& operator>>(std::istream& is, RotMatrix<dim>& m)
{
  CoordType d[dim*dim];
  char next;

  is >> next;
  if(next != '(')
    throw ParseError();

  for(int i = 0; i < dim; ++i) {
    _ReadCoordList(is, d + i * dim, dim);
    is >> next;
    char want = (i == dim - 1) ? ')' : ',';
    if(next != want)
      throw ParseError();
  }

  if(!m._setVals(d, FloatMax(WFMATH_EPSILON, _GetEpsilon(is))))
    throw ParseError();

  return is;
}

template<int dim>
inline std::ostream& operator<<(std::ostream& os, const Point<dim>& p)
{
  _WriteCoordList(os, p.m_elem, dim);
  return os;
}

template<int dim>
inline std::istream& operator>>(std::istream& is, Point<dim>& p)
{
  _ReadCoordList(is, p.m_elem, dim);
  p.m_valid = true;
  return is;
}

template<int dim>
inline std::ostream& operator<<(std::ostream& os, const AxisBox<dim>& a)
{
  return os << "AxisBox: m_low = " << a.m_low << ", m_high = " << a.m_high;
}

template<int dim>
inline std::istream& operator>>(std::istream& is, AxisBox<dim>& a)
{
  char next;

  do {
    is >> next;
  } while(next != '=');

  is >> a.m_low;

  do {
    is >> next;
  } while(next != '=');

  is >> a.m_high;

  return is;
}

template<int dim>
inline std::ostream& operator<<(std::ostream& os, const Ball<dim>& b)
{
  return os << "Ball: m_center = " << b.m_center <<
	  + ", m_radius = " << b.m_radius;
}

template<int dim>
inline std::istream& operator>>(std::istream& is, Ball<dim>& b)
{
  char next;

  do {
    is >> next;
  } while(next != '=');

  is >> b.m_center;

  do {
    is >> next;
  } while(next != '=');

  is >> b.m_radius;

  return is;
}

template<int dim>
inline std::ostream& operator<<(std::ostream& os, const Segment<dim>& s)
{
  return os << "Segment: m_p1 = " << s.m_p1 << ", m_p2 = " << s.m_p2;
}

template<int dim>
inline std::istream& operator>>(std::istream& is, Segment<dim>& s)
{
  char next;

  do {
    is >> next;
  } while(next != '=');

  is >> s.m_p1;

  do {
    is >> next;
  } while(next != '=');

  is >> s.m_p2;

  return is;
}

template<int dim>
inline std::ostream& operator<<(std::ostream& os, const RotBox<dim>& r)
{
  return os << "RotBox: m_corner0 = " << r.m_corner0
	 << ", m_size = " << r.m_size
	 << ", m_orient = " << r.m_orient;
}

template<int dim>
inline std::istream& operator>>(std::istream& is, RotBox<dim>& r)
{
  char next;

  do {
    is >> next;
  } while(next != '=');

  is >> r.m_corner0;

  do {
    is >> next;
  } while(next != '=');

  is >> r.m_size;

  do {
    is >> next;
  } while(next != '=');

  is >> r.m_orient;

  return is;
}

template<> std::ostream& operator<<(std::ostream& os, const Polygon<2>& r);
template<> std::istream& operator>>(std::istream& is, Polygon<2>& r);

template<int dim>
inline std::ostream& operator<<(std::ostream& os, const Polygon<dim>& r)
{
  int size = r.m_poly.numCorners();

  if(size == 0) {
    os << "<empty>";
    return os;
  }

  os << "Polygon: (";

  for(int i = 0; i < size; ++i)
    os << r.getCorner(i) << (i < (dim - 1) ? ',' : ')');

  return os;
}

// Can't stick this in operator>>(std::istream&, Polygon<>&), because
// we use it as a template argument for list<>. Why isn't that allowed?
template<int dim> struct _PolyReader
{
  Point<dim> pd;
  Point<2> p2;
};

template<int dim>
std::istream& operator>>(std::istream& is, Polygon<dim>& r)
{
  char next;
  _PolyReader<dim> read;
  std::list<_PolyReader<dim> > read_list;

  // Read in the points

  do {
    is >> next;
    if(next == '<') { // empty polygon
       do {
         is >> next;
       } while(next != '>');
       return is;
    }
  } while(next != '(');

  while(true) {
    is >> read.pd;
    read_list.push_back(read);
    is >> next;
    if(next == ')')
      break;
    if(next != ',')
      throw ParseError();
  }

  // Convert to internal format. Be careful about the order points are
  // added to the orientation. If the first few points are too close together,
  // round off error can skew the plane, and later points that are further
  // away may fail.

  typename std::list<_PolyReader<dim> >::iterator i, end = read_list.end();
  bool succ;

  std::streamsize str_prec = is.precision();
  double str_eps = 1;
  while(--str_prec > 0) // Precision of 6 gives epsilon = 1e-5
    str_eps /= 10;
  double epsilon = DoubleMax(str_eps, WFMATH_EPSILON);

  r.m_orient = _Poly2Orient<dim>();

  if(read_list.size() < 3) { // This will always work
    for(i = read_list.begin(); i != end; ++i) {
      succ = r.m_orient.expand(i->pd, i->p2, epsilon);
      assert(succ);
    }
  }
  else { // Find the three furthest apart points
    typename std::list<_PolyReader<dim> >::iterator p1 = end, p2 = end, p3 = end, j; // invalid values
    CoordType dist = -1;

    for(i = read_list.begin(); i != end; ++i) {
      for(j = i, ++j; j != end; ++j) {
        CoordType new_dist = SloppyDistance(i->pd, j->pd);
        if(new_dist > dist) {
          p1 = i;
          p2 = j;
          dist = new_dist;
        }
      }
    }

    assert(p1 != end);
    assert(p2 != end);

    dist = -1;

    for(i = read_list.begin(); i != end; ++i) {
      // Don't want to be near either p1 or p2
      if(i == p1 || i == p2)
        continue;
      CoordType new_dist = FloatMin(SloppyDistance(i->pd, p1->pd),
				    SloppyDistance(i->pd, p2->pd));
      if(new_dist > dist) {
        p3 = i;
        dist = new_dist;
      }
    }

    assert(p3 != end);

    // Add p1, p2, p3 first

    succ = r.m_orient.expand(p1->pd, p1->p2, epsilon);
    assert(succ);
    succ = r.m_orient.expand(p2->pd, p2->p2, epsilon);
    assert(succ);
    succ = r.m_orient.expand(p3->pd, p3->p2, epsilon);
    assert(succ);

    // Try to add the rest

    for(i = read_list.begin(); i != end; ++i) {
      if(i == p1 || i == p2 || i == p3) // Did these already
        continue;
      succ = r.m_orient.expand(i->pd, i->p2, epsilon);
      if(!succ) {
        r.clear();
        throw ParseError();
      }
    }
  }

  // Got valid points, add them to m_poly

  r.m_poly.resize(read_list.size());

  int pnum;
  for(i = read_list.begin(), pnum = 0; i != end; ++i, ++pnum)
    r.m_poly[pnum] = i->p2;

  return is;
}

} // namespace WFMath

#endif // WFMATH_STREAM_H
