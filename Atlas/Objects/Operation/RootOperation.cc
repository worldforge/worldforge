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
    SetAttr("id", string("root_operation"));
    Object::ListType parent;
    parent.push_back(string("root"));
    SetAttr("parent", parent);
    SetAttr("objtype", string("op_definition"));
    SetAttr("serialno", 0);
    SetAttr("refno", 0);
    SetAttr("from", string(""));
    SetAttr("to", string(""));
    SetAttr("seconds", 0.0);
    SetAttr("future_seconds", 0.0);
    SetAttr("time_string", string(""));
    Object::ListType args;
    SetAttr("args", args);
}

RootOperation RootOperation::Instantiate()
{
    RootOperation value;

    Object::ListType parent;
    parent.push_back(string("root_operation"));
    value.SetAttr("parent", parent);
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
