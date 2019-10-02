#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Log.h"

#include <Atlas/Message/MEncoder.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/PresentationBridge.h>

namespace Eris
{

static LogLevel _logLevel = DEFAULT_LOG;    
sigc::signal<void, LogLevel, const std::string&> Logged;
    
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

std::ostream& operator<<(std::ostream& os, const Atlas::Objects::Root& obj)
{
    std::stringstream s;
	Atlas::PresentationBridge bridge(s);
    Atlas::Objects::ObjectsEncoder debugEncoder(bridge);
    debugEncoder.streamObjectsMessage(obj);
    return os << s.str();
}

std::ostream& operator<<(std::ostream& os, const Atlas::Message::Element& msg)
{
	if (msg.isMap()) {
		std::stringstream s;
		Atlas::PresentationBridge bridge(s);
		Atlas::Message::Encoder debugEncoder(bridge);
		debugEncoder.streamMessageElement(msg.Map());
		return os << s.str();
	}
	return os;
}

} // of namespace
