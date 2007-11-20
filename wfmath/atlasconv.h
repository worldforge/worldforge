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
#include <wfmath/const.h>
#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/quaternion.h>
#include <wfmath/axisbox.h>

namespace WFMath {

#ifdef ATLAS_MESSAGE_ELEMENT_H

typedef Atlas::Message::WrongTypeException _AtlasBadParse;
typedef Atlas::Message::Element _AtlasMessageType;
typedef Atlas::Message::FloatType _AtlasFloatType;
typedef Atlas::Message::ListType _AtlasListType;

inline bool _isNum(const _AtlasMessageType& a) {return a.isNum();}
inline _AtlasFloatType _asNum(const _AtlasMessageType& a) {return a.asNum();}

#elif defined(ATLAS_MESSAGE_OBJECT_H)

struct _AtlasBadParse : public Atlas::Message::WrongTypeException,
			virtual public std::exception
{
  virtual ~_AtlasBadParse() throw() {}
};

typedef Atlas::Message::Object _AtlasMessageType;
typedef Atlas::Message::Object::FloatType _AtlasFloatType;
typedef Atlas::Message::Object::ListType _AtlasListType;

inline bool _isNum(const _AtlasMessageType& a) {return a.IsNum();}
inline _AtlasMessageType::FloatType _asNum(const _AtlasMessageType& a) {return a.AsNum();}

#else
#error "You must include Atlas/Message/Element.h or Atlas/Message/Object.h before wfmath/atlasconv.h"
#endif

class AtlasInType
{
 public:
  AtlasInType(const _AtlasMessageType& val) : m_val(val) {}
  // allow nice conversions when necessary
  template<class C> AtlasInType(C c) : m_obj(c), m_val(m_obj) {}
  operator const _AtlasMessageType&() const {return m_val;}
#ifdef ATLAS_MESSAGE_ELEMENT_H
  bool IsList() const {return m_val.isList();}
  const _AtlasListType& AsList() const {return m_val.asList();}
#else // ATLAS_MESSAGE_OBJECT_H
  bool IsList() const {return m_val.IsList();}
  const _AtlasListType& AsList() const {return m_val.AsList();}
#endif
 private:
  _AtlasMessageType m_obj;
  const _AtlasMessageType& m_val;
};

class AtlasOutType
{
 public:
  AtlasOutType(const _AtlasListType& l) : m_val(l) {}
  operator _AtlasMessageType&() {return m_val;}
  operator const _AtlasMessageType&() const {return m_val;}
 private:
  _AtlasMessageType m_val;
};

inline AtlasOutType _ArrayToAtlas(const CoordType* array, unsigned len)
{
  _AtlasListType a(len);

  for(unsigned i = 0; i < len; ++i)
    a[i] = array[i];

  return a;
}

inline void _ArrayFromAtlas(CoordType* array, unsigned len, const AtlasInType& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();

  const _AtlasListType& list(a.AsList());

  if(list.size() != (unsigned int) len)
    throw _AtlasBadParse();

  for(unsigned i = 0; i < len; ++i)
    array[i] = _asNum(list[i]);
}

template<const int dim>
inline void Vector<dim>::fromAtlas(const AtlasInType& a)
{
  _ArrayFromAtlas(m_elem, dim, a);
  m_valid = true;
}

template<const int dim>
inline AtlasOutType Vector<dim>::toAtlas() const
{
  return _ArrayToAtlas(m_elem, dim);
}

inline void Quaternion::fromAtlas(const AtlasInType& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();


  const _AtlasListType& list(a.AsList());

  if(list.size() != 4)
    throw _AtlasBadParse();


  for(int i = 0; i < 3; ++i)
    m_vec[i] = _asNum(list[i]);

  m_w = _asNum(list[3]);

  CoordType norm = sqrt(m_w * m_w + m_vec.sqrMag());

  if (norm <= WFMATH_EPSILON) {
    m_valid = false;
    m_vec.setValid(false);
    return;
  }

  m_vec /= norm;
  m_w /= norm;

  m_valid = true;
  m_age = 1;
  m_vec.setValid();
}

inline AtlasOutType Quaternion::toAtlas() const
{
  _AtlasListType a(4);

  for(int i = 0; i < 3; ++i)
    a[i] = m_vec[i];
  a[3] = m_w;

  return a;
}

template<const int dim>
inline void Point<dim>::fromAtlas(const AtlasInType& a)
{
  _ArrayFromAtlas(m_elem, dim, a);
  m_valid = true;
}

template<const int dim>
inline AtlasOutType Point<dim>::toAtlas() const
{
  return _ArrayToAtlas(m_elem, dim);
}

template<const int dim>
void AxisBox<dim>::fromAtlas(const AtlasInType& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();

  const _AtlasListType& list(a.AsList());

  switch(list.size()) {
    case dim:
      m_low.setToOrigin();
      m_high.fromAtlas(a);
      break;
    case (2 * dim):
      for(int i = 0; i < dim; ++i) {
        m_low[i] = _asNum(list[i]);
        m_high[i] = _asNum(list[i+dim]);
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

template<const int dim>
AtlasOutType AxisBox<dim>::toAtlas() const
{
  int i;

  for(i = 0; i < dim; ++i)
    if(m_low[i] != 0)
      break;

  if(i == dim)
    return m_high.toAtlas(); // matches case 'dim' above

  // Do case '2 * dim' above

  _AtlasListType a(2*dim);
  for(i = 0; i < dim; ++i) {
    a[i] = m_low[i];
    a[dim+i] = m_high[i];
  }

  return a;
}

} // namespace WFMath

#endif // WFMATH_ATLAS_CONV_H
