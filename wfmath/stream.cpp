// stream.cpp (Stream conversion backend in the WFMath library)
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
// Created: 2001-12-13

#include "stream.h"
#include "quaternion.h"

using namespace WFMath;

void WFMath::_WriteCoordList(std::ostream& os, const CoordType* d, const int num)
{
  os << '(';

  for(int i = 0; i < num; ++i)
    os << d[i] << (i < (num - 1) ? ',' : ')');
}

void WFMath::_ReadCoordList(std::istream& is, CoordType* d, const int num)
{
  char next;

  is >> next;

  if(next != '(') {
    is.setstate(std::istream::failbit);
    return;
  }

  for(int i = 0; i < num; ++i) {
    is >> d[i] >> next;
    char want = (i == num - 1) ? ')' : ',';
    if(next != want) {
      is.setstate(std::istream::failbit);
      return;
    }
  }
}

CoordType WFMath::_GetEpsilon(std::istream& is)
{
  int str_prec = is.precision();
  double str_eps = 1;
  while(--str_prec > 0) // Precision of 6 gives epsilon = 1e-5
    str_eps /= 10;

  return str_eps;
}


// This is the only way I could get the operator<<() and operator>>()
// templates to recognize the declarations in the headers
namespace WFMath {

template<>
std::ostream& operator<<(std::ostream& os, const Polygon<2>& r)
{
  int size = r.m_points.size();

  if(size == 0) {
    os << "<empty>";
    return os;
  }

  os << "Polygon: (";

  for(int i = 0; i < size; ++i) {
    os << r.m_points[i] << (i < (size - 1) ? ',' : ')');
  }

  return os;
}

template<>
std::istream& operator>>(std::istream& is, Polygon<2>& r)
{
  char next;
  Point<2> p;

  r.m_points.clear();

  do {
    if(!is)
      return is;
    is >> next;
    if(next == '<') { // empty polygon
       do {
         if(!is)
           return is;
         is >> next;
       } while(next != '>');
       return is;
    }
  } while(next != '(');

  while(true) {
    is >> p;
    if(!is)
      return is;
    r.m_points.push_back(p);
    is >> next;
    if(next == ')')
      return is;
    if(next != ',') {
      is.setstate(std::istream::failbit);
      return is;
    }
  }
}

std::ostream& operator<<(std::ostream& os, const Quaternion& q)
{
  return os << "Quaternion: (" << q.m_w << ',' << q.m_vec << ')';
}

std::istream& operator>>(std::istream& is, Quaternion& q)
{
  char next;

  do {
    if(!is)
      return is;
    is >> next;
  } while(next != '(');

  is >> q.m_w;
  if(!is)
    return is;

  is >> next;
  if(next != ',') {
    is.setstate(std::istream::failbit);
    return is;
  }

  is >> q.m_vec;
  if(!is)
    return is;

  CoordType norm = q.m_w * q.m_w + q.m_vec.sqrMag();

  if(!Equal(norm, 1, _GetEpsilon(is))) {
    is.setstate(std::istream::failbit);
    return is;
  }

  norm = sqrt(norm);
  q.m_w /= norm;
  q.m_vec /= norm;

  is >> next;
  if(next != ')')
    is.setstate(std::istream::failbit);

  return is;
}

} // namespace WFMath
