// polygon_funcs.h (Polygon<> intersection functions)
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
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.
//

// Author: Ron Steinke
// Created: 2002-2-20

#ifndef WFMATH_POLYGON_INTERSECT_H
#define WFMATH_POLYGON_INTERSECT_H

#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/polygon.h>
#include <wfmath/intersect.h>
#include <wfmath/error.h>

#include <cmath>

#include <cassert>

// FIXME Work is needed on this code. At very least the following notes
// from the original author apply:
// "The Intersect() and Contains() functions involving WFMath::Polygon<>"
// "are still under development, and probably shouldn't be used yet."

namespace WFMath {


template<>
bool Intersect(const Polygon<2>& r, const Point<2>& p, bool proper);
template<>
bool Contains(const Point<2>& p, const Polygon<2>& r, bool proper);

template<>
bool Intersect(const Polygon<2>& p, const AxisBox<2>& b, bool proper);
template<>
bool Contains(const Polygon<2>& p, const AxisBox<2>& b, bool proper);
template<>
bool Contains(const AxisBox<2>& b, const Polygon<2>& p, bool proper);

template<>
bool Intersect(const Polygon<2>& p, const Ball<2>& b, bool proper);
template<>
bool Contains(const Polygon<2>& p, const Ball<2>& b, bool proper);
template<>
bool Contains(const Ball<2>& b, const Polygon<2>& p, bool proper);

template<>
bool Intersect(const Polygon<2>& r, const Segment<2>& s, bool proper);
template<>
bool Contains(const Polygon<2>& p, const Segment<2>& s, bool proper);
template<>
bool Contains(const Segment<2>& s, const Polygon<2>& p, bool proper);

template<>
bool Intersect(const Polygon<2>& p, const RotBox<2>& r, bool proper);
template<>
bool Contains(const Polygon<2>& p, const RotBox<2>& r, bool proper);
template<>
bool Contains(const RotBox<2>& r, const Polygon<2>& p, bool proper);

template<>
bool Intersect(const Polygon<2>& p1, const Polygon<2>& p2, bool proper);
template<>
bool Contains(const Polygon<2>& outer, const Polygon<2>& inner, bool proper);

} // namespace WFMath

#endif  // WFMATH_POLYGON_INTERSECT_H
