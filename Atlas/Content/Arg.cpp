/*
        Arg.cpp
        ----------------
        begin           : 1999.12.28
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#include "Arg.h"

Arg::Arg(const string& name, const AObject& val)
 : m_name(name), m_val(val)
{
}

Arg::Arg(const AObject& val)
 : m_name(""), m_val(val)
{
}

const string& Arg::getName() const 
{
    return m_name;
}
    
const AObject& Arg::getVal() const
{
    return m_val;
}

Arg* A(const string& id, const string& value)
{
    return new Arg(id, AObject(value));
}

Arg* A(const string& id, long value)
{
    return new Arg(id, AObject(value));
}

Arg* A(const string& id, double value)
{
    return new Arg(id, AObject(value));
}

Arg* A(const string& id, const AObject& value)
{
    return new Arg(id, AObject(value));
}

Arg* A(const string& value)
{
    return new Arg(AObject(value));
}

Arg* A(long value)
{
    return new Arg(AObject(value));
}

Arg* A(double value)
{
    return new Arg(AObject(value));
}

Arg* A(const AObject& value)
{
    return new Arg(AObject(value));
}

