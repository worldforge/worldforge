#ifndef FORGE_CLOCK_H
#define FORGE_CLOCK_H

namespace Time {
	
#ifdef WIN32

#else
	
#endif

void Init();	
	
long GetTicks();
	
}; // of namespace Time

#endif
