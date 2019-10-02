// MersenneTwister.h
//
//  The WorldForge Project
//  Copyright (C) 2013  The WorldForge Project
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
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.
//
#ifndef WFMATH_MERSENNE_TWISTER_H_
#define WFMATH_MERSENNE_TWISTER_H_

#include <iosfwd>
#include <climits>
#include <cmath>
#include <stdint.h>

namespace WFMath {

class MTRand {
public:
  typedef uint32_t uint32;

  static const uint32 state_size = 624;

public:
  MTRand();
  MTRand(uint32 oneSeed);
  MTRand(uint32* const bigSeed, uint32 const seedLength = state_size);

  // real-valued random numbers on [0, 1] or [0, n]
  template<typename FloatT>
    FloatT rand();
  double rand();
  double rand(const double& n);

  // integer-valued random numbers on [0, 2^32-1] or [0, n]
  uint32 randInt();
  uint32 randInt(uint32 n);

  void seed();
  void seed(uint32 oneSeed);
  void seed(uint32* const init_vector, uint32 init_vector_length = state_size);

  std::ostream& save(std::ostream&) const;
  std::istream& load(std::istream&);

  static MTRand instance;

private:
  uint32 state[state_size];
  uint32 index;
};


inline MTRand::MTRand(uint32 s)
: index(0)
{ seed(s); }

inline MTRand::MTRand(uint32* const bigSeed, const uint32 seedLength)
: index(0)
{ seed(bigSeed, seedLength); }

inline MTRand::MTRand()
: index(0)
{ seed(); }

template<>
inline float MTRand::rand<float>()
{ return float(randInt()) * (1.0f/4294967295.0f); }

template<>
inline double MTRand::rand<double>()
{ return double(randInt()) * (1.0/4294967295.0); }

inline double MTRand::rand()
{ return double(randInt()) * (1.0/4294967295.0); }

inline double MTRand::rand( const double& n )
{ return rand() * n; }


inline MTRand::uint32 MTRand::randInt(uint32 n)
{
  uint32 used = n;
  used |= used >> 1;
  used |= used >> 2;
  used |= used >> 4;
  used |= used >> 8;
  used |= used >> 16;

  uint32 i;
  do
    i = randInt() & used;
  while( i > n );
  return i;
}


#if 0
inline void MTRand::save(uint32* saveArray) const
{
  register uint32 *sa = saveArray;
  register const uint32 *s = state;
  register int i = state_size;
  for( ; i--; *sa++ = *s++ ) {}
  *sa = left;
}


inline void MTRand::load(uint32 *const loadArray)
{
  register uint32 *s = state;
  register uint32 *la = loadArray;
  register int i = state_size;
  for( ; i--; *s++ = *la++ ) {}
  left = *la;
  pNext = &state[state_size-left];
}
#endif

std::ostream& operator<<(std::ostream& os, MTRand const& mtrand);
std::istream& operator>>(std::istream& is, MTRand& mtrand);

} // namespace

#endif // WFMATH_MERSENNE_TWISTER_H_
