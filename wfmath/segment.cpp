// vector.cpp (Vector<> implementation)
//
//  The WorldForge Project
//  Copyright (C) 2011  The WorldForge Project
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

// Author: Alistair Riddoch
// Created: 2011-1-29

// Extensive amounts of this material come from the Vector2D
// and Vector3D classes from stage/math, written by Bryce W.
// Harrington, Kosh, and Jari Sundell (Rakshasa).

#include "segment_funcs.h"

#include "axisbox.h"
#include "ball.h"
#include "vector.h"

#include <cmath>

using namespace WFMath;

namespace WFMath {

template<> Segment<3>& Segment<3>::rotatePoint(const Quaternion& q,
                                               const Point<3>& p)
{
  m_p1.rotate(q, p);
  m_p2.rotate(q, p);
  return *this;
}

template<> Segment<3>& Segment<3>::rotateCenter(const Quaternion& q)
{
  rotatePoint(q, getCenter());
  return *this;
}

template<> Segment<3> Segment<3>::toParentCoords(const Point<3>& origin,
                                                 const Quaternion& rotation) const
{
  return Segment(m_p1.toParentCoords(origin, rotation),
                 m_p2.toParentCoords(origin, rotation));
}

template<> Segment<3> Segment<3>::toLocalCoords(const Point<3>& origin,
                                                const Quaternion& rotation) const
{
  return Segment(m_p1.toLocalCoords(origin, rotation),
                 m_p2.toLocalCoords(origin, rotation));
}


template class Segment<2>;
template class Segment<3>;

}
