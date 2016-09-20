// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/Intersect.h>
#include <Mercator/Forest.h>
#include <Mercator/Area.h>
#include "util_timer.h"
#include <iostream>

typedef WFMath::Point<2> Point2;

int main()
{
    Mercator::Terrain terrain;
    
    terrain.setBasePoint(0, 0, 2.8);
    terrain.setBasePoint(1, 0, 7.1);
    terrain.setBasePoint(0, 1, 0.2);
    terrain.setBasePoint(1, 1, 14.7);

    Mercator::Segment * segment = terrain.getSegmentAtIndex(0, 0);

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
 

    Mercator::Forest forest(4249162ul);
    WFMath::Polygon<2> p;
    p.addCorner(p.numCorners(), Point2(-50, -50));
    p.addCorner(p.numCorners(), Point2(50, -50));
    p.addCorner(p.numCorners(), Point2(50, 50));
    p.addCorner(p.numCorners(), Point2(-50, 50));    
    
    Mercator::Area* ar = new Mercator::Area(1, false);
    ar->setShape(p);
    forest.setArea(ar);
    
    time.start();
    for (int q=0;q<10;q++) {   
        forest.populate();
    }
    time.stop();

    std::cout << "time per 100x100 forest = "<< (time.interval()*100.0) << " ms" << std::endl;

    p.clear();
    p.addCorner(p.numCorners(), Point2(-100, -100));
    p.addCorner(p.numCorners(), Point2(100, -100));
    p.addCorner(p.numCorners(), Point2(100, 100));
    p.addCorner(p.numCorners(), Point2(-100, 100));    
    
    ar = new Mercator::Area(1, false);
    ar->setShape(p);
    forest.setArea(ar);
    
    time.start();
    for (int q=0;q<10;q++) {   
        forest.populate();
    }
    time.stop();


    std::cout << "time per 200x200 forest = "<< (time.interval()*100.0) << " ms" << std::endl;
    
    
    
    return 0;
}
