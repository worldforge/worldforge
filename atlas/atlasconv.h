// -*-C++-*-
// atlasconv.h (Functions to convert WFMath library object to/from an Atlas Message)
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
// Created: 2001-12-11

// Since we don't want WFMath and Atlas to depend on each other,
// we're putting all the atlas interface functions into this header.

// WARNING! WARNING! Do not include this file in any other file in wfmath.

#ifndef WFMATH_ATLAS_CONV_H
#define WFMATH_ATLAS_CONV_H

#include <Atlas/Message/Object.h>
#include <wfmath/wfmath.h>

namespace WF { namespace Math {

template<const int dim>
bool FromAtlas(Vector<dim>& v, const Atlas::Message::Object& a)
{
  return a.isString() && v.fromString(a.asString());
}

template<const int dim>
Atlas::Message::Object ToAtlas(const Vector<dim>& v)
{
  return v.toString();
}

template<const int dim>
bool FromAtlas(RotMatrix<dim>& m, const Atlas::Message::Object& a)
{
  return a.isString() && m.fromString(a.asString());
}

template<const int dim>
Atlas::Message::Object ToAtlas(const RotMatrix<dim>& m)
{
  return m.toString();
}

template<const int dim>
bool FromAtlas(Point<dim>& p, const Atlas::Message::Object& a)
{
  return a.isString() && p.fromString(a.asString());
}

template<const int dim>
Atlas::Message::Object ToAtlas(const Point<dim>& p)
{
  return p.toString();
}

template<const int dim>
bool FromAtlas(AxisBox<dim>& b, const Atlas::Message::Object& a)
{
  return a.isString() && b.fromString(a.asString());
}

template<const int dim>
Atlas::Message::Object ToAtlas(const AxisBox<dim>& b)
{
  return b.toString();
}

template<const int dim>
bool FromAtlas(Ball<dim>& b, const Atlas::Message::Object& a)
{
  return a.isString() && b.fromString(a.asString());
}

template<const int dim>
Atlas::Message::Object ToAtlas(const Ball<dim>& b)
{
  return b.toString();
}

template<const int dim>
bool FromAtlas(Segment<dim>& s, const Atlas::Message::Object& a)
{
  return a.isString() && s.fromString(a.asString());
}

template<const int dim>
Atlas::Message::Object ToAtlas(const Segment<dim>& s)
{
  return s.toString();
}

template<const int dim>
bool FromAtlas(RotBoxim>& b, const Atlas::Message::Object& a)
{
  return a.isString() && b.fromString(a.asString());
}

template<const int dim>
Atlas::Message::Object ToAtlas(const RotBoxim>& b)
{
  return b.toString();
}

}} // namespace WF:Math

#endif // WFMATH_ATLAS_CONV_H
