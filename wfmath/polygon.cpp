// polygon.cpp (Polygon<> implementation)
//
//  The WorldForge Project
//  Copyright (C) 2002  The WorldForge Project
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
// Created: 2002-1-4

#include "const.h"
#include "vector.h"
#include "point.h"
#include "axisbox.h"
#include "polygon.h"

using namespace WFMath;

void _Poly2Reorient::reorient(Polygon<2>& poly, int skip) const
{
  int end = poly.numCorners();

  switch(m_type) {
    case _WFMATH_POLY2REORIENT_NONE:
      return;
    case _WFMATH_POLY2REORIENT_CLEAR_AXIS2:
      for(int i = 0; i != end; ++i) {
        if(i == skip)
          continue;
        (poly[i])[1] = 0;
      }
      return;
    case _WFMATH_POLY2REORIENT_CLEAR_BOTH_AXES:
      for(int i = 0; i != end; ++i) {
        if(i == skip)
          continue;
        (poly[i])[0] = 0;
        (poly[i])[1] = 0;
      }
      return;
    case _WFMATH_POLY2REORIENT_MOVE_AXIS2_TO_AXIS1:
      for(int i = 0; i != end; ++i) {
        if(i == skip)
           continue;
        (poly[i])[0] = (poly[i])[1];
        (poly[i])[1] = 0;
      }
      return;
    case _WFMATH_POLY2REORIENT_SCALE1_CLEAR2:
      for(int i = 0; i != end; ++i) {
        if(i == skip)
          continue;
        (poly[i])[0] *= m_scale;
        (poly[i])[1] = 0;
      }
      return;
    default:
      assert(false);
      return;
  }
}

//template<>
bool Polygon<2>::isEqualTo(const Polygon<2>& p, double epsilon) const
{
  if(m_points.size() != p.m_points.size())
    return false;

  Polygon<2>::theConstIter i1 = m_points.begin(), i2 = p.m_points.begin(),
			   end = m_points.end();

  while(i1 != end) {
    if(!Equal(*i1, *i2, epsilon))
      return false;
    ++i1;
    ++i2;
  }

  return true;
}

bool Polygon<2>::isValid() const
{
  for(theConstIter i = m_points.begin(); i != m_points.end(); ++i)
    if(!i->isValid())
      return false;

  return true;
}

//template<>
bool Polygon<2>::operator< (const Polygon& p) const
{
  if(m_points.size() < p.m_points.size())
    return true;
  if(p.m_points.size() < m_points.size())
    return false;

  theConstIter i1 = m_points.begin(), i2 = p.m_points.begin(), end = m_points.end();

  while(i1 != end) {
    if(*i1 < *i2)
      return true;
    if(*i2 < *i1)
      return false;
    ++i1;
    ++i2;
  }

  return false;
}

//template<>
Polygon<2>& Polygon<2>::shift(const Vector<2>& v)
{
  for(theIter i = m_points.begin(); i != m_points.end(); ++i)
    *i += v;

  return *this;
}

//template<>
Polygon<2>& Polygon<2>::rotatePoint(const RotMatrix<2>& m, const Point<2>& p)
{
  for(theIter i = m_points.begin(); i != m_points.end(); ++i)
    i->rotate(m, p);

  return *this;
}
