// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Action.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Action::Action()
     : RootOperation()
{
    SetAttr("id", string("action"));
    Object::ListType parents;
    parents.push_back(string("root_operation"));
    SetAttr("parents", parents);
}

Action Action::Instantiate()
{
    Action value;

    Object::ListType parents;
    parents.push_back(string("action"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
