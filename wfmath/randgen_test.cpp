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

// Author: Alistair Riddoch
// Created: 2011-2-14

#include "randgen.h"
#include <cstdio>

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

using WFMath::MTRand;

int main()
{
    MTRand one(23);

    printf("%.16f %.16f\n", one.rand(), one.rand());
    float oneres = static_cast<float>(one.rand());

    MTRand two(23);

    printf("%.16f %.16f\n", two.randf(), two.randf());
    float twores = two.randf();

    MTRand thr(23);

    printf("%.16f %.16f\n", thr.rand<float>(), thr.rand<float>());
    float thrres = thr.rand<float>();

    printf("%.16f %.16f %.16f\n", oneres, twores, thrres);
}
