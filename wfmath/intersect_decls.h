// -*-C++-*-
// intersect_decls.h (Declarations for "friend" intersection functions)
//
//  The WorldForge Project
//  Copyright (C) 2000, 2001  The WorldForge Project
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

#ifndef WFMATH_INTERSECT_DECLS_H
#define WFMATH_INTERSECT_DECLS_H

namespace WF { namespace Math {

template<const int dim> class Point;
template<const int dim> class AxisBox;
template<const int dim> class Ball;
template<const int dim> class Segment;
template<const int dim> class RotBox;

template<const int dim>
bool Intersect(const AxisBox<dim>& b, const Point<dim>& p);
template<const int dim>
bool IntersectProper(const AxisBox<dim>& b, const Point<dim>& p);
template<const int dim>
bool Contains(const Point<dim>& p, const AxisBox<dim>& b);

template<const int dim>
bool Intersect(const Ball<dim>& b, const Point<dim>& p);
template<const int dim>
bool IntersectProper(const Ball<dim>& b, const Point<dim>& p);
template<const int dim>
bool Contains(const Point<dim>& p, const Ball<dim>& b);

template<const int dim>
bool Intersect(const Segment<dim>& s, const Point<dim>& p);
template<const int dim>
bool IntersectProper(const Segment<dim>& s, const Point<dim>& p);
template<const int dim>
bool Contains(const Point<dim>& p, const Segment<dim>& s);

template<const int dim>
bool Intersect(const RotBox<dim>& r, const Point<dim>& p);
template<const int dim>
bool IntersectProper(const RotBox<dim>& r, const Point<dim>& p);
template<const int dim>
bool Contains(const Point<dim>& p, const RotBox<dim>& r);

template<const int dim>
bool Intersect(const AxisBox<dim>& b1, const AxisBox<dim>& b2);
template<const int dim>
bool IntersectProper(const AxisBox<dim>& b1, const AxisBox<dim>& b2);
template<const int dim>
bool Contains(const AxisBox<dim>& outer, const AxisBox<dim>& inner);
template<const int dim>
bool ContainsProper(const AxisBox<dim>& outer, const AxisBox<dim>& inner);

template<const int dim>
bool Intersect(const Ball<dim>& b, const AxisBox<dim>& a);
template<const int dim>
bool IntersectProper(const Ball<dim>& b, const AxisBox<dim>& a);
template<const int dim>
bool Contains(const Ball<dim>& b, const AxisBox<dim>& a);
template<const int dim>
bool ContainsProper(const Ball<dim>& b, const AxisBox<dim>& a);
template<const int dim>
bool Contains(const AxisBox<dim>& a, const Ball<dim>& b);
template<const int dim>
bool ContainsProper(const AxisBox<dim>& a, const Ball<dim>& b);

template<const int dim>
bool Intersect(const Segment<dim>& s, const AxisBox<dim>& b);
template<const int dim>
bool IntersectProper(const Segment<dim>& s, const AxisBox<dim>& b);
template<const int dim>
bool Contains(const Segment<dim>& s, const AxisBox<dim>& b);
template<const int dim>
bool ContainsProper(const Segment<dim>& s, const AxisBox<dim>& b);
template<const int dim>
bool Contains(const AxisBox<dim>& b, const Segment<dim>& s);
template<const int dim>
bool ContainsProper(const AxisBox<dim>& b, const Segment<dim>& s);

template<const int dim>
bool Intersect(const Ball<dim>& b1, const Ball<dim>& b2);
template<const int dim>
bool IntersectProper(const Ball<dim>& b1, const Ball<dim>& b2);
template<const int dim>
bool Contains(const Ball<dim>& outer, const Ball<dim>& inner);
template<const int dim>
bool ContainsProper(const Ball<dim>& outer, const Ball<dim>& inner);

template<const int dim>
bool Intersect(const Segment<dim>& s, const Ball<dim>& b);
template<const int dim>
bool IntersectProper(const Segment<dim>& s, const Ball<dim>& b);
template<const int dim>
bool Contains(const Ball<dim>& b, const Segment<dim>& s);
template<const int dim>
bool ContainsProper(const Ball<dim>& b, const Segment<dim>& s);
template<const int dim>
bool Contains(const Segment<dim>& s, const Ball<dim>& b);
template<const int dim>
bool ContainsProper(const Segment<dim>& s, const Ball<dim>& b);

template<const int dim>
bool Intersect(const Segment<dim>& s1, const Segment<dim>& s2);
template<const int dim>
bool IntersectProper(const Segment<dim>& s1, const Segment<dim>& s2);
template<const int dim>
bool Contains(const Segment<dim>& s1, const Segment<dim>& s2);
template<const int dim>
bool ContainsProper(const Segment<dim>& s1, const Segment<dim>& s2);

}} // namespace WF::Math

#endif  // WFMATH_INTERSECT_DECLS_H
