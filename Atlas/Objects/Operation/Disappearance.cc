// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Disappearance.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Disappearance::Disappearance()
     : Sight()
{
    SetAttr("id", string("disappearance"));
    Object::ListType parents;
    parents.push_back(string("sight"));
    SetAttr("parents", parents);
}

Disappearance Disappearance::Instantiate()
{
    Disappearance value;

    Object::ListType parents;
    parents.push_back(string("disappearance"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
