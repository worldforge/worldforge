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

#include <wfmath/error.h>
#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/matrix.h>
#include <wfmath/point.h>
#include <wfmath/shape.h>
#include <wfmath/axisbox.h>

// Do not include stream.h, as it includes iostream.
// Let end users include it on their own.

// This file doesn't directly include header files which contain only
// functions which are intended for internal library use only (prefixed with '_').

// Any header file "foo_funcs.h" is automatically included in "foo.h",
// so those files are not listed here.

#endif // WFMATH_WFMATH_H
