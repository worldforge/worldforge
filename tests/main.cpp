#include <Mercator/Segment.h>
#include <Mercator/Matrix.h>
#include <Mercator/BasePoint.h>

#include <iostream>

int main() 
{
    Mercator::Matrix<2, 2, Mercator::BasePoint> base;
    base[0] = Mercator::BasePoint(10.f, 3.f);
    base[1].height() = 15.f;
    base[2] = Mercator::BasePoint(10.f, 10.f);
    base[3] = Mercator::BasePoint(9.32f, 1.9f);
    
    int size = 64;
    Mercator::Segment *s = new Mercator::Segment(size);
    s->populate(base);
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
    
    return 0;
}

        
