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

inline bool Quaternion::fromAtlas(const Atlas::Message::Object& a)
{
  if(!a.IsList())
    return false;

  Atlas::Message::Object::ListType list(a.AsList());

  if(list.size() != 4)
    return false;

  for(int i = 0; i < 3; ++i) {
    if(!list[i].IsFloat())
      return false;
    m_vec[i] = list[i].AsFloat();
  }

  if(!list[3].IsFloat())
    return false;
  m_w = list[3].AsFloat();

  return true;
}

inline Atlas::Message::Object Quaternion::toAtlas() const
{
  Atlas::Message::Object::ListType a;

  for(int i = 0; i < 3; ++i)
    a.push_back(m_vec[i]);
  a.push_back(m_w);

  return a;
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
  if(!a.IsList())
    return false;

  Atlas::Message::Object::ListType list(a.AsList());

  switch(list.size()) {
    case 1:
      m_low.origin();
      if(!m_high.fromAtlas(list[0]))
        return false;
      break;
    case 2:
      if(!m_low.fromAtlas(list[0]) || !m_high.fromAtlas(list[1]))
        return false;
      break;
    case dim:
      m_low.origin();
      if(!m_high.fromAtlas(a))
        return false;
      break;
    case (2 * dim):
      for(int i = 0; i < dim; ++i) {
        if(!list[i].IsFloat() || !list[i+dim].IsFloat())
          return false;
        m_low[i] = list[i].AsFloat();
        m_high[i] = list[i+dim].AsFloat();
      }
      break;
    default:
      return false;
  }

  for(int i = 0; i < dim; ++i)
    if(m_low[i] > m_high[i])
      return false;

  return true;
}

template<>
bool AxisBox<2>::fromAtlas(const Atlas::Message::Object& a)
{
  if(!a.IsList())
    return false;

  Atlas::Message::Object::ListType list(a.AsList());

  switch(list.size()) {
    case 1:
      m_low.origin();
      if(!m_high.fromAtlas(list[0]))
        return false;
      break;
    case 2: // 2 possible different cases
      if(list[0].IsFloat()) {
        m_low.origin();
        if(!m_high.fromAtlas(a))
          return false;
      }
      else if(!m_low.fromAtlas(list[0]) || !m_high.fromAtlas(list[1]))
        return false;
      break;
    case 4:
      for(int i = 0; i < 2; ++i) {
        if(!list[i].IsFloat() || !list[i+2].IsFloat())
          return false;
        m_low[i] = list[i].AsFloat();
        m_high[i] = list[i+2].AsFloat();
      }
      break;
    default:
      return false;
  }

  for(int i = 0; i < 2; ++i)
    if(m_low[i] > m_high[i])
      return false;

  return true;
}

template<>
bool AxisBox<1>::fromAtlas(const Atlas::Message::Object& a)
{
  if(!a.IsList())
    return false;

  Atlas::Message::Object::ListType list(a.AsList());

  bool got_float = list[0].IsFloat();

  switch(list.size()) {
    case 1:
      m_low.origin();
      if(!m_high.fromAtlas(got_float ? a : list[0]))
        return false;
      break;
    case 2:
      if(got_float) {
          if(!list[1].IsFloat())
            return false;
          m_low[0] = list[0].AsFloat();
          m_high[0] = list[1].AsFloat(); 
      }
      else if(!m_low.fromAtlas(list[0]) || !m_high.fromAtlas(list[1]))
        return false;
      break;
    default:
      return false;
  }

  if(m_low[0] > m_high[0])
    return false;

  return true;
}

template<const int dim>
Atlas::Message::Object AxisBox<dim>::toAtlas() const
{
  if(m_low == Point<dim>().origin())
    return m_high.toAtlas(); // matches case 'dim' above

  // Do case '2' above

  Atlas::Message::Object::ListType a;
  a.push_back(m_low.toAtlas());
  a.push_back(m_high.toAtlas());

  return a;
}

}} // namespace WF:Math

#endif // WFMATH_ATLAS_CONV_H
