// const.cpp (basic comparison functions)
//
//  The WorldForge Project
//  Copyright (C) 2000, 2001, 2002  The WorldForge Project
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

// Author: Ron Steinke

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <wfmath/const.h>

#include <cmath>

#include <cassert>

// Implementation of IsFloatEqual() is borrowed from Jesse Jones (thanks!).
// The comments also belong to him.  The names have been changed
// to protect the innocent....
//
// Comparing floating point numbers for equality is tricky because
// the limited precision of the hardware introduces small errors
// so that two numbers that should compare equal don't. So what
// we do is consider the numbers equal if their difference is less
// than some epsilon value. But choosing this epsilon is also tricky
// because if the numbers are large epsilon should also be large,
// and if the numbers are very small the epsilon must be even smaller.
// To get around this we'll use a technique from Knuth's "Seminumerical
// Algorithms" section 4.2.2 and scale epsilon by the exponent of
// one of the specified numbers.
//
//---------------------------------------------------------------

using namespace WFMath;

bool WFMath::Equal(double x1, double x2, double epsilon)
{
    // Hack to get around nonstandard std:: namespacing in MSVC
    using namespace std;

    // If the difference between the numbers is smaller than the
    // scaled epsilon we'll consider the numbers to be equal.

    return fabs(x1 - x2) <= _ScaleEpsilon(x1, x2, epsilon);
}

bool WFMath::Equal(float x1, float x2, double epsilon)
{
    // Hack to get around nonstandard std:: namespacing in MSVC
    using namespace std;

    // If the difference between the numbers is smaller than the
    // scaled epsilon we'll consider the numbers to be equal.

    return fabs(x1 - x2) <= _ScaleEpsilon(x1, x2, epsilon);
}

double WFMath::_ScaleEpsilon(double x1, double x2, double epsilon)
{
    // Hack to get around nonstandard std:: namespacing in MSVC
    using namespace std;

    // Get the exponent of the smaller of the two numbers (using the
    // smaller of the two gives us a tighter epsilon value).
    int exponent;
    (void) frexp(fabs(x1) < fabs(x2) ? x1 : x2, &exponent);   

    // Scale epsilon by the exponent.
    return ldexp(epsilon, exponent);
}

double WFMath::_ScaleEpsilon(const CoordType* x1, const CoordType* x2,
                             int length, double epsilon)
{
  assert(length > 0);

  double max1 = 0, max2 = 0;

  for(int i = 0; i < length; ++i) {
    double val1 = fabs(x1[i]), val2 = fabs(x2[i]);
    if(val1 > max1)
      max1 = val1;
    if(val2 > max2)
      max2 = val2;
  }

  return _ScaleEpsilon(max1, max2, epsilon);
}
