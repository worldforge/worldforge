// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Divide.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Divide::Divide()
     : Create()
{
    SetAttr("id", string("divide"));
    Object::ListType parent;
    parent.push_back(string("create"));
    SetAttr("parent", parent);
    SetAttr("specification", string("atlas-game"));
}

Divide Divide::Instantiate()
{
    Divide value;

    Object::ListType parent;
    parent.push_back(string("divide"));
    value.SetAttr("parent", parent);
    value.SetAttr("objtype", string("instance"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
