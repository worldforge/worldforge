// -*-C++-*-
// stream.cpp (Stream utility backend in the WFMath library)
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

using namespace WF::Math;

void WF::Math::_WriteCoordList(std::ostream& os, const CoordType* d, const int num)
{
  os << '(';

  for(int i = 0; i < num; ++i)
    os << d[i] << (i < (num - 1) ? ',' : ')');
}

void WF::Math::_ReadCoordList(std::istream& is, CoordType* d, const int num)
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
