/*
        Arg.cpp
        ----------------
        begin           : 1999.12.28
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#include "Arg.h"

Arg::Arg(const string& name, const Object& val)
 : m_name(name), m_val(val)
{
}

Arg::Arg(const Object& val)
 : m_name(""), m_val(val)
{
}

const string& Arg::getName() const 
{
    return m_name;
}
    
const Object& Arg::getVal() const
{
    return m_val;
}

Arg* A(const string& id, const string& value)
{
    return new Arg(id, Object(value));
}

Arg* A(const string& id, long value)
{
    return new Arg(id, Object(value));
}

Arg* A(const string& id, double value)
{
    return new Arg(id, Object(value));
}

Arg* A(const string& id, const Object& value)
{
    return new Arg(id, Object(value));
}

Arg* A(const string& value)
{
    return new Arg(Object(value));
}

Arg* A(long value)
{
    return new Arg(Object(value));
}

Arg* A(double value)
{
    return new Arg(Object(value));
}

Arg* A(const Object& value)
{
    return new Arg(Object(value));
}

