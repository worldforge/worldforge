#include <Mercator/RandCache.h>
#include <iostream>


int main(){

    SpiralOrdering m(1,1);

    for (int i =2;i>-3;i--) {
        std::cout << "y" << i << ":";
        for (int j =-2;j<3;j++) {
            std::cout << j << " " <<  m(j,i) << " || ";
        }
        std::cout << std::endl;
    }

    RandCache rc(123, new ZeroSpiralOrdering());
    RandCache rc2(123, new ZeroSpiralOrdering());
    RandCache rc3(323, new ZeroSpiralOrdering());

    std::cout << rc(2,2) << std::endl;
    std::cout << rc2(2,2) << std::endl;
    std::cout << rc3(2,2) << std::endl;

//    std::cout << rc(1,2) << ":" << rc(10,12) << rc(11,13) << rc(2,2) << std::endl;
    return 0;
}
