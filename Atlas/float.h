// float.h (Float comparison for the Atlas library, copied from WFMath's const.h)
//
//  The WorldForge Project
//  Copyright (C) 2001, 2002  The WorldForge Project
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
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2001-12-7

#ifndef ATLAS_FLOAT_H
#define ATLAS_FLOAT_H

#include <limits>

namespace Atlas {

bool Equal(double x1, double x2, double epsilon = std::numeric_limits<double>::epsilon());
bool Equal(float x1, float x2, float epsilon = std::numeric_limits<float>::epsilon());

} // namespace Atlas

#endif // ATLAS_FLOAT_H
