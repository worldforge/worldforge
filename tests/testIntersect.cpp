// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/Intersect.h>

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

    segment->populate();
    
    //test box definitely outside terrain
    WFMath::AxisBox<3> highab(WFMath::Point<3> (10.0, 10.0, segment->getMax() + 3.0), 
                          WFMath::Point<3> (20.0, 20.0, segment->getMax() + 6.1));

    if (Mercator::Intersect(terrain, highab)) {
        std::cerr << "axisbox intersects with terrain even though it should be above it"
            << std::endl;
        return 1;
    }

    //test box definitely inside terrain
    WFMath::AxisBox<3> lowab(WFMath::Point<3> (10.0, 10.0, segment->getMin() - 6.1), 
                          WFMath::Point<3> (20.0, 20.0, segment->getMax() - 3.0));

    if (!Mercator::Intersect(terrain, lowab)) {
        std::cerr << "axisbox does not intersect with terrain even though it should be below it"
            << std::endl;
        return 1;
    }


    //test axis box moved from above terrain to below it. 
    bool inter=false;
    float dz = highab.highCorner()[2] - highab.lowCorner()[2] - 0.1;
    while (highab.highCorner()[2] > segment->getMin()) {
        highab.shift(WFMath::Vector<3>(0.0, 0.0, -dz));
        if (Mercator::Intersect(terrain, highab)) {
            inter=true;
            break;
        }
    }
    
    if (!inter) {
        std::cerr << "axisbox passed through terrain with no intersection"
            << std::endl;
        return 1;
    }
     

    //test axisbox that spans two segments
    terrain.setBasePoint(0, 2, 4.8);
    terrain.setBasePoint(1, 2, 3.7);

    Mercator::Segment *segment2 = terrain.getSegment(0, 1);
    segment2->populate();

    float segmax=std::max(segment->getMax(), segment2->getMax());
    float segmin=std::min(segment->getMin(), segment2->getMin());
    
    WFMath::AxisBox<3> ab(WFMath::Point<3> (50.0, 10.0, segmax + 3.0), 
                          WFMath::Point<3> (70.0, 20.0, segmax + 6.1));

    if (Mercator::Intersect(terrain, ab)) {
        std::cerr << "axisbox2 intersects with terrain even though it should be above it"
            << std::endl;
        return 1;
    }

    WFMath::AxisBox<3> ab2(WFMath::Point<3> (50.0, 10.0, segmin - 6.1), 
                          WFMath::Point<3> (70.0, 20.0, segmin + 3.0));

    if (!Mercator::Intersect(terrain, ab2)) {
        std::cerr << "axisbox2 does not intersect with terrain even though it should be below it"
            << std::endl;
        return 1;
    }


    float xxx;
    return Mercator::Intersect(terrain, WFMath::Point<3>(20.1, 20.2, 20.0), 
                               WFMath::Vector<3>(-3.0,4.0,-18.0), xxx);

    return 0;
}
