#include <time.h>
#include <sstream>
#include <iostream>

clock_t calc_time0,calc_time1;
double calc_time;

void printTime(const std::string& msg, long long iterations, double iterPerSec) {
    std::stringstream ss;
    ss << msg;
    while (ss.tellp() < 30) {
        ss << ' ';
    }
    ss << " iterations=" << iterations;
    while (ss.tellp() < 60) {
        ss << ' ';
    }
    ss <<" CPU Time="<<std::fixed<<calc_time;
    while (ss.tellp() < 80) {
        ss << ' ';
    }
    ss <<" iter/s="<<iterPerSec<<std::endl;
    std::cout << ss.str() << std::flush;
}


#define TIME_ON  calc_time0=clock();
#define TIME_OFF(msg) calc_time1=clock();                    \
  calc_time=(double)(calc_time1-calc_time0)/CLOCKS_PER_SEC;  \
  printTime(msg, i, i/calc_time);

