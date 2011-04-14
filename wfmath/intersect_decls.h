// intersect_decls.h (Declarations for "friend" intersection functions)
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
//

// Author: Ron Steinke

#ifndef WFMATH_INTERSECT_DECLS_H
#define WFMATH_INTERSECT_DECLS_H

#include <wfmath/const.h>

namespace WFMath {

// Some Intersect and Contains helper functions

inline bool _Less(CoordType x1, CoordType x2, bool proper)
{
  return !proper ? x1 < x2 : x1 <= x2;
}

inline bool _LessEq(CoordType x1, CoordType x2, bool proper)
{
  return !proper ? x1 <= x2 : x1 < x2;
}

inline bool _Greater(CoordType x1, CoordType x2, bool proper)
{
  return !proper ? x1 > x2 : x1 >= x2;
}

inline bool _GreaterEq(CoordType x1, CoordType x2, bool proper)
{
  return !proper ? x1 >= x2 : x1 > x2;
}

template<int dim>
bool Intersect(const AxisBox<dim>& b, const Point<dim>& p, bool proper);
template<int dim>
bool Contains(const Point<dim>& p, const AxisBox<dim>& b, bool proper);

template<int dim>
bool Intersect(const Ball<dim>& b, const Point<dim>& p, bool proper);
template<int dim>
bool Contains(const Point<dim>& p, const Ball<dim>& b, bool proper);

template<int dim>
bool Intersect(const Segment<dim>& s, const Point<dim>& p, bool proper);
template<int dim>
bool Contains(const Point<dim>& p, const Segment<dim>& s, bool proper);

template<int dim>
bool Intersect(const RotBox<dim>& r, const Point<dim>& p, bool proper);
template<int dim>
bool Contains(const Point<dim>& p, const RotBox<dim>& r, bool proper);

template<int dim>
bool Intersect(const AxisBox<dim>& b1, const AxisBox<dim>& b2, bool proper);
template<int dim>
bool Contains(const AxisBox<dim>& outer, const AxisBox<dim>& inner, bool proper);

template<int dim>
bool Intersect(const Ball<dim>& b, const AxisBox<dim>& a, bool proper);
template<int dim>
bool Contains(const Ball<dim>& b, const AxisBox<dim>& a, bool proper);
template<int dim>
bool Contains(const AxisBox<dim>& a, const Ball<dim>& b, bool proper);

template<int dim>
bool Intersect(const Segment<dim>& s, const AxisBox<dim>& b, bool proper);
template<int dim>
bool Contains(const Segment<dim>& s, const AxisBox<dim>& b, bool proper);
template<int dim>
bool Contains(const AxisBox<dim>& b, const Segment<dim>& s, bool proper);

template<int dim>
bool Intersect(const RotBox<dim>& r, const AxisBox<dim>& b, bool proper);
template<int dim>
bool Contains(const RotBox<dim>& r, const AxisBox<dim>& b, bool proper);
template<int dim>
bool Contains(const AxisBox<dim>& b, const RotBox<dim>& r, bool proper);

template<int dim>
bool Intersect(const Ball<dim>& b1, const Ball<dim>& b2, bool proper);
template<int dim>
bool Contains(const Ball<dim>& outer, const Ball<dim>& inner, bool proper);

template<int dim>
bool Intersect(const Segment<dim>& s, const Ball<dim>& b, bool proper);
template<int dim>
bool Contains(const Ball<dim>& b, const Segment<dim>& s, bool proper);
template<int dim>
bool Contains(const Segment<dim>& s, const Ball<dim>& b, bool proper);

template<int dim>
bool Intersect(const RotBox<dim>& r, const Ball<dim>& b, bool proper);
template<int dim>
bool Contains(const RotBox<dim>& r, const Ball<dim>& b, bool proper);
template<int dim>
bool Contains(const Ball<dim>& b, const RotBox<dim>& r, bool proper);

template<int dim>
bool Intersect(const Segment<dim>& s1, const Segment<dim>& s2, bool proper);
template<int dim>
bool Contains(const Segment<dim>& s1, const Segment<dim>& s2, bool proper);

template<int dim>
bool Intersect(const RotBox<dim>& r, const Segment<dim>& s, bool proper);
template<int dim>
bool Contains(const RotBox<dim>& r, const Segment<dim>& s, bool proper);
template<int dim>
bool Contains(const Segment<dim>& s, const RotBox<dim>& r, bool proper);

template<int dim>
bool Intersect(const RotBox<dim>& r1, const RotBox<dim>& r2, bool proper);
template<int dim>
bool Contains(const RotBox<dim>& outer, const RotBox<dim>& inner, bool proper);

template<int dim>
bool Intersect(const Polygon<dim>& r, const Point<dim>& p, bool proper);
template<int dim>
bool Contains(const Point<dim>& p, const Polygon<dim>& r, bool proper);

template<int dim>
bool Intersect(const Polygon<dim>& p, const AxisBox<dim>& b, bool proper);
template<int dim>
bool Contains(const Polygon<dim>& p, const AxisBox<dim>& b, bool proper);
template<int dim>
bool Contains(const AxisBox<dim>& b, const Polygon<dim>& p, bool proper);

template<int dim>
bool Intersect(const Polygon<dim>& p, const Ball<dim>& b, bool proper);
template<int dim>
bool Contains(const Polygon<dim>& p, const Ball<dim>& b, bool proper);
template<int dim>
bool Contains(const Ball<dim>& b, const Polygon<dim>& p, bool proper);

template<int dim>
bool Intersect(const Polygon<dim>& r, const Segment<dim>& s, bool proper);
template<int dim>
bool Contains(const Polygon<dim>& p, const Segment<dim>& s, bool proper);
template<int dim>
bool Contains(const Segment<dim>& s, const Polygon<dim>& p, bool proper);

template<int dim>
bool Intersect(const Polygon<dim>& p, const RotBox<dim>& r, bool proper);
template<int dim>
bool Contains(const Polygon<dim>& p, const RotBox<dim>& r, bool proper);
template<int dim>
bool Contains(const RotBox<dim>& r, const Polygon<dim>& p, bool proper);

template<int dim>
bool Intersect(const Polygon<dim>& p1, const Polygon<dim>& p2, bool proper);
template<int dim>
bool Contains(const Polygon<dim>& outer, const Polygon<dim>& inner, bool proper);

} // namespace WFMath

#endif  // WFMATH_INTERSECT_DECLS_H
