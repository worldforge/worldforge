/*
        Arg.cpp
        ----------------
        begin           : 1999.12.28
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#include "Arg.h"

using namespace Atlas;

Atlas::Arg::Arg(const string& name, const Object& val)
 : m_name(name), m_val(val)
{
}

Atlas::Arg::Arg(const Object& val)
 : m_name(""), m_val(val)
{
}

const string& Atlas::Arg::getName() const 
{
    return m_name;
}
    
const Object& Atlas::Arg::getVal() const
{
    return m_val;
}

Atlas::Arg* Atlas::A(const string& id, const string& value)
{
    return new Arg(id, Object(value));
}

Atlas::Arg* Atlas::A(const string& id, long value)
{
    return new Arg(id, Object(value));
}

Atlas::Arg* Atlas::A(const string& id, double value)
{
    return new Arg(id, Object(value));
}

Atlas::Arg* Atlas::A(const string& id, const Object& value)
{
    return new Arg(id, Object(value));
}

Atlas::Arg* Atlas::A(const string& value)
{
    return new Arg(Object(value));
}

Atlas::Arg* Atlas::A(long value)
{
    return new Arg(Object(value));
}

Atlas::Arg* Atlas::A(double value)
{
    return new Arg(Object(value));
}

Atlas::Arg* Atlas::A(const Object& value)
{
    return new Arg(Object(value));
}

