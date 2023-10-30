// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <wfmath/MersenneTwister.h>

#include <iostream>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <cassert>

int main()
{
    static WFMath::MTRand::uint32 results[] = { 2221777491u, 2873750246u, 4067173416u, 794519497u, 3287624630u, 3357287912u, 1212880927u, 2464917741u, 949382604u, 1898004827u };
    WFMath::MTRand rng;
    int exit = 0;

    rng.seed(23);
    // rng.randInt();

    for(int i = 0; i < 10; ++i) {
        WFMath::MTRand::uint32 rnd = rng.randInt();
        if (rnd != results[i]) {
            std::cerr << "Mismatch between QRNG and known result sequuence"
                      << std::endl
                      << rnd << " != " << results[i] << std::endl << std::flush;
            exit = 1;
        }
        assert(rnd == results[i]);
    }
    return exit;
}
