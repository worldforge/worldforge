// -*-C++-*-
// stream.h (Functions in the WFMath library that use streams)
//
//  The WorldForge Project
//  Copyright (C) 2001  The WorldForge Project
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
#include <wfmath/matrix.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/segment.h>
#include <wfmath/rotbox.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace WF { namespace Math {

template<class C>
std::string ToString(const C& c, unsigned int precision = 6)
{
  ostringstream ost;
  ost.precision(precision);
  ost << c;
  return ost.str();
}

template<class C>
bool FromString(C& c, const std::string& s, unsigned int precision = 6)
{
  istringstream ist(s);
  ist.precision(precision);
  ist >> c;
  return ist; // Check ist.fail()
}

void _ReadCoordList(std::istream& is, CoordType* d, const int num);
void _WriteCoordList(std::ostream& os, const CoordType* d, const int num);

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Vector<dim>& v)
{
  _WriteCoordList(os, v.m_elem, dim);
  return os;
}

template<const int dim>
std::istream& operator>>(std::istream& is, Vector<dim>& v)
{
  _ReadCoordList(is, m_elem, dim);
  return is;
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const RotMatrix<dim>& m)
{
  os << '(';

  for(int i = 0; i < dim; ++i) {
    _WriteCoordList(os, m.m_elem[i], dim);
    os << (i < (dim - 1) ? ',' : ')');
  }

  return os;
}

template<const int dim>
std::istream& operator>>(std::istream& is, RotMatrix<dim>& m)
{
  CoordType d[dim*dim];
  char next;

  is >> next;
  if(next != '(') {
    is.setstate(std::istream::failbit);
    return is;
  }

  for(int i = 0; i < dim; ++i) {
    _ReadCoordList(is, d + i * dim, dim);
    if(!is)
      return is;
    is >> next;
    char want = (i == dim - 1) ? ')' : ',';
    if(next != want) {
      is.setstate(std::istream::failbit);
      return is;
    }
  }

  int str_prec = is.precision();
  double str_eps = 1;
  while(str_prec-- > 0)
    str_eps /= 10;

  if(!m._setVals(d, FloatMax(WFMATH_EPSILON, str_eps)))
    is.setstate(std::istream::failbit);

  return is;
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Point<dim>& p)
{
  _WriteCoordList(os, p.m_elem, dim);
  return os;
}

template<const int dim>
std::istream& operator>>(std::istream& is, Point<dim>& p)
{
  _ReadCoordList(is, m_elem, dim);
  return is;
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const AxisBox<dim>& a)
{
  return os << "AxisBox: m_low = " << a.m_low << ", m_high = " << a.m_high;
}

template<const int dim>
std::istream& operator>>(std::istream& is, AxisBox<dim>& a)
{
  char next;

  do {
    if(!is)
      return is;
    is >> next;
  } while(next != '=');

  is >> m_low;
  if(!is)
    return is;

  do {
    if(!is)
      return is;
    is >> next;
  } while(next != '=');

  is >> m_high;

  return is;
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Ball<dim>& b)
{
  return os << "Ball: m_center = " << b.m_center <<
	  + ", m_radius = " << b.m_radius;
}

template<const int dim>
std::istream& operator>>(std::istream& is, Ball<dim>& b)
{
  char next;

  do {
    if(!is)
      return is;
    is >> next;
  } while(next != '=');

  is >> m_center;
  if(!is)
    return is;

  do {
    if(!is)
      return is;
    is >> next;
  } while(next != '=');

  is >> m_radius;

  return is;
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Segment<dim>& s)
{
  return os << "Segment: m_p1 = " << s.m_p1 << ", m_p2 = " << s.m_p2;
}

template<const int dim>
std::istream& operator>>(std::istream& is, Segment<dim>& s)
{
  char next;

  do {
    if(!is)
      return is;
    is >> next;
  } while(next != '=');

  is >> m_p1;
  if(!is)
    return is;

  do {
    if(!is)
      return is;
    is >> next;
  } while(next != '=');

  is >> m_p2;

  return is;
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const RotBox<dim>& r)
{
  return os << "RotBox: m_corner0 = " << r.m_corner0
	 << ", m_size = " << r.m_size
	 << ", m_orient = " << r.m_orient;
}

template<const int dim>
std::istream& operator>>(std::istream& is, RotBox<dim>& r)
{
  char next;

  do {
    if(!is)
      return is;
    is >> next;
  } while(next != '=');

  is >> m_corner0;
  if(!is)
    return is;

  do {
    if(!is)
      return is;
    is >> next;
  } while(next != '=');

  is >> m_size;
  if(!is)
    return is;

  do {
    if(!is)
      return is;
    is >> next;
  } while(next != '=');

  is >> m_orient;

  return is;
}

}} // namespace WF::Math

#endif // WFMATH_STREAM_H
