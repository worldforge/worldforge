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
    Object::ListType parents;
    parents.push_back(string("create"));
    SetAttr("parents", parents);
    SetAttr("specification", string("atlas-game"));
}

Divide Divide::Instantiate()
{
    Divide value;

    Object::ListType parents;
    parents.push_back(string("divide"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
