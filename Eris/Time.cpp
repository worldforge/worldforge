#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Time.h"

namespace Time
{

#include <unistd.h>	
#include <sys/time.h>
 
	/*
void Init()
{
	gettimeofday(&_startTime, NULL);
}
	*/
	
Stamp getCurrentStamp()
{
	Stamp n;
#ifndef __WIN32__
	gettimeofday(&n, NULL);
#else
  SYSTEMTIME sysTime;
  FILETIME fileTime;  /* 100ns == 1 */
  LARGE_INTEGER i;

  GetSystemTime(&sysTime);
  SystemTimeToFileTime(&sysTime, &fileTime);
  /* Documented as the way to get a 64 bit from a
   * FILETIME. */
  memcpy(&i, &fileTime, sizeof(LARGE_INTEGER));

  n.tv_sec = i.QuadPart / 10000000; /*10e7*/
  n.tv_usec = (i.QuadPart / 10) % 1000000;  /*10e6*/
                                  
#endif
	return n;
}
	
bool operator<(const Stamp &a, const Stamp &b)
{
	if (a.tv_sec == b.tv_sec)
		return (a.tv_usec < b.tv_usec);
	else
		return a.tv_sec < b.tv_sec;
}

Stamp operator+(const Stamp &a, unsigned long msec)
{
	Stamp ret = a;
	ret.tv_sec += msec / 1000;
	ret.tv_usec += (msec % 1000) * 1000;
	
	if (ret.tv_usec > 1000000) {
		ret.tv_sec++;
		ret.tv_usec-=1000000;
	}
	
	return ret;
}

Stamp operator-(const Stamp &a, unsigned long msec)
{
	Stamp ret = a;
	ret.tv_sec -= msec / 1000;
	ret.tv_usec -= (msec % 1000) * 1000;
	
	if (ret.tv_usec < 0) {
		ret.tv_sec--;
		ret.tv_usec+=1000000;
	}
	
	return ret;
}

long operator-(const Stamp &a, const Stamp &b)
{
	unsigned long ret = (a.tv_sec - b.tv_sec) * 1000;
	ret += (a.tv_usec - b.tv_usec) / 1000;
	return ret;
}

	
}
