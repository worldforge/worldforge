// -*-C++-*-
// const.h (Defined constants for the WFMath library)
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

#ifndef WFMATH_CONST_H
#define WFMATH_CONST_H

#include <float.h>
// The next two aren't used in this file, but are used
// everywhere else in the library.
#include <assert.h>
#include <math.h>

// Forward declarations for all classes
#include <iosfwd>
namespace Atlas { namespace Message { class Object;}}

namespace WFMath {

#define WFMATH_CONST_PI		3.141592653589793238462643383279
#define WFMATH_CONST_SQRT2	1.414213562373095048801688724210
#define WFMATH_CONST_SQRT3	1.732050807568877293527446341506

typedef float CoordType;

// This is the attempted precision of the library. It's essentially
// the machine precision multiplied by a fudge factor.
#define WFMATH_EPSILON		(30 * FLT_EPSILON)

// Max and min values of CoordType
#define WFMATH_MAX		FLT_MAX
#define WFMATH_MIN		FLT_MIN

bool IsFloatEqual(double x, double y, double epsilon = WFMATH_EPSILON);
inline double FloatAdd(double x, double y,
		       double epsilon = WFMATH_EPSILON)
	{return IsFloatEqual(x, -y, epsilon) ? 0 : x + y;}
inline double FloatSubtract(double x, double y,
			    double epsilon = WFMATH_EPSILON)
	{return IsFloatEqual(x, y, epsilon) ? 0 : x - y;}

// These let us avoid including <algorithm> for the sake of
// std::max() and std::min()
inline CoordType FloatMax(CoordType a, CoordType b)
	{return (a > b) ? a : b;}
inline CoordType FloatMin(CoordType a, CoordType b)
	{return (a < b) ? a : b;}
inline CoordType FloatClamp(CoordType val, CoordType min, CoordType max)
	{return (min >= val) ? min : (max <= val ? max : val);}

// This stuff came from libCoal

#ifdef WIN32
#define isnan _isnan
#endif

// Set NAN to be an impossible value, so isnan() will pick it up.
#ifdef NAN
const CoordType NanVal = NAN;
#else
const CoordType NanVal = sqrt (-4.0);
#endif

} // namespace WFMath

#endif // WFMATH_CONST_H
