// timestamp.cpp (time and random number implementation)
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

#if defined _MSC_VER || defined __MINGW32__
#include <sys/timeb.h>
#else
#include <unistd.h>	
#include <sys/time.h>
#endif

#include "timestamp.h"

#ifdef __WIN32__
#include <winsock2.h> 
#endif

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

static const long Million = 1000000;

// apparently, some architectures use int instead of long
// in struct timeval
template<class T1, class T2>
static void regularize(T1 &sec, T2 &usec)
{
  if(usec >= Million) {
    usec -= Million;
    ++sec;
  }
  else if(usec < 0) {
    usec += Million;
    --sec;
  }
}

namespace WFMath {

TimeDiff::TimeDiff(long sec, long usec, bool is_valid) : m_isvalid(is_valid),
  m_sec(sec), m_usec(usec)
{
    if (m_isvalid)
        regularize(m_sec, m_usec);
}

TimeDiff::TimeDiff(long msec) : 
    m_isvalid(true),
    m_sec(msec / 1000),
    m_usec(msec % 1000)
{
  if(msec < 0) {
    --m_sec;
    if(m_usec < 0) // behavior of % is machine dependent
      m_usec += Million;
  }
}

long TimeDiff::milliseconds() const
{
  return m_sec * 1000 + m_usec / 1000;
}

TimeDiff& operator+=(TimeDiff &val, const TimeDiff &d)
{
  val.m_sec += d.m_sec;
  val.m_usec += d.m_usec;
  val.m_isvalid = val.m_isvalid && d.m_isvalid;

    if (val.m_isvalid)
        regularize(val.m_sec, val.m_usec);

  return val;
}

TimeDiff& operator-=(TimeDiff &val, const TimeDiff &d)
{
  val.m_sec -= d.m_sec;
  val.m_usec -= d.m_usec;
  val.m_isvalid = val.m_isvalid && d.m_isvalid;
    
    if (val.m_isvalid)
        regularize(val.m_sec, val.m_usec);

  return val;
}

TimeDiff operator+(const TimeDiff &a, const TimeDiff &b)
{
  return TimeDiff(a.m_sec + b.m_sec, a.m_usec + b.m_usec, a.m_isvalid && b.m_isvalid);
}

TimeDiff operator-(const TimeDiff &a, const TimeDiff &b)
{
  return TimeDiff(a.m_sec - b.m_sec, a.m_usec - b.m_usec, a.m_isvalid && b.m_isvalid);
}

bool operator<(const TimeDiff &a, const TimeDiff &b)
{
  return (a.m_sec < b.m_sec) || ((a.m_sec == b.m_sec) && (a.m_usec < b.m_usec));
}

bool operator==(const TimeDiff &a, const TimeDiff &b)
{
  return (a.m_sec == b.m_sec) && (a.m_usec == b.m_usec);
}

TimeStamp TimeStamp::now()
{
    TimeStamp ret;
#ifndef __WIN32__
    gettimeofday(&ret._val, NULL);
#else
  SYSTEMTIME sysTime;
  FILETIME fileTime;  /* 100ns == 1 */
  LARGE_INTEGER i;

  GetSystemTime(&sysTime);
  SystemTimeToFileTime(&sysTime, &fileTime);
  /* Documented as the way to get a 64 bit from a
   * FILETIME. */
  memcpy(&i, &fileTime, sizeof(LARGE_INTEGER));

    ret._val.tv_sec = i.QuadPart / 10000000; /*10e7*/
    ret._val.tv_usec = (i.QuadPart / 10) % 1000000;  /*10e6*/
                                  
#endif
    ret._isvalid = true;
    return ret;
}

TimeStamp TimeStamp::epochStart()
{
  TimeStamp ret;

  ret._val.tv_sec = 0;
  ret._val.tv_usec = 0;
  ret._isvalid = true;

  return ret;
}

// FIXME C++0x supports initialising _val this way
// _val({sec, usec}),
TimeStamp::TimeStamp(long sec, long usec, bool isvalid) : _isvalid(isvalid)
{
  _val.tv_sec = sec;
  _val.tv_usec = usec;
  
  if (_isvalid)
    regularize(_val.tv_sec, _val.tv_usec);
}

bool operator<(const TimeStamp &a, const TimeStamp &b)
{
	if (a._val.tv_sec == b._val.tv_sec)
		return (a._val.tv_usec < b._val.tv_usec);
	else
		return a._val.tv_sec < b._val.tv_sec;
}

bool operator==(const TimeStamp &a, const TimeStamp &b)
{
	return (a._val.tv_sec == b._val.tv_sec)
		&& (a._val.tv_usec == b._val.tv_usec);
}

TimeStamp& operator+=(TimeStamp &a, const TimeDiff &d)
{
  a._val.tv_sec += d.m_sec;
  a._val.tv_usec += d.m_usec;
  a._isvalid = a._isvalid && d.m_isvalid;

    if (a._isvalid)
        regularize(a._val.tv_sec, a._val.tv_usec);
  return a;
}

TimeStamp& operator-=(TimeStamp &a, const TimeDiff &d)
{
  a._val.tv_sec -= d.m_sec;
  a._val.tv_usec -= d.m_usec;
  a._isvalid = a._isvalid && d.m_isvalid;

    if (a._isvalid)
        regularize(a._val.tv_sec, a._val.tv_usec);
  return a;
}

TimeStamp operator+(const TimeStamp &a, const TimeDiff &d)
{
  return TimeStamp(a._val.tv_sec + d.m_sec, a._val.tv_usec + d.m_usec,
	a._isvalid && d.m_isvalid);
}

TimeStamp operator-(const TimeStamp &a, const TimeDiff &d)
{
  return TimeStamp(a._val.tv_sec - d.m_sec, a._val.tv_usec - d.m_usec,
	a._isvalid && d.m_isvalid);
}

TimeDiff operator-(const TimeStamp &a, const TimeStamp &b)
{
  return TimeDiff(a._val.tv_sec - b._val.tv_sec,
    a._val.tv_usec - b._val.tv_usec, a._isvalid && b._isvalid);
}

}
