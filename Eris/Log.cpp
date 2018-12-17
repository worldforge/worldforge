#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Log.h"

#include <cassert>
#include <stdio.h>
#include <algorithm>

#include <Atlas/Codecs/Bach.h>
#include <Atlas/Message/MEncoder.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>

#include <iostream>

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
    Atlas::Codecs::Bach debugCodec(s, s, *(Atlas::Bridge*) nullptr);
    Atlas::Objects::ObjectsEncoder debugEncoder(debugCodec);
    debugEncoder.streamObjectsMessage(obj);
    return os << s.str();
}

std::ostream& operator<<(std::ostream& os, const Atlas::Message::Element& msg)
{
    std::stringstream s;
    Atlas::Codecs::Bach debugCodec(s, s, *(Atlas::Bridge*) nullptr);
    Atlas::Message::Encoder debugEncoder(debugCodec);
    debugEncoder.streamMessageElement(msg.asMap());
    return os << s.str();
}

} // of namespace
