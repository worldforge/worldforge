// randgen.cpp (time and random number implementation)
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

#include "randgen.h"
#include "timestamp.h"

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

static bool seed()
{
  // seed with msecs in current time
  unsigned int seed_val = (unsigned int) (WFMath::TimeStamp::now()
	- WFMath::TimeStamp::epochStart()).milliseconds();

  // swap low and high bits, so the most random bits are
  // the most significant
  const int var_bits = sizeof(unsigned int) * 8;
  unsigned int swapped_seed_val = 0;
  for(int i = 0; i < var_bits / 2; ++i) {
    int shift = var_bits - (2 * i + 1);
    swapped_seed_val |= ((seed_val & (1 << i)) << shift)
                      | ((seed_val & (1 << (var_bits - (i + 1)))) >> shift);
  }

  srand(swapped_seed_val);

  return true;
}

// Force seeding at program start time
static bool seeded = seed(); 

// Use these to work around systems with
// poor implementations of rand()
static inline unsigned int MyRand() {return rand();}
const unsigned int MyRandMax = RAND_MAX;

void WFMath::SeedRand(unsigned int seed_val)
{
  srand(seed_val);
}

double WFMath::DRand() // returns between 0 and 1
{
  return ((double) MyRand()) / ((double) MyRandMax);
}

unsigned int WFMath::IRand(unsigned int max) // returns from 0 to arg-1
{
  assert("Need valid input" && max > 0);

  unsigned rand_max = MyRandMax, rand_val = MyRand();
  // The UINT_MAX check should make it easier for the
  // compiler to optimize this away when it's not needed.
  while(UINT_MAX > MyRandMax && max > rand_max) {
    if(max == rand_max + 1)
      return rand_val;
    // deal with 16 bit rand with 32 bit ints
    rand_val += MyRand() * (rand_max + 1);
    rand_max += MyRandMax * (rand_max + 1);
  }

  unsigned int divisor = rand_max / max;
  unsigned int ans = rand_val / divisor;

  if(ans < max)
    return ans;

  // Rounding error, rare case

  unsigned int leftover = rand_max - rand_val;
  assert(leftover < max);
  return leftover;
}
