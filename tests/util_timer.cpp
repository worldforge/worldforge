// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 Jari Sundell

#include "util_timer.h"

namespace Util {

Timer::Timer() {
  gettimeofday(&m_start,0);
}

void Timer::start() {
  gettimeofday(&m_start,0);
}

void Timer::stop() {
  gettimeofday(&m_stop,0);    
}

double Timer::interval() {
  struct timeval time;
  double inter;
  gettimeofday(&time,0);
  inter=(time.tv_sec-m_start.tv_sec)+((double)(time.tv_usec-m_start.tv_usec))/1000000.0;
  gettimeofday(&m_start,0);
  return inter;

}

double Timer::time() {
  return (m_stop.tv_sec-m_start.tv_sec)+((double)(m_stop.tv_usec-m_start.tv_usec))/1000000.0;
}

double Timer::now() {
  gettimeofday(&m_temp,0);
  return (double)(m_temp.tv_sec-m_start.tv_sec)+(double)(m_temp.tv_usec-m_start.tv_usec)/1000000.0;
}

double Timer::real() {
  struct timeval t;
  gettimeofday(&t,0);
  return (double)t.tv_sec + (double)t.tv_usec / 1000000.0;
}

}
