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

bool RootOperation::HasAttr(const string& name) const
{
    if (name == "serialno") return true;
    if (name == "refno") return true;
    if (name == "from") return true;
    if (name == "to") return true;
    if (name == "seconds") return true;
    if (name == "future_seconds") return true;
    if (name == "time_string") return true;
    if (name == "args") return true;
    return Root::HasAttr(name);
}

Object RootOperation::GetAttr(const string& name) const
    throw (NoSuchAttrException)
{
    if (name == "serialno") return attr_serialno;
    if (name == "refno") return attr_refno;
    if (name == "from") return attr_from;
    if (name == "to") return attr_to;
    if (name == "seconds") return attr_seconds;
    if (name == "future_seconds") return attr_future_seconds;
    if (name == "time_string") return attr_time_string;
    if (name == "args") return attr_args;
    return Root::GetAttr(name);
}

void RootOperation::SetAttr(const string& name, const Object& attr)
{
    if (name == "serialno") { SetSerialno(attr.AsInt()); return; }
    if (name == "refno") { SetRefno(attr.AsInt()); return; }
    if (name == "from") { SetFrom(attr.AsString()); return; }
    if (name == "to") { SetTo(attr.AsString()); return; }
    if (name == "seconds") { SetSeconds(attr.AsFloat()); return; }
    if (name == "future_seconds") { SetFutureSeconds(attr.AsFloat()); return; }
    if (name == "time_string") { SetTimeString(attr.AsString()); return; }
    if (name == "args") { SetArgs(attr.AsList()); return; }
    Root::SetAttr(name, attr);
}

void RootOperation::RemoveAttr(const string& name)
{
    if (name == "serialno") return;
    if (name == "refno") return;
    if (name == "from") return;
    if (name == "to") return;
    if (name == "seconds") return;
    if (name == "future_seconds") return;
    if (name == "time_string") return;
    if (name == "args") return;
    Root::RemoveAttr(name);
}

void RootOperation::SendContents(Bridge* b)
{
    SendSerialno(b);
    SendRefno(b);
    SendFrom(b);
    SendTo(b);
    SendSeconds(b);
    SendFutureSeconds(b);
    SendTimeString(b);
    SendArgs(b);
    Root::SendContents(b);
}

Object RootOperation::AsObject() const
{
    Object::MapType m = Root::AsObject().AsMap();
    m["serialno"] = Object(attr_serialno);
    m["refno"] = Object(attr_refno);
    m["from"] = Object(attr_from);
    m["to"] = Object(attr_to);
    m["seconds"] = Object(attr_seconds);
    m["future_seconds"] = Object(attr_future_seconds);
    m["time_string"] = Object(attr_time_string);
    m["args"] = Object(attr_args);
    return Object(m);
}

} } } // namespace Atlas::Objects::Operation
