// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Jari Sundell

#ifndef STAGE_UTIL_TIMER
#define STAGE_UTIL_TIMER
#include <sys/time.h>
#include <unistd.h>

namespace Util {

class Timer {
 protected:
  struct timeval m_start;
  struct timeval m_stop;
  struct timeval m_temp;
 public:
  Timer();  
  void start();
  void stop();
  double interval();
  double time();
  double now();

  static double real();
};

}

#endif
