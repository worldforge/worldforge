#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Timestamp.h>

#ifndef _MSC_VER
#include <unistd.h>	
#include <sys/time.h>
#else
#include <sys/timeb.h>
#endif

bool Time::Stamp::_did_init = false;
	
Time::Stamp Time::Stamp::now()
{
    Stamp ret;
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
    return ret;
}

// The operator definitons need the Time:: prefix, since
// they were declared in that namespace
	
bool Time::operator<(const Time::Stamp &a, const Time::Stamp &b)
{
	if (a._val.tv_sec == b._val.tv_sec)
		return (a._val.tv_usec < b._val.tv_usec);
	else
		return a._val.tv_sec < b._val.tv_sec;
}

bool Time::operator>(const Time::Stamp &a, const Time::Stamp &b)
{
	if (a._val.tv_sec == b._val.tv_sec)
		return (a._val.tv_usec > b._val.tv_usec);
	else
		return a._val.tv_sec > b._val.tv_sec;
}

Time::Stamp Time::operator+(const Time::Stamp &a, long msec)
{
	Stamp ret = a;
	ret._val.tv_sec += msec / 1000;
	ret._val.tv_usec += (long) ((msec % 1000) * 1000);
	
	if (ret._val.tv_usec > 1000000) {
		ret._val.tv_sec++;
		ret._val.tv_usec-=1000000;
	}
	
	return ret;
}

Time::Stamp Time::operator-(const Time::Stamp &a, long msec)
{
	Stamp ret = a;
	ret._val.tv_sec -= msec / 1000;
	ret._val.tv_usec -= (msec % 1000) * 1000;
	
	if (ret._val.tv_usec < 0) {
		ret._val.tv_sec--;
		ret._val.tv_usec+=1000000;
	}
	
	return ret;
}

long Time::operator-(const Time::Stamp &a, const Time::Stamp &b)
{
	long ret = (a._val.tv_sec - b._val.tv_sec) * 1000;
	ret += (a._val.tv_usec - b._val.tv_usec) / 1000;
	return ret;
}

double Time::Stamp::seconds()
{
	double rv = _val.tv_sec + (_val.tv_usec * 1e-6);
	return rv;
}
