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

#ifndef WFMATH_ATLAS_FUNCS_H
#define WFMATH_ATLAS_FUNCS_H

#include <Atlas/Message/Object.h>

#include <wfmath/vector.h>
#include <wfmath/matrix.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>

namespace WF { namespace Math {

bool _GetAtlasDoubleList(const Atlas::Message::Object& a, double* d, int num);
Atlas::Message::Object _SetAtlasDoubleList(double* d, int num);

template<const int len>
bool FromAtlas(Vector<len>& v, const Atlas::Message::Object& a)
{
  double d[len];

  if(GetAtlasDoubleList(a, d, len)) {
    v = d;
    return true;
  }
  else
    return false;
}

template<const int len>
Atlas::Message::Object ToAtlas(const Vector<len>& v)
{
  double d[len];

  for(int i = 0; i < len; ++i)
    d[i] = v[i];

  return SetAtlasDoubleList(d, len);
}

template<const int size>
bool FromAtlas(RotMatrix<size>& m, const Atlas::Message::Object& a)
{
  const int nParams = RotMatrix<size>::nParams;
  double d[nParams];

  if(GetAtlasDoubleList(a, d, nParams)) {
    m.fromEuler(d);
    return true;
  }
  else
    return false;
}

template<const int size>
Atlas::Message::Object ToAtlas(const RotMatrix<size>& m)
{
  const int nParams = RotMatrix<size>::nParams;
  double d[nParams];

  m.toEuler(d);

  return SetAtlasDoubleList(d, nParams);
}

template<const int dim>
bool FromAtlas(Point<dim>& p, const Atlas::Message::Object& a)
{
  double d[dim];

  if(GetAtlasDoubleList(a, d, dim)) {
    p = d;
    return true;
  }
  else
    return false;
}

template<const int dim>
Atlas::Message::Object ToAtlas(const Point<dim>& p)
{
  double d[dim];

  for(int i = 0; i < dim; ++i)
    d[i] = p[i];

  return SetAtlasDoubleList(d, dim);
}

template<const int dim>
bool FromAtlas(AxisBox<dim>& a, const Atlas::Message::Object& a)
{
  double d[2*dim];

  if(GetAtlasDoubleList(a, d, 2*dim)) {
    Point<dim> p_low, p_high;
    for(int i = 0; i < dim; ++i) {
      p_low[i] = d[i];
      p_high[i] = d[i+dim];
    }
    a = AxisBox<dim>(p_low, p_high);
    return true;
  }
  else
    return false;
}

template<const int dim>
Atlas::Message::Object ToAtlas(const Point<dim>& p)
{
  double d[2*dim];

  for(int i = 0; i < dim; ++i) {
    d[i] = lowerBound(i);
    d[dim+i] = upperBound(i);
  }

  return SetAtlasDoubleList(d, dim);
}

}} // namespace WF:Math

#endif // WFMATH_ATLAS_FUNCS_H
