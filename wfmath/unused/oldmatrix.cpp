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
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2001-12-7

#include "matrix_funcs.h"
#include "const.h"
#include <float.h>

namespace WF { namespace Math {

double _MatrixDeterminantImpl(const int size, double* m)
{
  // First, construct an upper triangular matrix with the
  // same determinant as the original matrix. Then just
  // multiply the diagonal terms to get the determinant.

  for(int i = 0; i < size - 1; ++i) {
    double minval = 0;
    for(int j = 0; j < size; ++j)
      minval += m[j*size+i] * m[j*size+i];
    minval /= DBL_MAX;
    if(minval < DBL_MIN)
      minval = DBL_MIN;
    if(m[i*size+i] * m[i*size+i] < minval) { // Find a row with nonzero element
      int j;
      for(j = i + 1; j < size; ++j)
        if(m[j*size+i] * m[j*size+i] >= minval)
          break;
      if(j == size) // No nonzero element found, degenerate matrix, det == 0
        return 0;
      m[i*size+i] = m[j*size+i];
      for(int k = i + 1; k < size; ++k) { // For k < i, m[j*size+k] == 0
        if(m[j*size+k] != 0 && fabs(m[i*size+k] / m[j*size+k] + 1) < WFMATH_EPSILON)
          m[i*size+k] = 0;
        else
          m[i*size+k] += m[j*size+k];
      }
    }
    for(int j = i + 1; j < size; ++j) {
      double factor = m[j*size+i] / m[i*size+i];
      // We know factor isn't bigger than about sqrt(DBL_MAX), due to
      // the comparison with minval done above.
      m[j*size+i] = 0;
      if(factor != 0) {
        for(int k = i + 1; k < size; ++k) { // For k < i, m[k*size+j] == 0
          double sub_val = m[i*size+k] * factor;
          if(m[j*size+k] != 0 && fabs(sub_val / m[j*size+k] - 1) < WFMATH_EPSILON)
            m[j*size+k] = 0;
          else
            m[j*size+k] -= sub_val;
        }
      }
    }
  }

  double out = 1;

  for(int i = 0; i < size; ++i)
    out *= m[i*size+i];

  return out;
}

int _MatrixInverseImpl(const int size, double* in, double* out)
{
  // Invert using row operations. First, make m upper triangular,
  // with 1's on the diagonal

  for(int i = 0; i < size; ++i) {

    // Make sure in[i*size+i] is nonzero
    double minval = 0;
    for(int j = 0; j < size; ++j)
      minval += in[j*size+i] * in[j*size+i];
    minval /= DBL_MAX;
    if(minval < DBL_MIN)
      minval = DBL_MIN;
    if(in[i*size+i] * in[i*size+i] < minval) { // Find a nonzero element
      int j;
      for(j = i + 1;  j < size; ++j)
        if(in[j*size+i] * in[j*size+i] >= minval)
          break;
      if(j == size) // degenerate matrix
        return -1;
      for(int k = 0; k < size; ++k) {
        if(in[j*size+k] != 0 && fabs(out[i*size+k] / in[j*size+k] + 1) < WFMATH_EPSILON)
          out[i*size+k] = 0;
        else
          out[i*size+k] += in[j*size+k];
        if(in[j*size+k] != 0 && fabs(in[i*size+k] / in[j*size+k] + 1) < WFMATH_EPSILON)
          in[i*size+k] = 0;
        else
          in[i*size+k] += in[j*size+k];
      }
    }
    // We now know in[i*size+i] / in[j*size+i] >= sqrt(DBL_MIN) for any j

    // Normalize the row, so in[i*size+i] == 1
    double tmp = in[i*size+i];
    in[i*size+i] = 1;
    for(int j = 0; j < size; ++j) {
      out[i*size+j] /= tmp;
      if(j > i) // in[i*size+j] == 0 for j < i
        in[i*size+j] /= tmp;
    }

    // Do row subtraction to make in[j*size+i] zero for j > i
    for(int j = i + 1; j < size; ++j) {
      double tmp = in[j*size+i];
      in[j*size+i] = 0;
      if(tmp != 0) {
        for(int k = 0; k < size; ++k) {
          double in_sub_val = in[i*size+k] * tmp;
          double out_sub_val = out[i*size+k] * tmp;
          if(out[j*size+k] != 0 && fabs(out_sub_val / out[j*size+k] - 1) < WFMATH_EPSILON)
            out[j*size+k] = 0;
          else
            out[j*size+k] -= out_sub_val;
          if(k > i) { // in[j*size+k] == 0 for k < i
            if(in[j*size+k] != 0 && fabs(in_sub_val / in[j*size+k] - 1) < WFMATH_EPSILON)
              in[j*size+k] = 0;
            else
              in[j*size+k] -= in_sub_val;
          }
        }
      }
    }
  }

  // Now perform row operations on "out" which would make "m"
  // into the identity matrix

  for(int i = size - 1; i >= 1; --i) {
    for(int j = i - 1; j >= 0; --j) {
      double tmp = in[j*size+i];
      if(tmp != 0)
        for(int k = 0; k < size; ++k)
          out[j*size+k] -= out[i*size+k] * tmp;
          // Don't bother modifying in[j*size+k], we never use it again.
    }
  }

  return 0;
}

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
