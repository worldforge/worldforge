// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Create.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Create::Create()
     : Action()
{
    SetAttr("id", string("create"));
    Object::ListType parent;
    parent.push_back(string("action"));
    SetAttr("parent", parent);
}

Create Create::Instantiate()
{
    Create value;

    Object::ListType parent;
    parent.push_back(string("create"));
    value.SetAttr("parent", parent);
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
