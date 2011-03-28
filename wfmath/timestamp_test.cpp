#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif


#include <wfmath/timestamp.h>
#include <stdlib.h>
#include <iostream>

using namespace WFMath;
using std::cout;
using std::endl;

int main()
{

    TimeStamp tsa;
    if (tsa.isValid()) {
        cout << "default constructor returned valid" << endl;
        return EXIT_FAILURE;
    }
    
    tsa += TimeDiff(50);
    if (tsa.isValid()) {
        cout << "invalid stamp += valid time diff returned valid" << endl;
        return EXIT_FAILURE;
    }
    
    tsa = TimeStamp::now();
    if (!tsa.isValid()) return EXIT_FAILURE;
    
    tsa += TimeDiff();
    if (tsa.isValid()) {
        cout << "valid time += invalid diff returned valid" << endl;
        return EXIT_FAILURE;
    }
    
    TimeDiff tda(1000);
    if (!tda.isValid()) {
        cout << "construct diff from literal msecs is broken" << endl;
        return EXIT_FAILURE;
    }
    
    tsa = TimeStamp::now();
    TimeStamp tsb = tsa;
    if (tsa != tsb) {
        cout << "equality of stamps is broken" << endl;
        return EXIT_FAILURE; // equality is broken
    }
    
    tsb += tda;
    if (tsa >= tsb) return EXIT_FAILURE; // compare is broken
    
    return EXIT_SUCCESS;
}
