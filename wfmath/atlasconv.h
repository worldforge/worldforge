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
#include <wfmath/stream.h>

namespace WF { namespace Math {

inline Atlas::Message::Object _ArrayToAtlas(const CoordType* array, int len)
{
  Atlas::Message::Object::ListType a;

  for(const CoordType* i = array; i < array + len; ++i)
    a.push_back(*i);

  return a;
}

inline bool _ArrayFromAtlas(CoordType* array, int len, const Atlas::Message::Object& a)
{
  if(!a.IsList())
    return false;

  Atlas::Message::Object::ListType list(a.AsList());

  if(list.size() != (unsigned int) len)
    return false;

  for(int i = 0; i < len; ++i) {
    if(!list[i].IsFloat())
      return false;
    array[i] = list[i].AsFloat();
  }

  return true;
}

template<const int dim>
bool Vector<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  return _ArrayFromAtlas(m_elem, dim, a);
}

template<const int dim>
Atlas::Message::Object Vector<dim>::toAtlas() const
{
  return _ArrayToAtlas(m_elem, dim);
}

template<const int dim>
bool RotMatrix<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  return a.IsString() && FromString(*this, a.AsString());
}

template<const int dim>
Atlas::Message::Object RotMatrix<dim>::toAtlas() const
{
  return ToString(*this);
}

inline bool Quaternion::fromAtlas(const Atlas::Message::Object& a)
{
  return a.IsString() && FromString(*this, a.AsString());
}

inline Atlas::Message::Object Quaternion::toAtlas() const
{
  return ToString(*this);
}

template<const int dim>
bool Point<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  return _ArrayFromAtlas(m_elem, dim, a);
}

template<const int dim>
Atlas::Message::Object Point<dim>::toAtlas() const
{
  return _ArrayToAtlas(m_elem, dim);
}

template<const int dim>
bool AxisBox<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  return a.IsString() && FromString(*this, a.AsString());
}

template<const int dim>
Atlas::Message::Object AxisBox<dim>::toAtlas() const
{
  return ToString(*this);
}

template<const int dim>
bool Ball<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  return a.IsString() && FromString(*this, a.AsString());
}

template<const int dim>
Atlas::Message::Object Ball<dim>::toAtlas() const
{
  return ToString(*this);
}

template<const int dim>
bool Segment<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  return a.IsString() && FromString(*this, a.AsString());
}

template<const int dim>
Atlas::Message::Object Segment<dim>::toAtlas() const
{
  return ToString(*this);
}

template<const int dim>
bool RotBox<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  return a.IsString() && FromString(*this, a.AsString());
}

template<const int dim>
Atlas::Message::Object RotBox<dim>::toAtlas() const
{
  return ToString(*this);
}

template<const int dim>
bool Polygon<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  return a.IsString() && FromString(*this, a.AsString());
}

template<const int dim>
Atlas::Message::Object Polygon<dim>::toAtlas() const
{
  return ToString(*this);
}

// Need this because Polygon<2> is a specialization

//template<>
bool Polygon<2>::fromAtlas(const Atlas::Message::Object& a)
{
  return a.IsString() && FromString(*this, a.AsString());
}

//template<>
Atlas::Message::Object Polygon<2>::toAtlas() const
{
  return ToString(*this);
}

}} // namespace WF:Math

#endif // WFMATH_ATLAS_CONV_H
