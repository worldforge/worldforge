#ifndef FORGE_TIME_H
#define FORGE_TIME_H

/** Timing related primitives in a portable fashion - note this is for interval / elapsed
time measurement, not displaying a human readable time. */

#include <sys/time.h>

namespace Time {

#if defined( WIN32 )
	
	// ??

#elif defined( macintosh )
	typedef UnsignedWide Stamp;	// micro-seconds
#else
	// POSIX, BeOS, ....
	typedef struct timeval Stamp;
#endif	

bool operator<(const Stamp &a, const Stamp &b);

Stamp& operator+(const Stamp &a, unsigned long msec);
	
long operator-(const Stamp &a, const Stamp &b);	
	
void Init();	
	
Stamp getCurrentStamp();
	
}; // of namespace Time

#endif
