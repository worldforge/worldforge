/*
        Arg.cpp
        ----------------
        begin           : 1999.12.28
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#include "Arg.h"

Arg::Arg()
 : m_type(AINVALID), m_name("")
{
}

Arg::Arg(const string& name, const string& value)
 : m_type(ASTRING), m_name(name), m_valString(value)
{
}

Arg::Arg(const string& name, long value)
 : m_type(AINT), m_name(name), m_valInt(value)
{
}

Arg::Arg(const string& name, double value)
 : m_type(AFLOAT), m_name(name), m_valFloat(value)
{
}

Arg::Arg(const string& name, const AObject& value)
 : m_type(AOBJECT), m_name(name), m_valObject(value)
{
}

Arg::Arg(const string& value)
 : m_type(ASTRING), m_name(""), m_valString(value)
{
}

Arg::Arg(long value)
 : m_type(AINT), m_name(""), m_valInt(value)
{
}

Arg::Arg(double value)
 : m_type(AFLOAT), m_name(""), m_valFloat(value)
{
}

Arg::Arg(const AObject& value)
 : m_type(AOBJECT), m_name(""), m_valObject(value)
{
}

atype_t Arg::getType() const
{
    return m_type;
}

const string& Arg::getName() const 
{
    return m_name;
}
    
const string& Arg::getString() const
{
    return m_valString;
}

long Arg::getInt() const
{
    return m_valInt;
}

double Arg::getFloat() const
{
    return m_valFloat;
}

const AObject& Arg::getObject() const
{
    return m_valObject;
}

Arg* A(const string& id, const string& value)
{
    return new Arg(id, value);
}

Arg* A(const string& id, long value)
{
    return new Arg(id, value);
}

Arg* A(const string& id, double value)
{
    return new Arg(id, value);
}

Arg* A(const string& id, const AObject& value)
{
    return new Arg(id, value);
}

Arg* A(const string& value)
{
    return new Arg(value);
}

Arg* A(long value)
{
    return new Arg(value);
}

Arg* A(double value)
{
    return new Arg(value);
}

Arg* A(const AObject& value)
{
    return new Arg(value);
}

