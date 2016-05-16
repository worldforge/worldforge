// -*-C++-*-
// matrix.cpp (Matrix<> implementation)
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
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2001-12-7

#include "matrix_funcs.h"
#include "const.h"
#include <float.h>

namespace WF { namespace Math {

Matrix<3> SkewSymmetric(const Vector<3>& v)
{
  Matrix<3> out;

  out.elem(0, 0) = out.elem(1, 1) = out.elem(2, 2) = 0;

  out.elem(0, 1) = v[2];
  out.elem(1, 0) = -v[2];
  out.elem(1, 2) = v[0];
  out.elem(2, 1) = -v[0];
  out.elem(2, 0) = v[1];
  out.elem(0, 2) = -v[1];

  return out;
}


}} // namespace WF::Math
