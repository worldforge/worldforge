// -*-C++-*-
// stringconv.cpp (String utility backend in the WFMath library)
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

#include <sstream>
#include "stringconv.h"

using namespace WF::Math;

static void _ToString(std::ostream& os, const CoordType* d, const int num);
static bool _FromString(std::istream& is, CoordType* d, const int num);

// FIXME the string/double conversion should be replaced
// with something using pure C, not streams

std::string WF::Math::_StringFromFloat(CoordType d)
{
  ostringstream ost;

  ost << d;

  return ost.str();
}

bool WF::Math::_StringToCoordList(const std::string& s, CoordType* d, const int num)
{
  istringstream ist(s);

  return _FromString(ist, d, num);
}

std::string WF::Math::_StringFromCoordList(const CoordType* d, const int num)
{
  ostringstream ost;

  _ToString(ost, d, num);

  return ost.str();
}

bool WF::Math::_StringToCoordArray(const std::string& s, CoordType* d, const int rows,
				   const int columns)
{
  istringstream ist(s);
  char next;

  ist >> next;
  if(next != '(')
    return false;

  for(int i = 0; i < rows; ++i) {
    if(!_FromString(ist, d + i * columns, columns))
      return false;
    ist >> next;
    char want = (i == rows - 1) ? ')' : ',';
    if(next != want)
      return false;
  }

  return true;
}

std::string WF::Math::_StringFromCoordArray(const CoordType* d, const int rows,
					    const int columns)
{
  ostringstream ost;

  ost << '(';

  int i = 0;

  while(true) {
    _ToString(ost, d + i * columns, columns);
    if(++i == rows)
      break;
    ost << ',';
  }

  ost << ')';

  return ost.str();
}

static void _ToString(std::ostream& os, const CoordType* d, const int num)
{
  os << '(';

  int i = 0;

  while(true) {
    os << d[i];
    if(++i == num)
      break;
    os << ',';
  }

  os << ')';
}

static bool _FromString(std::istream& is, CoordType* d, const int num)
{
  char next;

  is >> next;

  if(next != '(')
    return false;

  for(int i = 0; i < num; ++i) {
    is >> d[i];
    is >> next;
    char want = (i == num - 1) ? ')' : ',';
    if(next != want)
      return false;
  }

  return true;
}
