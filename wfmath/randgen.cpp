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

WFMath::MTRand WFMath::MTRand::instance;

void WFMath::MTRand::seed()
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

