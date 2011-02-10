// randgen.h (random number functions)
//
//  The WorldForge Project
//  Copyright (C) 2002  The WorldForge Project
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
// Created: 2002-5-23

#ifndef WFMATH_RANDGEN_H
#define WFMATH_RANDGEN_H

#include <wfmath/MersenneTwister.h>

namespace WFMath {

// backwards compatibility functions

#ifdef WFMATH_USE_OLD_RAND

/// Seed WFMath's random number generators.
/**
 * The random number generators use a static instance of MTRand.
 **/
inline void SeedRand(unsigned int val) {MTRand::instance.seed(val);}
/// Get a random number between 0 and 1
inline double DRand() {return MTRand::instance.rand();}
/// Get a random integer ranging from 0 to (val passed) - 1
inline unsigned int IRand(unsigned int val) {return MTRand::instance.randInt(val - 1);}

#endif

} // namespace WFMath

#endif  // WFMATH_RANDGEN_H
