#ifndef ERIS_LOGSTREAM_H
#define ERIS_LOGSTREAM_H

#include "Log.h"

#include <Atlas/Objects/ObjectsFwd.h>

#include <sstream>

namespace Atlas {
    namespace Message {
        class Element;
    }
}

namespace Eris
{

void doLog(LogLevel lvl, const std::string& msg);

class logStreamBase
{
public:
    std::ostream& operator<<(const std::string& s)
    {
        return m_stream << s;
    }


protected:

    std::ostringstream m_stream;
};

class notice : public logStreamBase
{
public:
    ~notice()
    {
        m_stream << std::flush;
        doLog(LOG_NOTICE, m_stream.str());
    }
};

class debug : public logStreamBase
{
public:
    ~debug()
    {
        m_stream << std::flush;
        doLog(LOG_DEBUG, m_stream.str());
    }
};

class warning : public logStreamBase
{
public:
    ~warning()
    {
        m_stream << std::flush;
        doLog(LOG_WARNING, m_stream.str());
    }
};

class error : public logStreamBase
{
public:
    ~error()
    {
        m_stream << std::flush;
        doLog(LOG_ERROR, m_stream.str());
    }
};

std::ostream& operator<<(std::ostream& s, const Atlas::Objects::Root& obj);
std::ostream& operator<<(std::ostream& s, const Atlas::Message::Element& msg);

} // of namespace Eris

#endif
