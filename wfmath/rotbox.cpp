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
// Created: 2011-1-27

// Extensive amounts of this material come from the Vector2D
// and Vector3D classes from stage/math, written by Bryce W.
// Harrington, Kosh, and Jari Sundell (Rakshasa).

#include "rotbox_funcs.h"

using namespace WFMath;

namespace WFMath {

template<> RotBox<3>& RotBox<3>::rotatePoint(const Quaternion& q, const Point<3>& p)
{
  m_orient = m_orient.rotate(q); m_corner0.rotate(q, p); return *this;
}

template<> RotBox<3>& RotBox<3>::rotateCorner(const Quaternion& q, int corner)
{
  rotatePoint(q, getCorner(corner)); return *this;
}

template<> RotBox<3>& RotBox<3>::rotateCenter(const Quaternion& q)
{
  rotatePoint(q, getCenter()); return *this;
}

template<> RotBox<3> RotBox<3>::toParentCoords(const Point<3>& origin,
                                               const Quaternion& rotation) const
{
  RotMatrix<3> orient = m_orient;
  return RotBox<3>(m_corner0.toParentCoords(origin, rotation), m_size, orient.rotate(rotation));
}

template<> RotBox<3> RotBox<3>::toLocalCoords(const Point<3>& origin,
                                              const Quaternion& rotation) const
{
  RotMatrix<3> orient = m_orient;
  return RotBox<3>(m_corner0.toLocalCoords(origin, rotation), m_size, orient.rotate(rotation.inverse()));
}

template class RotBox<2>;
template class RotBox<3>;

template Point<2> Point<2>::toLocalCoords(RotBox<2> const&) const;
template Point<3> Point<3>::toLocalCoords(RotBox<3> const&) const;
template Point<2> Point<2>::toParentCoords(RotBox<2> const&) const;
template Point<3> Point<3>::toParentCoords(RotBox<3> const&) const;

}
