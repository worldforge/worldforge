// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Error.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Error::Error()
     : Info()
{
    SetAttr("id", string("error"));
    Object::ListType parents;
    parents.push_back(string("info"));
    SetAttr("parents", parents);
}

Error Error::Instantiate()
{
    Error value;

    Object::ListType parents;
    parents.push_back(string("error"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
