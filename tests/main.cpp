// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes, Alistair Riddoch

#include <Mercator/Segment.h>
#include <Mercator/Terrain.h>
#include <Mercator/Matrix.h>
#include <Mercator/BasePoint.h>

#include <iostream>

int main() 
{
    int size = 64;
    Mercator::Segment *s = new Mercator::Segment(0, 0, size);

    Mercator::Matrix<2, 2, Mercator::BasePoint> & base = s->getControlPoints();
    base[0] = Mercator::BasePoint(10.f, 3.f);
    base[1].height() = 15.f;
    base[2] = Mercator::BasePoint(10.f, 10.f);
    base[3] = Mercator::BasePoint(9.32f, 1.9f);
    
    s->populate();
/*    s->populateNormals(); 

    float *n=s->getNormals();
i*/    //for (int i=0; i<=size; ++i) {
        int i=0;
        for (int j=0; j<=size; ++j) {
                std::cout <<  j << ":" << s->get(i,j) << ", " ;
        }
       //}
/*          std::cout << s->get(i,j) << " ; ";
            std::cout << n[j * size * 3 + i * 3 ] << " : ";
            std::cout << n[j * size * 3 + i * 3 + 1] << " : ";
            std::cout << n[j * size * 3 + i * 3 + 2];
            std::cout << std::endl;
        }
            std::cout << std::endl;
    }
*/

    Mercator::Terrain t;
    t.setBasePoint(0,0,10.0);
    t.setBasePoint(0,1,12.0);
    t.setBasePoint(1,0,14.0);
    t.setBasePoint(1,1,16.0);
    std::cout << t.getSegmentAtIndex(0,0);
    t.setBasePoint(1,1,12.0);
    std::cout << t.getSegmentAtIndex(0,0);
    std::cout << std::endl;
    
    return 0;
}
