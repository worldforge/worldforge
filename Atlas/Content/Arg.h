/*
        Arg.h
        ----------------
        begin           : 1999.12.28
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#ifndef __Arg_h_
#define __Arg_h_

#include <string>
using namespace std;

#include <AtlasObject.h>

enum atype_t
{
    AINT,
    AFLOAT,
    ASTRING,
    AOBJECT,
    AINVALID
};

class Arg // Container for an attribute argument
{
public:
    Arg();
    Arg(const string& name, const string& value);
    Arg(const string& name, long value);
    Arg(const string& name, double value);
    Arg(const string& name, const AObject& value);

    Arg(const string& value);
    Arg(long value);
    Arg(double value);
    Arg(const AObject& value);

    atype_t getType() const;

    const string& getName() const;
    
    const string& getString() const;
    long getInt() const;
    double getFloat() const;
    const AObject& getObject() const;

protected:
    atype_t m_type;

    string m_name;
    
    string m_valString;
    long m_valInt;
    double m_valFloat;
    AObject m_valObject;
};

#define AEND (new Arg())

Arg* A(const string& id, const string& value);
Arg* A(const string& id, long value);
Arg* A(const string& id, double value);
Arg* A(const string& id, const AObject& value);
Arg* A(const string& value);
Arg* A(long value);
Arg* A(double value);
Arg* A(const AObject& value);

#endif
