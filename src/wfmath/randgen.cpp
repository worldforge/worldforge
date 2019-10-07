// randgen.cpp (time and random number implementation)
//
//  The WorldForge Project
//  Copyright (C) 2002, 2013  The WorldForge Project
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
//
//  Portions of the code in this file were copied and modified from
//  http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c
//
//  Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    1. Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//    2. Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//    3. The names of its contributors may not be used to endorse or promote
//       products derived from this software without specific prior written
//       permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

// Original Author: Ron Steinke
// Created: 2002-5-23

#include "randgen.h"
#include "timestamp.h"
#include <ctime>
#include <cstdio>
#include <istream>
#include <ostream>

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

namespace WFMath {

const MTRand::uint32 MTRand::state_size;

MTRand MTRand::instance;

static const MTRand::uint32 period = 397;
static const MTRand::uint32 MATRIX_A = 0x9908b0df;
static const MTRand::uint32 UPPER_MASK = 0x80000000;
static const MTRand::uint32 LOWER_MASK = 0x7fffffff;


static MTRand::uint32 hash( time_t t, clock_t c )
{
  // Get a uint32 from t and c
  // Better than uint32(x) in case x is floating point in [0,1]
  // Based on code by Lawrence Kirby (fred@genesis.demon.co.uk)

  typedef MTRand::uint32 uint32;

  // guarantee time-based seeds will change
  static uint32 differ = 0;

  uint32 h1 = 0;
  unsigned char *p = (unsigned char *) &t;
  for( size_t i = 0; i < sizeof(t); ++i )
  {
    h1 *= UCHAR_MAX + 2U;
    h1 += p[i];
  }
  uint32 h2 = 0;
  p = (unsigned char *) &c;
  for( size_t j = 0; j < sizeof(c); ++j )
  {
    h2 *= UCHAR_MAX + 2U;
    h2 += p[j];
  }
  return ( h1 + differ++ ) ^ h2;
}


void MTRand::seed()
{
  // First try getting an array from /dev/urandom
  FILE* urandom = fopen( "/dev/urandom", "rb" );
  if( urandom )
  {
    uint32 init_vector[state_size];
    uint32 *s = init_vector;
    int i = state_size;
    bool success = true;
    while( success && i-- )
      success = fread( s++, sizeof(uint32), 1, urandom );
    fclose(urandom);
    if( success ) { seed( init_vector, state_size );  return; }
  }

  // Was not successful, so use time() and clock() instead
  seed( hash( time(NULL), clock() ) );
}


void MTRand::seed(uint32 s)
{
  state[0] = s;
  for (index = 1; index < state_size; ++index)
  {
    state[index] = (1812433253UL * (state[index-1] ^ (state[index-1] >> 30)) + index);
  }
}


void MTRand::seed(uint32* const init_vector, const uint32 init_vector_length)
{
  seed(19650218UL);
  uint32 i = 1;
  uint32 j = 0;
  uint32 k = (state_size > init_vector_length ? state_size : init_vector_length);
  for( ; k; --k )
  {
    state[i] ^= ((state[i-1] ^ (state[i-1] >> 30)) * 1664525UL);
    state[i] += (init_vector[j] & 0xffffffffUL) + j;
    ++i;  ++j;
    if (i >= state_size) { state[0] = state[state_size-1];  i = 1; }
    if (j >= init_vector_length) j = 0;
  }
  for (k = state_size - 1; k; --k)
  {
    state[i] ^= ((state[i-1] ^ (state[i-1] >> 30)) * 1566083941UL);
    state[i] -= i;
    ++i;
    if (i >= state_size) { state[0] = state[state_size-1];  i = 1; }
  }
  state[0] = 0x80000000UL;  // MSB is 1, assuring non-zero initial array
}


MTRand::uint32 MTRand::randInt()
{
  uint32 y;
  static unsigned long mag01[2]={0x0UL, MATRIX_A};

  /* generate state_size words at one time */
  if (index >= state_size)
  {
    uint32 kk;
    for (kk=0; kk < state_size - period; kk++)
    {
      y = (state[kk]&UPPER_MASK) | (state[kk+1]&LOWER_MASK);
      state[kk] = state[kk + period] ^ (y >> 1) ^ mag01[y & 0x01];
    }
    for (; kk < state_size-1; kk++)
    {
        y = (state[kk]&UPPER_MASK) | (state[kk+1]&LOWER_MASK);
        state[kk] = state[kk+(period - state_size)] ^ (y >> 1) ^ mag01[y & 0x01];
    }
    y = (state[state_size-1]&UPPER_MASK) | (state[0]&LOWER_MASK);
    state[state_size-1] = state[period-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

    index = 0;
  }

  y = state[index++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}


std::ostream& MTRand::save(std::ostream& ostr) const
{
  for (uint32 i = 0; i < state_size; ++i)
  {
    ostr << state[i] << '\t';
  }
  return ostr << index;
}


std::istream& MTRand::load(std::istream& istr)
{
  for (uint32 i = 0; i < state_size; ++i)
    istr >> state[i];
  istr >> index;
  return istr;
}

}
