// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/Intersect.h>
#include "util_timer.h"
#include <iostream>

int main()
{
    Mercator::Terrain terrain;
    
    terrain.setBasePoint(0, 0, 2.8);
    terrain.setBasePoint(1, 0, 7.1);
    terrain.setBasePoint(0, 1, 0.2);
    terrain.setBasePoint(1, 1, 14.7);

    Mercator::Segment * segment = terrain.getSegment(0, 0);

    if (segment == 0) {
        std::cerr << "Segment not created by addition of required basepoints"
                  << std::endl << std::flush;
        return 1;
    }

    Util::Timer time;
    time.start();
    
    for (int q=0;q<1000;q++) {   
        segment->populate();
    }

    time.stop();
    std::cout << "time per segment = "<< (time.interval()/1.0) << " ms" << std::endl;
 
    return 0;
}
