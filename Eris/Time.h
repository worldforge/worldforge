#ifndef FORGE_TIME_H
#define FORGE_TIME_H

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

namespace Time {

class Stamp {
 private:
#if defined( __WIN32__ )
	
	// We roll our own timeval... may only need to be done for mingw32.
	// FIXME since we're no longer doing a typedef, do we really need to do this?
	struct eris_timeval _val;

#elif defined( macintosh )
	UnsignedWide _val;	// micro-seconds
#else
	// POSIX, BeOS, ....
	struct timeval _val;
#endif	

	// Hook for future expansion to other platforms,
	// in case the timer needs initializing
	static void Init() {_did_init = true;}
	static bool _did_init;

 public:
	Stamp() {Init();}
	// Don't need to call Init() in copy constructor, it must
	// have been called when the value we're copying from was
	// constructed

	friend bool operator<(const Stamp &a, const Stamp &b);

	friend Stamp operator+(const Stamp &a, unsigned long msec);
	friend Stamp operator-(const Stamp &a, unsigned long msec);
	
	friend long operator-(const Stamp &a, const Stamp &b);	
	
	friend Stamp getCurrentStamp();
};
	
inline Stamp operator+(unsigned long msec, const Stamp &a) {return a + msec;}

} // of namespace Time

#endif
