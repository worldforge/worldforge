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

#include <stdexcept>
#include <Atlas/Message/Object.h>
#include <wfmath/const.h>
#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/quaternion.h>
#include <wfmath/axisbox.h>

namespace WFMath {

// Change this when we go to Atlas-0.5
struct _AtlasBadParse : public Atlas::Message::WrongTypeException,
			virtual public std::exception
{
  virtual ~_AtlasBadParse() throw() {}
};

inline Atlas::Message::Object _ArrayToAtlas(const CoordType* array, int len)
{
  Atlas::Message::Object::ListType a;

  for(const CoordType* i = array; i < array + len; ++i)
    a.push_back(*i);

  return a;
}

inline void _ArrayFromAtlas(CoordType* array, int len, const Atlas::Message::Object& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();

  Atlas::Message::Object::ListType list(a.AsList());

  if(list.size() != (unsigned int) len)
    throw _AtlasBadParse();

  for(int i = 0; i < len; ++i)
    array[i] = list[i].AsFloat();
}

template<const int dim>
inline void Vector<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  _ArrayFromAtlas(m_elem, dim, a);
  m_valid = true;
}

template<const int dim>
inline Atlas::Message::Object Vector<dim>::toAtlas() const
{
  return _ArrayToAtlas(m_elem, dim);
}

inline void Quaternion::fromAtlas(const Atlas::Message::Object& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();


  Atlas::Message::Object::ListType list(a.AsList());

  if(list.size() != 4)
    throw _AtlasBadParse();


  for(int i = 0; i < 3; ++i)
    m_vec[i] = list[i].AsFloat();

  m_w = list[3].AsFloat();

  CoordType norm = sqrt(m_w * m_w + m_vec.sqrMag());

  m_vec /= norm;
  m_w /= norm;

  m_valid = true;
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
inline void Point<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  _ArrayFromAtlas(m_elem, dim, a);
  m_valid = true;
}

template<const int dim>
inline Atlas::Message::Object Point<dim>::toAtlas() const
{
  return _ArrayToAtlas(m_elem, dim);
}

template<const int dim>
void AxisBox<dim>::fromAtlas(const Atlas::Message::Object& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();

  Atlas::Message::Object::ListType list(a.AsList());

  switch(list.size()) {
    case 1:
      m_low.setToOrigin();
      m_high.fromAtlas(list[0]);
      break;
    case 2:
      m_low.fromAtlas(list[0]);
      m_high.fromAtlas(list[1]);
      break;
    case dim:
      m_low.setToOrigin();
      m_high.fromAtlas(a);
      break;
    case (2 * dim):
      for(int i = 0; i < dim; ++i) {
        m_low[i] = list[i].AsFloat();
        m_high[i] = list[i+dim].AsFloat();
      }
      m_low.setValid();
      m_high.setValid();
      break;
    default:
      throw _AtlasBadParse();
  }

  for(int i = 0; i < dim; ++i) {
    if(m_low[i] > m_high[i]) { // spec may allow this?
      CoordType tmp = m_low[i];
      m_low[i] = m_high[i];
      m_high[i] = tmp;
    }
  }
}

template<>
inline void AxisBox<2>::fromAtlas(const Atlas::Message::Object& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();

  Atlas::Message::Object::ListType list(a.AsList());

  switch(list.size()) {
    case 1:
      m_low.setToOrigin();
      m_high.fromAtlas(list[0]);
      break;
    case 2: // 2 possible different cases
      if(list[0].IsFloat()) {
        m_low.setToOrigin();
        m_high.fromAtlas(a);
      }
      else {
        m_low.fromAtlas(list[0]);
        m_high.fromAtlas(list[1]);
      }
      break;
    case 4:
      for(int i = 0; i < 2; ++i) {
        m_low[i] = list[i].AsFloat();
        m_high[i] = list[i+2].AsFloat();
      }
      m_low.setValid();
      m_high.setValid();
      break;
    default:
      throw _AtlasBadParse();
  }

  for(int i = 0; i < 2; ++i) {
    if(m_low[i] > m_high[i]) { // spec may allow this?
      CoordType tmp = m_low[i];
      m_low[i] = m_high[i];
      m_high[i] = tmp;
    }
  }
}

template<>
inline void AxisBox<1>::fromAtlas(const Atlas::Message::Object& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();

  Atlas::Message::Object::ListType list(a.AsList());

  bool got_float = list[0].IsFloat();

  switch(list.size()) {
    case 1:
      m_low.setToOrigin();
      m_high.fromAtlas(got_float ? a : list[0]);
      break;
    case 2:
      if(got_float) {
        m_low[0] = list[0].AsFloat();
        m_high[0] = list[1].AsFloat(); 
        m_low.setValid();
        m_high.setValid();
      }
      else {
        m_low.fromAtlas(list[0]);
        m_high.fromAtlas(list[1]);
      }
      break;
    default:
      throw _AtlasBadParse();
  }

  if(m_low[0] > m_high[0]) { // spec may allow this?
    CoordType tmp = m_low[0];
    m_low[0] = m_high[0];
    m_high[0] = tmp;
  }
}

template<const int dim>
Atlas::Message::Object AxisBox<dim>::toAtlas() const
{
  int i;

  for(i = 0; i < dim; ++i)
    if(m_low[i] != 0)
      break;

  if(i == dim)
    return m_high.toAtlas(); // matches case 'dim' above

  // Do case '2 * dim' above

  Atlas::Message::Object::ListType a;
  for(i = 0; i < dim; ++i)
    a.push_back(m_low[i]);
  for(i = 0; i < dim; ++i)
    a.push_back(m_high[i]);

  return a;
}

} // namespace WFMath

#endif // WFMATH_ATLAS_CONV_H
