// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "RootOperation.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

RootOperation::RootOperation()
     : Root()
{
    SetId(string("root_operation"));
    Object::ListType parents;
    parents.push_back(string("root"));
    SetParents(parents);
    SetObjtype(string("op_definition"));
    SetSerialno(0);
    SetRefno(0);
    SetFrom(string(""));
    SetTo(string(""));
    SetSeconds(0.0);
    SetFutureSeconds(0.0);
    SetTimeString(string(""));
    Object::ListType args;
    SetArgs(args);
}

RootOperation RootOperation::Instantiate()
{
    RootOperation value;

    Object::ListType parents;
    parents.push_back(string("root_operation"));
    value.SetParents(parents);
    value.SetObjtype(string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
