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

#include "../Object/Object.h"

using namespace Atlas;

class Arg // Container for an attribute argument
{
public:
    Arg(const string& name, const Object& val);

    Arg(const Object& val);

    const string& getName() const;
    
    const Object& getVal() const;

protected:
    string m_name;
    
    Object m_val;

private:
    Arg();
};

#define AEND NULL

Arg* A(const string& id, const string& value);
Arg* A(const string& id, long value);
Arg* A(const string& id, double value);
Arg* A(const string& id, const Object& value);
Arg* A(const string& value);
Arg* A(long value);
Arg* A(double value);
Arg* A(const Object& value);

#endif
