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
    SetId(string("action"));
    Object::ListType parents;
    parents.push_back(string("root_operation"));
    SetParents(parents);
}

Action Action::Instantiate()
{
    Action value;

    Object::ListType parents;
    parents.push_back(string("action"));
    value.SetParents(parents);
    value.SetObjtype(string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
