#ifndef _CONFIG_H
#define _CONFIG_H

#undef _XOPEN_SOURCE
#undef _ISOC99_SOURCE

#ifdef __WIN32__
// icky hack.
#ifndef snprintf
	#define snprintf _snprintf
#endif
#endif

#endif

