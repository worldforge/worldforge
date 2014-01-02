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
#include <iosfwd>

/** Timing related primitives in a portable fashion - note this is for interval / elapsed
time measurement, not displaying a human readable time. */

#ifdef _MSC_VER
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

namespace WFMath {

class TimeStamp;

/// The difference between two timestamps
/**
 * This class implements the 'generic' subset of the interface in
 * the fake class Shape, with the exception of the stream operators.
 * It also has the full set of comparison * operators (<, <=, >, >=, ==, !=).
 **/
class TimeDiff
{
  TimeDiff(long sec, long usec, bool is_valid);
 public:
  /// construct an uninitialized TimeDiff
  TimeDiff() : m_isvalid(false) {}
  /// construct a TimeDiff of a given number of milliseconds
  TimeDiff(long msec);
  // default copy constructor is fine

  /// Get the value of a TimeDiff in milliseconds
  /**
   * WARNING! This function does not check for overflow, if the
   * number of milliseconds is large
   **/
  long milliseconds() const;
  /// Get the value of a TimeDiff in (seconds, microseconds)
  std::pair<long,long> full_time() const {return std::make_pair(m_sec,m_usec);}

  bool isValid() const {return m_isvalid;}

  /// increment a TimeDiff
  friend TimeDiff& operator+=(TimeDiff&, const TimeDiff&);
  /// decrement a TimeDiff
  friend TimeDiff& operator-=(TimeDiff&, const TimeDiff&);
  /// negate a TimeDiff
  TimeDiff operator-() const {return TimeDiff(-m_sec, -m_usec, m_isvalid);}

  /// add two TimeDiff instances
  friend TimeDiff operator+(const TimeDiff &a, const TimeDiff &b);
  /// subtract two TimeDiff instances
  friend TimeDiff operator-(const TimeDiff &a, const TimeDiff &b);

  /// advance a TimeStamp by a TimeDiff
  friend TimeStamp& operator+=(TimeStamp&, const TimeDiff&);
  /// regress a TimeStamp by a TimeDiff
  friend TimeStamp& operator-=(TimeStamp&, const TimeDiff&);

  /// find the result of advancing a TimeStamp
  friend TimeStamp operator+(const TimeStamp &a, const TimeDiff &msec);
  /// find the result of regressing a TimeStamp
  friend TimeStamp operator-(const TimeStamp &a, const TimeDiff &msec);

  /// find the time difference between two time stamps
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
/**
 * This class implements the 'generic' subset of the interface in
 * the fake class Shape, with the exception of the stream operators.
 * It also has the full set of comparison operators (<, <=, >, >=, ==, !=).
 **/
class TimeStamp {
 private:
#ifdef _WIN32 
  // We roll our own timeval... may only need to be done for mingw32.
  struct {
    long tv_sec;	/* seconds */
    long tv_usec;	/* microseconds */
  } _val;
#else
  // POSIX, BeOS, ....
  struct timeval _val;
#endif
  bool _isvalid;
  TimeStamp(long sec, long usec, bool isvalid);
 public:
  /// Construct an uninitialized TimeStamp
  TimeStamp() : _isvalid(false) {}
  // default copy constructor is fine

  friend bool operator<(const TimeStamp &a, const TimeStamp &b);
  friend bool operator==(const TimeStamp &a, const TimeStamp &b);

  friend std::ostream& operator<<(std::ostream& os, const TimeStamp&);
  friend std::istream& operator>>(std::istream& is, TimeStamp&);

  bool isValid() const {return _isvalid;}
  ///
  friend TimeStamp& operator+=(TimeStamp&, const TimeDiff&);
  ///
  friend TimeStamp& operator-=(TimeStamp&, const TimeDiff&);

  ///
  friend TimeStamp operator+(const TimeStamp &a, const TimeDiff &msec);
  ///
  friend TimeStamp operator-(const TimeStamp &a, const TimeDiff &msec);

  ///
  friend TimeDiff operator-(const TimeStamp &a, const TimeStamp &b);	

  /// set a TimeStamp to the current time
  static TimeStamp now();
  /// set a TimeStamp to Jan 1, 1970
  static TimeStamp epochStart();
};

///
inline TimeStamp operator+(TimeDiff msec, const TimeStamp &a) {return a + msec;}

inline bool operator>(const TimeStamp &a, const TimeStamp &b) {return b < a;}
inline bool operator<=(const TimeStamp &a, const TimeStamp &b) {return !(b < a);}
inline bool operator>=(const TimeStamp &a, const TimeStamp &b) {return !(a < b);}
inline bool operator!=(const TimeStamp &a, const TimeStamp &b) {return !(b == a);}

} // namespace WFMath

#endif  // WFMATH_TIMESTAMP_H
