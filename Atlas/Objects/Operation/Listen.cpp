// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Listen.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Listen::Listen()
     : Perceive()
{
    SetId(string("listen"));
    Object::ListType parents;
    parents.push_back(string("perceive"));
    SetParents(parents);
}

Listen Listen::Instantiate()
{
    Listen value;

    Object::ListType parents;
    parents.push_back(string("listen"));
    value.SetParents(parents);
    value.SetObjtype(string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
