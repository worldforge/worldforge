#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Log.h>

#include <cassert>
#include <stdio.h>
#include <algorithm>


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

void doLog(LogLevel lvl, const std::string& msg)
{
    if (lvl <= _logLevel)
        Logged.emit(lvl, msg);
}

} // of namespace
