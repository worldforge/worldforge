#ifndef ERIS_LOG_H
#define ERIS_LOG_H

#include <sigc++/signal.h>

#include <string>

namespace Eris
{
    
/** Logging level : setting a higher level will automaticaly pull in the lower
 levels (i.e NOTICE implies ERROR and WARNING) */
typedef enum {
    LOG_ERROR = 0,  ///< serious failure indications
    LOG_WARNING,    ///< something is amiss, but probably okay to continue
    LOG_NOTICE,     ///< general information
    LOG_VERBOSE,    ///< <i>lots</i> of information, about every received operation, for example
    LOG_DEBUG       ///< excessive amounts of stuff
} LogLevel;

/// the default logging level for a new connection
const LogLevel DEFAULT_LOG = LOG_WARNING;    
    
/** Emitted with logging information; client may handle as it see fit.
There is room for considerable expansion of this feature; notably message
classes (warning / info / debug). Any feedback greatly appreciated */
extern sigc::signal<void, LogLevel, const std::string&> Logged;
    
/** set the logging level for all sucessive messages : this can be called at any time, so it is
reasonable to bracket suspect calls in setLogLevel calls if you choose */
void setLogLevel(LogLevel lvl);

LogLevel getLogLevel();

}

#include <Eris/LogStream.h>

#endif
