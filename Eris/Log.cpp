#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <algorithm>

#ifdef __WIN32__

// Provide missing / misaligned function names.  May only be for mingw32
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif

#endif // __WIN32__

#include <Eris/Log.h>

namespace Eris
{

static LogLevel _logLevel = DEFAULT_LOG;    
SigC::Signal2<void, LogLevel, const std::string&> Logged;
    
void setLogLevel(LogLevel lvl)
{
    _logLevel = lvl;
}    
    
LogLevel getLogLevel()
{
    return _logLevel;
}

const int MSG_BUFFER_SIZE = 2048;	///< bounds for the static char[] buffers useds in vsnprintf

/** Log the specified printf() style string if the current LogLevel is sufficent. The client is free to attach
any (and several) outputs to the Connection::Log signal; notably to files, standard out, Quake style 'consoles',
etc, etc. Note this is purely informational - the client should never need to watch the log stream.*/
void log(LogLevel lvl, const char *str, ...)
{
    if (_logLevel < lvl) return;
	
	va_list args;
	va_start(args, str);
	
	static char buffer[MSG_BUFFER_SIZE];
	::vsnprintf(buffer, MSG_BUFFER_SIZE, str, args);
	
	Logged.emit(lvl, buffer);
	va_end(args);
}    
    
} // of namespace
