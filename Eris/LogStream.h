#ifndef ERIS_LOGSTREAM_H
#define ERIS_LOGSTREAM_H

#include <sstream>
#include <Eris/Log.h>

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
        doLog(LOG_DEBUG, m_stream.str());
    }
};

class error : public logStreamBase
{
public:
    ~error()
    {
        m_stream << std::flush;
        doLog(LOG_DEBUG, m_stream.str());
    }
};

} // of namespace Eris

#endif
