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

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/quaternion.h>
#include <wfmath/axisbox.h>
#include <wfmath/polygon.h>
#include <wfmath/ball.h>
#include <wfmath/rotbox.h>
#include <wfmath/line.h>

#include <cmath>

namespace WFMath {

#ifndef ATLAS_MESSAGE_ELEMENT_H
#error "You must include Atlas/Message/Element.h before wfmath/atlasconv.h"
#endif

typedef Atlas::Message::WrongTypeException _AtlasBadParse;

class AtlasInType
{
 public:
  AtlasInType(const Atlas::Message::Element& val) : m_val(val) {}
  // allow nice conversions when necessary
  template<class C> AtlasInType(C c) : m_obj(c), m_val(m_obj) {}
  operator const Atlas::Message::Element&() const {return m_val;}
  bool IsList() const {return m_val.isList();}
  const Atlas::Message::ListType& AsList() const {return m_val.asList();}
 private:
  Atlas::Message::Element m_obj;
  const Atlas::Message::Element& m_val;
};

class AtlasOutType
{
 public:
  AtlasOutType(const Atlas::Message::ListType& l) : m_val(l) {}
  AtlasOutType(const Atlas::Message::MapType& l) : m_val(l) {}
  operator Atlas::Message::Element&() {return m_val;}
  operator const Atlas::Message::Element&() const {return m_val;}
 private:
  Atlas::Message::Element m_val;
};

inline AtlasOutType _ArrayToAtlas(const CoordType* array, unsigned len)
{
  Atlas::Message::ListType a(len);

  for(unsigned i = 0; i < len; ++i)
    a[i] = array[i];

  return a;
}

inline void _ArrayFromAtlas(CoordType* array, unsigned len, const AtlasInType& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();

  const Atlas::Message::ListType& list(a.AsList());

  if(list.size() != (unsigned int) len)
    throw _AtlasBadParse();

  for(unsigned i = 0; i < len; ++i)
    array[i] = list[i].asNum();
}

template<int dim>
inline Vector<dim>::Vector(const AtlasInType& a)
{
  fromAtlas(a);
}

template<int dim>
inline void Vector<dim>::fromAtlas(const AtlasInType& a)
{
  _ArrayFromAtlas(m_elem, dim, a);
  m_valid = true;
}

template<int dim>
inline AtlasOutType Vector<dim>::toAtlas() const
{
  return _ArrayToAtlas(m_elem, dim);
}

inline void Quaternion::fromAtlas(const AtlasInType& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();


  const Atlas::Message::ListType& list(a.AsList());

  if(list.size() != 4)
    throw _AtlasBadParse();


  for(int i = 0; i < 3; ++i)
    m_vec[i] = list[i].asNum();

  m_w = list[3].asNum();

  CoordType norm = std::sqrt(m_w * m_w + m_vec.sqrMag());

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
  Atlas::Message::ListType a(4);

  for(int i = 0; i < 3; ++i)
    a[i] = m_vec[i];
  a[3] = m_w;

  return a;
}

template<int dim>
inline Point<dim>::Point(const AtlasInType& a)
{
  fromAtlas(a);
}

template<int dim>
inline void Point<dim>::fromAtlas(const AtlasInType& a)
{
  _ArrayFromAtlas(m_elem, dim, a);
  m_valid = true;
}

template<int dim>
inline AtlasOutType Point<dim>::toAtlas() const
{
  return _ArrayToAtlas(m_elem, dim);
}

template<int dim>
inline AxisBox<dim>::AxisBox(const AtlasInType& a)
{
  fromAtlas(a);
}

template<int dim>
inline void AxisBox<dim>::fromAtlas(const AtlasInType& a)
{
  if(!a.IsList())
    throw _AtlasBadParse();

  const Atlas::Message::ListType& list(a.AsList());

  switch(list.size()) {
    case dim:
      m_low.setToOrigin();
      m_high.fromAtlas(a);
      break;
    case (2 * dim):
      for(int i = 0; i < dim; ++i) {
        m_low[i] = list[i].asNum();
        m_high[i] = list[i+dim].asNum();
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

template<int dim>
inline AtlasOutType AxisBox<dim>::toAtlas() const
{
  int i;

  for(i = 0; i < dim; ++i)
    if(m_low[i] != 0)
      break;

  if(i == dim)
    return m_high.toAtlas(); // matches case 'dim' above

  // Do case '2 * dim' above

  Atlas::Message::ListType a(2*dim);
  for(i = 0; i < dim; ++i) {
    a[i] = m_low[i];
    a[dim+i] = m_high[i];
  }

  return a;
}

template<int dim>
inline void Ball<dim>::fromAtlas(const AtlasInType& a)
{
  const Atlas::Message::Element& message(a);
  if (message.isMap()) {
    const Atlas::Message::MapType& shapeElement(message.asMap());
    // Get sphere's radius
    Atlas::Message::MapType::const_iterator shape_I = shapeElement.find("radius");
    if (shape_I != shapeElement.end()) {
      const Atlas::Message::Element& shapeRadiusElem(shape_I->second);
      if (shapeRadiusElem.isNum()) {
        m_radius = shapeRadiusElem.asNum();
      }
    }
    Atlas::Message::MapType::const_iterator pos_I = shapeElement.find("position");
    if (pos_I != shapeElement.end()) {
      const Atlas::Message::Element& posElem(pos_I->second);
      if (posElem.isList()) {
        m_center.fromAtlas(posElem);
      }
    }
  }
}

template<int dim>
inline AtlasOutType Ball<dim>::toAtlas() const
{
  Atlas::Message::MapType map;
  map.insert(Atlas::Message::MapType::value_type("radius", m_radius));
  map.insert(Atlas::Message::MapType::value_type("position", m_center.toAtlas()));
  return map;
}

template<int dim>
inline Ball<dim>::Ball(const AtlasInType& a) : m_center(Point<dim>::ZERO()),
                                               m_radius(0)
{
  fromAtlas(a);
}

inline bool _ListNumCheck(const Atlas::Message::ListType & list, int dim)
{
  for(size_t i = 0; i < dim; ++i) {
    if (!list[i].isNum()) {
      return false;
    }
  }
  return true;
}

template<template <int> class ShapeT>
inline void _AddCorner(ShapeT<3> & shape,
                       const Atlas::Message::ListType & point)
{
  WFMath::Point<3> wpt(point[0].asNum(), point[1].asNum(), point[2].asNum());
  shape.addCorner(shape.numCorners(), wpt);
}

template<template <int> class ShapeT>
inline void _AddCorner(ShapeT<2> & shape,
                       const Atlas::Message::ListType & point)
{
  WFMath::Point<2> wpt(point[0].asNum(), point[1].asNum());
  shape.addCorner(shape.numCorners(), wpt);
}

template<template <int> class ShapeT, int dim>
inline void _CornersFromAtlas(ShapeT<dim> & shape,
                              const Atlas::Message::Element& message)
{
  if (message.isList()) {
    const Atlas::Message::ListType& pointsData(message.asList());
    
    for (size_t p = 0; p < pointsData.size(); ++p) {
      if (!pointsData[p].isList()) {
        continue;
      }
      
      const Atlas::Message::ListType& point(pointsData[p].asList());
      if ((point.size() < dim) || !_ListNumCheck(point, dim)) {
        continue;
      }
      
      _AddCorner(shape, point);
    }
  }
}

inline void Polygon<2>::fromAtlas(const AtlasInType& a)
{
  const Atlas::Message::Element& message(a);
  if (message.isMap()) {
    const Atlas::Message::MapType& shapeElement(message.asMap());
    Atlas::Message::MapType::const_iterator it = shapeElement.find("points");
    if ((it != shapeElement.end()) && it->second.isList()) {
      _CornersFromAtlas(*this, it->second);
      if (numCorners() > 2) {
        return;
      }
    }
  } else if (message.isList()) {
    _CornersFromAtlas(*this, message);
    if (numCorners() > 2) {
      return;
    }
  }
  throw _AtlasBadParse();
}

inline AtlasOutType Polygon<2>::toAtlas() const
{
  Atlas::Message::ListType points;
  for (theConstIter I = m_points.begin(); I != m_points.end(); ++I) 
  {
    points.push_back(I->toAtlas());
  }
  Atlas::Message::MapType map;
  map.insert(Atlas::Message::MapType::value_type("points", points));
  return map;
}

template<int dim>
inline void Line<dim>::fromAtlas(const AtlasInType& a)
{
  const Atlas::Message::Element& message(a);
  if (message.isMap()) {
    const Atlas::Message::MapType& shapeElement(message.asMap());
    Atlas::Message::MapType::const_iterator it = shapeElement.find("points");
    if ((it != shapeElement.end()) && it->second.isList()) {
      _CornersFromAtlas(*this, it->second);
      if (numCorners() > 1) {
        return;
      }
    }
  } else if (message.isList()) {
    _CornersFromAtlas(*this, message);
    if (numCorners() > 1) {
      return;
    }
  }
  throw _AtlasBadParse();
}

template<int dim>
inline AtlasOutType Line<dim>::toAtlas() const
{
  Atlas::Message::ListType points;
  for (const_iterator I = m_points.begin(); I != m_points.end(); ++I) 
  {
    points.push_back(I->toAtlas());
  }
  Atlas::Message::MapType map;
  map.insert(Atlas::Message::MapType::value_type("points", points));
  return map;
}

template<int dim>
inline Line<dim>::Line(const AtlasInType& a) {
  fromAtlas(a);
}

template<int dim>
inline void RotBox<dim>::fromAtlas(const AtlasInType& a)
{
  const Atlas::Message::Element& message(a);
  if (message.isMap()) {
    const Atlas::Message::MapType& shapeElement(message.asMap());
    // Get rotbox's position
    Atlas::Message::MapType::const_iterator shape_I = shapeElement.find("point");
    if (shape_I != shapeElement.end()) {
      const Atlas::Message::Element& shapePointElem(shape_I->second);
      WFMath::Point<dim> shapePoint;
      shapePoint.fromAtlas(shapePointElem);
      // Get rotbox's vector
      shape_I = shapeElement.find("size");
      if (shape_I != shapeElement.end()) {
        const Atlas::Message::Element& shapeVectorElem(shape_I->second);
        WFMath::Vector<dim> shapeVector;
        shapeVector.fromAtlas(shapeVectorElem);
        m_corner0 = shapePoint;
        m_size = shapeVector;
        m_orient = WFMath::RotMatrix<dim>().identity(); //TODO: parse rotation matrix (is it needed?)
        return;
      }
    }
  }
  throw _AtlasBadParse();
}

template<int dim>
inline AtlasOutType RotBox<dim>::toAtlas() const
{
  Atlas::Message::MapType map;
  map.insert(Atlas::Message::MapType::value_type("point", m_corner0.toAtlas()));
  map.insert(Atlas::Message::MapType::value_type("size", m_size.toAtlas()));
  //TODO: also add the rotmatrix
  return map;
}

template<int dim>
inline RotBox<dim>::RotBox(const AtlasInType& a) {
  fromAtlas(a);
}

} // namespace WFMath

#endif // WFMATH_ATLAS_CONV_H
