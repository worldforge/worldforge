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

    Mercator::Segment * segment = terrain.getSegmentAtIndex(0, 0);

    if (segment == 0) {
        std::cerr << "Segment not created by addition of required basepoints"
                  << std::endl << std::flush;
        return 1;
    }

    segment->populate();
    
    //test box definitely outside terrain
    WFMath::AxisBox<3> highab(WFMath::Point<3> (10.0, segment->getMax() + 3.0, 10.0),
                          WFMath::Point<3> (20.0, segment->getMax() + 6.1, 20.0));

    if (Mercator::Intersect(terrain, highab)) {
        std::cerr << "axisbox intersects with terrain even though it should be above it"
            << std::endl;
        return 1;
    }

    //test box definitely inside terrain
    WFMath::AxisBox<3> lowab(WFMath::Point<3> (10.0, segment->getMin() - 6.1, 10.0),
                          WFMath::Point<3> (20.0, segment->getMax() - 3.0, 20.0));

    if (!Mercator::Intersect(terrain, lowab)) {
        std::cerr << "axisbox does not intersect with terrain even though it should be below it"
            << std::endl;
        return 1;
    }


    //test axis box moved from above terrain to below it. 
    bool inter=false;
    float dy = highab.highCorner()[1] - highab.lowCorner()[1] - 0.1;
    while (highab.highCorner()[1] > segment->getMin()) {
        highab.shift(WFMath::Vector<3>(0.0, -dy, 0.0));
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

    Mercator::Segment *segment2 = terrain.getSegmentAtIndex(0, 1);
    segment2->populate();

    float segmax=std::max(segment->getMax(), segment2->getMax());
    float segmin=std::min(segment->getMin(), segment2->getMin());
    
    WFMath::AxisBox<3> ab(WFMath::Point<3> (50.0, segmax + 3.0, 10.0),
                          WFMath::Point<3> (70.0, segmax + 6.1, 20.0));

    if (Mercator::Intersect(terrain, ab)) {
        std::cerr << "axisbox2 intersects with terrain even though it should be above it"
            << std::endl;
        return 1;
    }

    WFMath::AxisBox<3> ab2(WFMath::Point<3> (50.0, segmin - 6.1, 10.0),
                          WFMath::Point<3> (70.0, segmin + 3.0, 20.0));

    if (!Mercator::Intersect(terrain, ab2)) {
        std::cerr << "axisbox2 does not intersect with terrain even though it should be below it"
            << std::endl;
        return 1;
    }


    WFMath::Point<3> intPoint;
    WFMath::Vector<3> intNorm;
    float par;
    //test vertical ray
    if (Mercator::Intersect(terrain, WFMath::Point<3>(20.1, segmax + 3, 20.2),
                               WFMath::Vector<3>(0.0,50.0,0.0), intPoint, intNorm, par)) {
        std::cerr << "vertical ray intersected when it shouldnt" << std::endl;
        return 1;
    }
    
    if (!Mercator::Intersect(terrain, WFMath::Point<3>(20.1, segmax + 3, 20.2),
                               WFMath::Vector<3>(0.0,-50.0,0.0), intPoint, intNorm, par)) {
        std::cerr << "vertical ray didnt intersect when it should" << std::endl;
        return 1;
    }

    //test each quadrant
    if (!Mercator::Intersect(terrain, WFMath::Point<3>(20.1, segmax + 3, 20.2),
                               WFMath::Vector<3>(10.0,-100.0,10.0), intPoint, intNorm, par)) {
        std::cerr << "quad1 ray didnt intersect when it should" << std::endl;
        return 1;
    }

    if (!Mercator::Intersect(terrain, WFMath::Point<3>(20.1, segmax + 3, 20.2),
                               WFMath::Vector<3>(10.0,-50.0,-15.0), intPoint, intNorm, par)) {
        std::cerr << "quad2 ray didnt intersect when it should" << std::endl;
        return 1;
    }

    if (!Mercator::Intersect(terrain, WFMath::Point<3>(20.1, segmax + 3, 20.2),
                               WFMath::Vector<3>(-10.0,-50.0,-10.0), intPoint, intNorm, par)) {
        std::cerr << "quad3 ray didnt intersect when it should" << std::endl;
        return 1;
    }

    if (!Mercator::Intersect(terrain, WFMath::Point<3>(20.1, segmax + 3, 20.2),
                               WFMath::Vector<3>(-10.0,-50.0,10.0), intPoint, intNorm, par)) {
        std::cerr << "quad4 ray didnt intersect when it should" << std::endl;
        return 1;
    }
    
    //test dx==0 and dy==0
    if (!Mercator::Intersect(terrain, WFMath::Point<3>(20.1, segmax + 3, 20.2),
                               WFMath::Vector<3>(0.0,-50.0,10.0), intPoint, intNorm, par)) {
        std::cerr << "y+ ray didnt intersect when it should" << std::endl;
        return 1;
    }

    if (!Mercator::Intersect(terrain, WFMath::Point<3>(20.1, segmax + 3, 20.2),
                               WFMath::Vector<3>(0.0,-50.0,-10.0), intPoint, intNorm, par)) {
        std::cerr << "y- ray didnt intersect when it should" << std::endl;
        return 1;
    }

    if (!Mercator::Intersect(terrain, WFMath::Point<3>(20.1, segmax + 3, 20.2),
                               WFMath::Vector<3>(-10.0,-50.0,0.0), intPoint, intNorm, par)) {
        std::cerr << "x- ray didnt intersect when it should" << std::endl;
        return 1;
    }

    if (!Mercator::Intersect(terrain, WFMath::Point<3>(20.1, segmax + 3, 20.2),
                               WFMath::Vector<3>(10.0,-50.0,0.0), intPoint, intNorm, par)) {
        std::cerr << "x+ ray didnt intersect when it should" << std::endl;
        return 1;
    }
    
    //test a longer ray
    if (!Mercator::Intersect(terrain, WFMath::Point<3>(-10.08, segmax + 3, -20.37),
                               WFMath::Vector<3>(100.0,-50.0,183.0), intPoint, intNorm, par)) {
        std::cerr << "long ray didnt intersect when it should" << std::endl;
        return 1;
    }
 
    //check the height value
    float h;
    WFMath::Vector<3> n;
    terrain.getHeightAndNormal(intPoint[0], intPoint[2], h, n);
    n.normalize();

    if (n != intNorm) {
        std::cerr << "calculated normal is different from getHeightAndNormal" << std::endl;
        std::cerr << intPoint << std::endl;
        std::cerr << intNorm << "!=" << n << std::endl;
       // return 1;
    }
    
    // We can't check for equality here is it just doesn't work with
    // floats. Look it up in any programming book if you don't believe me.
    //  - 20040721 <alriddoch@zepler.org>
    if (fabs(h - intPoint[1]) > 0.00001) {
        std::cerr << "calculated height is different from getHeightAndNormal" << std::endl;
        std::cerr << h << "!=" << intPoint[1] << std::endl;
        return 1;
    }
        
    return 0;
}
