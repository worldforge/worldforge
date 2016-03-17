#include <time.h>

clock_t calc_time0,calc_time1;
double calc_time;
#define TIME_ON  calc_time0=clock();
#define TIME_OFF(msg) calc_time1=clock();                    \
  calc_time=(double)(calc_time1-calc_time0)/CLOCKS_PER_SEC;  \
  std::cout<<msg<<": iterations="<<i                          \
      <<" CPU Time="<<std::fixed<<calc_time                              \
      <<" iter/s="<<i/calc_time<<std::endl<<std::flush;

