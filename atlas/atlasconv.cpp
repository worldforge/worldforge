// -*-C++-*-
// atlasconv.cpp (Backend to WFMath/Atlas Message conversion)
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
// Created: 2001-12-12

#include <Atlas/Message/Object.h>
#include "atlasconv.h"

using namespace WF::Math;

bool _GetAtlasDoubleList(const Atlas::Message::Object& a, double* d, int num);
{
  if(!a.IsList())
    return false;

  Atlas::Message::Object::ListType l = a.AsList();

  if(l.size() != num)
    return false;

  for(int i = 0; i < num; ++i) {
    if(!l[i].IsFloat())
      return false;
    d[i] = l[i].AsFloat();
  }

  return true;
}

Atlas::Message::Object _SetAtlasDoubleList(double* d, int num);
{
  Atlas::Message::Object::ListType a;

  for(int i = 0; i < num; ++i)
    a.push_back(Atlas::Message::Object(d[i]));

  return Atlas::Message::Object(a);
}
