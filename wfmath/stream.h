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

#include <wfmath/wfmath.h>
#include <string>
#include <iostream>
#include <sstream>

namespace WF { namespace Math {

template<class C>
inline std::string ToString(const C& c)
{
  ostringstream ost;
  ost << c;
  return ost.str();
}

template<class C>
inline bool FromString(C& c, const std::string& s)
{
  istringstream ist(s);
  return c.fromStream(ist);
}

bool _ReadCoordList(std::istream& is, CoordType* d, const int num);
void _WriteCoordList(std::ostream& os, const CoordType* d, const int num);

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Vector<dim>& v)
{
  _WriteCoordList(os, v.m_elem, dim);
  return os;
}

template<const int dim>
inline bool Vector<dim>::fromStream(std::istream& is)
{
  return _ReadCoordList(is, m_elem, dim);
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
bool RotMatrix<dim>::fromStream(std::istream& is)
{
  CoordType d[dim*dim];
  char next;

  is >> next;
  if(next != '(')
    return false;

  for(int i = 0; i < dim; ++i) {
    if(!_ReadCoordList(is, d + i * dim, dim))
      return false;
    is >> next;
    char want = (i == dim - 1) ? ')' : ',';
    if(next != want)
      return false;
  }

  return _setVals(d, WFMATH_STRING_EPSILON);
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Point<dim>& p)
{
  _WriteCoordList(os, p.m_elem, dim);
  return os;
}

template<const int dim>
inline bool Point<dim>::fromStream(std::istream& is)
{
  return _ReadCoordList(is, m_elem, dim);
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const AxisBox<dim>& a)
{
  return os << "AxisBox: m_low = " << a.m_low << ", m_high = " << a.m_high;
}

template<const int dim>
bool AxisBox<dim>::fromStream(std::istream& is)
{
  char next;

  do
    is >> next;
  while(next != '=');

  if(!m_low.fromStream(is))
    return false;

  do
    is >> next;
  while(next != '=');

  return m_high.fromStream(is);
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Ball<dim>& b)
{
  return os << "Ball: m_center = " << b.m_center <<
	  + ", m_radius = " << b.m_radius;
}

template<const int dim>
inline bool Ball<dim>::fromStream(std::istream& is)
{
  char next;

  do
    is >> next;
  while(next != '=');

  if(!m_center.fromStream(is))
    return false;

  do
    is >> next;
  while(next != '=');

  is >> m_radius;

  return true;
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Segment<dim>& s)
{
  return os << "Segment: m_p1 = " << s.m_p1 << ", m_p2 = " << s.m_p2;
}

template<const int dim>
bool Segment<dim>::fromStream(std::istream& is)
{
  char next;

  do
    is >> next;
  while(next != '=');

  if(!m_p1.fromStream(is))
    return false;

  do
    is >> next;
  while(next != '=');

  return m_p2.fromStream(is);
}

template<const int dim>
std::ostream& operator<<(std::ostream& os, const RotBox<dim>& r)
{
  return os << "RotBox: m_corner0 = " << r.m_corner0
	 << ", m_size = " << r.m_size
	 << ", m_orient = " << r.m_orient;
}

template<const int dim>
bool RotBox<dim>::fromStream(std::istream& is)
{
  char next;

  do
    is >> next;
  while(next != '=');

  if(!m_corner0.fromStream(is))
    return false;

  do
    is >> next;
  while(next != '=');

  if(!m_size.fromStream(is))
    return false;

  do
    is >> next;
  while(next != '=');

  return m_orient.fromStream(is);
}

}} // namespace WF::Math

#endif // WFMATH_STREAM_H
