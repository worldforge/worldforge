// This file is distributed under the GNU Lesser General Public license.
// Copyright (C) 2000 Michael Day (See the file COPYING for details).

#ifndef ATLAS_DEBUG_H
#define ATLAS_DEBUG_H

#include <string>
#include <cstdio>
#include <cstdarg>

namespace Atlas
{
    namespace Debug
    {	
	void openLog(const std::string& filename);
	void closeLog();

	void setLevel(int level);
	int getLevel();

	void logmsg(const char* buf);

#ifdef _ADebug_
	inline void Msg(int level, const char* fmt, ...)
	{
	    if (level <= getLevel())
	    {
		char buf[1024];
		va_list va;
		
		va_start(va,fmt);
		vsnprintf(buf, sizeof(buf), fmt, va);
		va_end(va);

		logmsg(buf);
	    }
	}
#else
	inline void Msg(int, const char*, ...) { }
#endif
    }
}

#endif
