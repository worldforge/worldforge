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

namespace Atlas
{

/// Container for an attribute argument
class Arg 
{
public:
    /// construct a named attribute
    Arg(const string& name, const Object& val);
    /// construct an anonymous attribute
    Arg(const Object& val);
    /// get the name of this attribute
    const string& getName() const;
    /// get the value of this attribute
    const Object& getVal() const;

protected:
    /// attribute name
    string m_name;
    /// attribute value
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

} // end namespace Atlas

#endif
