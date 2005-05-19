// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Al Riddoch

#include <wfmath/polygon_intersect.h>

#include <iostream>

int main()
{
    WFMath::Polygon<2> p1, p2, p3, p4;
    p1.addCorner(0, WFMath::Point<2>(0,0));
    p1.addCorner(1, WFMath::Point<2>(0,10));
    p1.addCorner(2, WFMath::Point<2>(10,10));
    p1.addCorner(3, WFMath::Point<2>(10,0));

    p2.addCorner(0, WFMath::Point<2>(0,0));
    p2.addCorner(1, WFMath::Point<2>(0,5));
    p2.addCorner(2, WFMath::Point<2>(5,5));
    p2.addCorner(3, WFMath::Point<2>(5,0));

    p3.addCorner(0, WFMath::Point<2>(1,1));
    p3.addCorner(1, WFMath::Point<2>(1,5));
    p3.addCorner(2, WFMath::Point<2>(5,5));
    p3.addCorner(3, WFMath::Point<2>(5,1));

    p4.addCorner(0, WFMath::Point<2>(15,15));
    p4.addCorner(1, WFMath::Point<2>(15,20));
    p4.addCorner(2, WFMath::Point<2>(20,20));
    p4.addCorner(3, WFMath::Point<2>(20,15));

    WFMath::AxisBox<2> box1(WFMath::Point<2>(0,0), WFMath::Point<2>(10,10));

    if (WFMath::Intersect(p1, p2, true)) {
        std::cout << "p1 and p2 intersect propper" << std::endl << std::flush;
    }

    if (WFMath::Intersect(p1, p2, true)) {
        std::cout << "p1 and p2 intersect propper" << std::endl << std::flush;
    }

    if (WFMath::Intersect(p1, p3, true)) {
        std::cout << "p1 and p3 intersect propper" << std::endl << std::flush;
    }

    if (WFMath::Intersect(p1, p4, true)) {
        std::cout << "p1 and p4 intersect propper" << std::endl << std::flush;
    }


    if (WFMath::Intersect(p1, box1, true)) {
        std::cout << "p1 and p2 intersect propper" << std::endl << std::flush;
    }

    if (WFMath::Intersect(p2, box1, true)) {
        std::cout << "p1 and p2 intersect propper" << std::endl << std::flush;
    }

    if (WFMath::Intersect(p3, box1, true)) {
        std::cout << "p1 and p3 intersect propper" << std::endl << std::flush;
    }

    if (WFMath::Intersect(p4, box1, true)) {
        std::cout << "p1 and p4 intersect propper" << std::endl << std::flush;
    }

}
