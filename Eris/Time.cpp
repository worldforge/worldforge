#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Time.h"

namespace Time
{

#ifdef WIN32
	
#else	

	#include <unistd.h>	
	#include <sys/time.h>
	
	static struct timeval _startTime;
#endif
	
void Init()
{
	gettimeofday(&_startTime, NULL);
}
	
long GetTicks()
{
	struct timeval n;
	gettimeofday(&n, NULL);
	
	return ((n.tv_sec - _startTime.tv_sec) * 1000) 
		+ ((n.tv_usec - _startTime.tv_usec) / 1000);
}

}
