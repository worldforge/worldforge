// -*-C++-*-
// error.h (Class structures for errors thrown by the WFMath library)
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

#ifndef WFMATH_ERROR_H
#define WFMATH_ERROR_H

#include <stdexcept>
#include <wfmath/vector.h>

namespace WFMath {

/// An error thrown by certain functions when passed parallel vectors.
template<int dim>
struct ColinearVectors : virtual public std::exception {
  ColinearVectors(const Vector<dim>& v1_in, const Vector<dim>& v2_in)
    : v1(v1_in), v2(v2_in) {}
  virtual ~ColinearVectors() throw () { }

  Vector<dim> v1, v2;
};

/// An error thrown by operator>>() when it fails to parse wfmath types
struct ParseError : virtual public std::exception {
  virtual ~ParseError() throw () { }
};

} // namespace WFMath

#endif // WFMATH_ERROR_H
