// -*-C++-*-
// wfmath.h (General include file for the WFMath library)
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

#ifndef WFMATH_WFMATH_H
#define WFMATH_WFMATH_H

// Simple stuff
#include <wfmath/error.h>
#include <wfmath/const.h>
// Basic types
#include <wfmath/vector.h>
#include <wfmath/rotmatrix.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>
// Shape types
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>
#include <wfmath/segment.h>
#include <wfmath/rotbox.h>
#include <wfmath/polygon.h>
// Shape intersection functions
#include <wfmath/intersect.h>
// Probability and statistics
#include <wfmath/probability.h>
#include <wfmath/timestamp.h>
#include <wfmath/randgen.h>
#include <wfmath/shuffle.h>
// iostreams and strings
#include <wfmath/stream.h>
#include <wfmath/int_to_string.h>

// Don't include atlasconv.h, which includes <Atlas/Message/Object.h>
// There is, however, no linker dependency on atlas in the library,
// so it is quite safe to use WFMath without atlas, as long as you
// don't include this header

// This file doesn't include basis.h, which only contains declarations
// for some functions which are used in vector.cpp and point.cpp

// Any header file "foo_funcs.h" is automatically included in "foo.h",
// so those files are not listed here. Similarly, "intersect_decls.h"
// is included in "intersect.h" (among other places).

#endif // WFMATH_WFMATH_H
