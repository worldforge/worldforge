// -*-C++-*-
// intersect.cpp (Backends to intersection functions)
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

#include "const.h"
#include "intersect.h"

using namespace WF::Math;
/*
void  WF::Math::_RotBoxAxisBoxIntersectImpl(int dim, int params, CoordType* matrix,
					    CoordType* low, CoordType* high)
{
  // Invert using row operations. First, make m upper triangular,
  // with 1's on the diagonal

  for(int i = 0; i < params; ++i) {

    // Make sure matrix[i*params+i] is nonzero
    CoordType minval = 0;
    for(int j = 0; j < dim; ++j)
      minval += matrix[j*params+i] * matrix[j*params+i]; // Don't need FloatAdd()
    minval /= WFMATH_MAX;
    if(minval < WFMATH_MIN)
      minval = WFMATH_MIN;
    if(matrix[i*params+i] * matrix[i*params+i] < minval) { // Find a nonzero element
      int j;
      for(j = i + 1;  j < dim; ++j)
        if(matrix[j*params+i] * matrix[j*params+i] >= minval)
          break;
      assert(j < dim); // Degenerate case dealt with earlier
      for(int k = 0; k < params; ++k)
        matrix[i*params+k] = FloatAdd(matrix[i*params+k], matrix[j*params+k]);
      low[i] = FloatAdd(low[i], low[j]);
      high[i] = FloatAdd(high[i], high[j]);
    }
    // We now know in[i*params+i] / in[j*params+i] >= sqrt(WFMATH_MIN) for any j

    // Normalize the row, so in[i*params+i] == 1
    CoordType tmp = matrix[i*params+i];
    matrix[i*params+i] = 1;
    for(int j = i + 1; j < params; ++j) // matrix[i*params+j] == 0 for j < i
        matrix[i*params+j] /= tmp;
    if(tmp > 0) {
      low[i] /= tmp;
      high[i] /= tmp;
    }
    else { // negative multiplier, swap sense of compare
      CoordType tmp2 = low[i] / tmp;
      low[i] = high[i] / tmp;
      high[i] = tmp2;
    }

    // Do row subtraction to make in[j*params+i] zero for j > i
    for(int j = i + 1; j < dim; ++j) {
      CoordType tmp = matrix[j*params+i];
      matrix[j*params+i] = 0;
      if(tmp != 0) {
        for(int k = i + 1; k < params; ++k)
          matrix[j*params+k] = FloatSubtract(matrix[j*params+k],
					     matrix[i*params+k] * tmp);
        if(tmp > 0) {
          low[j] = FloatSubtract(low[j], low[i] * tmp);
          high[j] = FloatSubtract(high[j], high[i] * tmp);
        }
        else {
          low[j] = FloatSubtract(low[j], high[i] * tmp);
          high[j] = FloatSubtract(high[j], low[i] * tmp);
        }
      }
    }
  }

  // Now perform row operations which would make the upper
  // square part of "matrix" into the identity matrix

  for(int i = params - 1; i >= 1; --i) {
    for(int j = i - 1; j >= 0; --j) {
      CoordType tmp = matrix[j*params+i];
      if(tmp != 0) {
        // Don't bother modifying matrix[j*params+k], we never use it again.
        if(tmp > 0) {
          low[j] = FloatSubtract(low[j], low[i] * tmp);
          high[j] = FloatSubtract(high[j], high[i] * tmp);
        }
        else {
          low[j] = FloatSubtract(low[j], high[i] * tmp);
          high[j] = FloatSubtract(high[j], low[i] * tmp);
        }
      }
    }
  }
}
*/
