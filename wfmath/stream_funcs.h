// -*-C++-*-
// stream_funcs.h (Functions in the WFMath library that use streams)
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
// Created: 2001-12-7

#ifndef WFMATH_STREAM_FUNCS_H
#define WFMATH_STREAM_FUNCS_H

#include <wfmath/vector.h>
#include <wfmath/matrix.h>
#include <wfmath/point.h>
#include <iostream>

namespace WF { namespace Math {

//TODO some template for all of these, that won't be too general
// and impact non-WFMath classes?

template<const int len>
inline std::ostream& operator<<(std::ostream& os, const Vector<len>& v)
{
  return os << v.toString();
}

template<const int size>
inline std::ostream& operator<<(std::ostream& os, const RotMatrix<size>& m)
{
  return os << m.toString();
}

template<const int dim>
inline std::ostream& operator<< (std::ostream& os, const Point<dim>& coord)
{
  return os << coord.toString();
}

}} // namespace WF::Math

#endif // WFMATH_STREAM_FUNCS_H
