// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Set.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Set::Set()
     : Action()
{
    SetId(string("set"));
    Object::ListType parents;
    parents.push_back(string("action"));
    SetParents(parents);
}

Set Set::Instantiate()
{
    Set value;

    Object::ListType parents;
    parents.push_back(string("set"));
    value.SetParents(parents);
    value.SetObjtype(string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
