#ifndef FORGE_TIME_H
#define FORGE_TIME_H

/** Timing related primitives in a portable fashion - note this is for interval / elapsed
time measurement, not displaying a human readable time. */

#include <sys/time.h>
#if defined ( __WIN32__ )
#include <winsock.h> 
extern "C" {
	struct eris_timeval  {
		long tv_sec;	/* seconds */
		long tv_usec;	/* microseconds */
	};
}
#endif

namespace Time {
#if defined( __WIN32__ )
	
	// We roll our own timeval... may only need to be done for mingw32.
	typedef struct eris_timeval Stamp;

#elif defined( macintosh )
	typedef UnsignedWide Stamp;	// micro-seconds
#else
	// POSIX, BeOS, ....
	typedef struct timeval Stamp;
#endif	

bool operator<(const Stamp &a, const Stamp &b);

Stamp operator+(const Stamp &a, unsigned long msec);
Stamp operator-(const Stamp &a, unsigned long msec);
	
long operator-(const Stamp &a, const Stamp &b);	
	
void Init();	
	
Stamp getCurrentStamp();
	
} // of namespace Time	

#endif
