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
#include <ctime>
#include <cstdio>

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

namespace WFMath {

MTRand MTRand::instance;

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
	// Seed the generator with an array from /dev/urandom if available
	// Otherwise use a hash of time() and clock() values
	
	// First try getting an array from /dev/urandom
	FILE* urandom = fopen( "/dev/urandom", "rb" );
	if( urandom )
	{
		uint32 bigSeed[N];
		register uint32 *s = bigSeed;
		register int i = N;
		register bool success = true;
		while( success && i-- )
			success = fread( s++, sizeof(uint32), 1, urandom );
		fclose(urandom);
		if( success ) { seed( bigSeed, N );  return; }
	}
	
	// Was not successful, so use time() and clock() instead
	seed( hash( time(NULL), clock() ) );
}

}
