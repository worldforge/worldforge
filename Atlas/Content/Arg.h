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

class Arg // Container for an attribute argument
{
public:
    Arg(const string& name, const AObject& val);

    Arg(const AObject& val);

    const string& getName() const;
    
    const AObject& getVal() const;

protected:
    string m_name;
    
    AObject m_val;

private:
    Arg();
};

#define AEND NULL

Arg* A(const string& id, const string& value);
Arg* A(const string& id, long value);
Arg* A(const string& id, double value);
Arg* A(const string& id, const AObject& value);
Arg* A(const string& value);
Arg* A(long value);
Arg* A(double value);
Arg* A(const AObject& value);

#endif
