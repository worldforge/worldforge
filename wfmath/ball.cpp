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

#include "ball_funcs.h"
#include "miniball_funcs.h"

#include <vector>
#include <cmath>

using namespace WFMath;

namespace WFMath {

template<> Ball<3>& Ball<3>::rotateCorner(const Quaternion&, int corner)
{
  abort();
  return *this;
}

template<> Ball<3>& Ball<3>::rotateCenter(const Quaternion&)
{
  return *this;
}

template<> Ball<3>& Ball<3>::rotatePoint(const Quaternion& q, const Point<3>& p)
{
  m_center.rotate(q, p); return *this;
}

template<> Ball<3> Ball<3>::toParentCoords(const Point<3>& origin,
                                           const Quaternion& rotation) const
{
  return Ball<3>(m_center.toParentCoords(origin, rotation), m_radius);
}

template<> Ball<3> Ball<3>::toLocalCoords(const Point<3>& origin,
                                          const Quaternion& rotation) const
{
  return Ball<3>(m_center.toLocalCoords(origin, rotation), m_radius);
}

template Ball<2> BoundingSphere<2, std::vector>(std::vector<Point<2>,
                                                std::allocator<Point<2> > > const&);

template Ball<2> BoundingSphereSloppy<2, std::vector>(std::vector<Point<2>,
                                                      std::allocator<Point<2> > > const&);

template Ball<2> Point<2>::boundingSphere() const;
template Ball<2> Point<2>::boundingSphereSloppy() const;

template Ball<3> Point<3>::boundingSphere() const;
template Ball<3> Point<3>::boundingSphereSloppy() const;

template class Ball<2>;
template class Ball<3>;

}
