// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Sight.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Sight::Sight()
     : Perception()
{
    SetAttr("id", string("sight"));
    Object::ListType parents;
    parents.push_back(string("perception"));
    SetAttr("parents", parents);
}

Sight Sight::Instantiate()
{
    Sight value;

    Object::ListType parents;
    parents.push_back(string("sight"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
