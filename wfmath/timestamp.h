// timestamp.h (time functions, taken from Eris)
//
//  The WorldForge Project
//  Copyright (C) 2002  The WorldForge Project
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2002-5-23

#ifndef WFMATH_TIMESTAMP_H
#define WFMATH_TIMESTAMP_H

#include <wfmath/const.h>
#include <algorithm> // For std::pair

namespace WFMath {

/** Timing related primitives in a portable fashion - note this is for interval / elapsed
time measurement, not displaying a human readable time. */

#ifdef _MSC_VER
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif
#if defined ( __WIN32__ )
#include <winsock.h> 
extern "C" {
  struct eris_timeval  {
    long tv_sec;	/* seconds */
    long tv_usec;	/* microseconds */
  };
}
#endif

class TimeStamp;

/// The difference between two timestamps
class TimeDiff
{
  TimeDiff(long sec, long usec, bool is_valid);
 public:
  TimeDiff() : m_isvalid(false) {}
  TimeDiff(long msec);
  // default copy constructor is fine

  long milliseconds() const; // Doesn't check overflow
  std::pair<long,long> full_time() const {return std::make_pair(m_sec,m_usec);}

  friend TimeDiff& operator+=(TimeDiff&, const TimeDiff&);
  friend TimeDiff& operator-=(TimeDiff&, const TimeDiff&);
  TimeDiff operator-() const {return TimeDiff(-m_sec, -m_usec, m_isvalid);}

  friend TimeDiff operator+(const TimeDiff &a, const TimeDiff &b);
  friend TimeDiff operator-(const TimeDiff &a, const TimeDiff &b);

  friend TimeStamp& operator+=(TimeStamp&, const TimeDiff&);
  friend TimeStamp& operator-=(TimeStamp&, const TimeDiff&);

  friend TimeStamp operator+(const TimeStamp &a, const TimeDiff &msec);
  friend TimeStamp operator-(const TimeStamp &a, const TimeDiff &msec);

  friend TimeDiff operator-(const TimeStamp &a, const TimeStamp &b);

  friend bool operator<(const TimeDiff&, const TimeDiff&);
  friend bool operator==(const TimeDiff&, const TimeDiff&);

 private:
  bool m_isvalid;
  long m_sec, m_usec;
};

inline bool operator>(const TimeDiff &a, const TimeDiff &b) {return b < a;}
inline bool operator<=(const TimeDiff &a, const TimeDiff &b) {return !(b < a);}
inline bool operator>=(const TimeDiff &a, const TimeDiff &b) {return !(a < b);}
inline bool operator!=(const TimeDiff &a, const TimeDiff &b) {return !(b == a);}

/// A time stamp
class TimeStamp {
 private:
#if defined( __WIN32__ )
	
  // We roll our own timeval... may only need to be done for mingw32.
  // FIXME since we're no longer doing a typedef, do we really need to do this?
  struct eris_timeval _val;

#elif defined( macintosh ) // This doesn't appear to be supported
  UnsignedWide _val;	// micro-seconds
#else
  // POSIX, BeOS, ....
  struct timeval _val;
#endif
  bool _isvalid;
  TimeStamp(long sec, long usec, bool isvalid);
 public:
  TimeStamp() : _isvalid(false) {}
  // default copy constructor is fine

  friend bool operator<(const TimeStamp &a, const TimeStamp &b);
  friend bool operator==(const TimeStamp &a, const TimeStamp &b);

  friend TimeStamp& operator+=(TimeStamp&, const TimeDiff&);
  friend TimeStamp& operator-=(TimeStamp&, const TimeDiff&);

  friend TimeStamp operator+(const TimeStamp &a, const TimeDiff &msec);
  friend TimeStamp operator-(const TimeStamp &a, const TimeDiff &msec);

  friend TimeDiff operator-(const TimeStamp &a, const TimeStamp &b);	

  static TimeStamp now();
  static TimeStamp epochStart(); // Jan 1, 1970
};

inline TimeStamp operator+(TimeDiff msec, const TimeStamp &a) {return a + msec;}

inline bool operator>(const TimeStamp &a, const TimeStamp &b) {return b < a;}
inline bool operator<=(const TimeStamp &a, const TimeStamp &b) {return !(b < a);}
inline bool operator>=(const TimeStamp &a, const TimeStamp &b) {return !(a < b);}
inline bool operator!=(const TimeStamp &a, const TimeStamp &b) {return !(b == a);}

} // namespace WFMath

#endif  // WFMATH_TIMESTAMP_H
