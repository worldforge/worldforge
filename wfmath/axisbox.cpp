// vector.cpp (Vector<> implementation)
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

// Author: Alistair Riddoch
// Created: 2009-01-12

#include "axisbox_funcs.h"

#include "rotmatrix.h"
#include "vector.h"

#include <vector>

#include <cmath>

namespace WFMath {

template class AxisBox<3>;
template class AxisBox<2>;

template bool Intersection<3>(const AxisBox<3>&, const AxisBox<3>&, AxisBox<3>&);
template bool Intersection<2>(const AxisBox<2>&, const AxisBox<2>&, AxisBox<2>&);

template AxisBox<3> Union<3>(const AxisBox<3> &, const AxisBox<3> &);
template AxisBox<2> Union<2>(const AxisBox<2>&, const AxisBox<2>&);

template AxisBox<3> BoundingBox<3, std::vector>(const std::vector<AxisBox<3>, std::allocator<AxisBox<3> > > &);
template AxisBox<2> BoundingBox<2, std::vector>(const std::vector<AxisBox<2>, std::allocator<AxisBox<2> > > &);

template AxisBox<2> BoundingBox<2, std::vector>(const std::vector<Point<2>, std::allocator<Point<2> > >&);

template AxisBox<2> Point<2>::boundingBox() const;
template AxisBox<3> Point<3>::boundingBox() const;

}
