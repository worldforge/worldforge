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
    SetAttr("id", string("listen"));
    Object::ListType parent;
    parent.push_back(string("perceive"));
    SetAttr("parent", parent);
}

Listen Listen::Instantiate()
{
    Listen listen;

    Object::ListType parent;
    parent.push_back(string("listen"));
    listen.SetAttr("parent", parent);
    
    return listen;
}

} } } // namespace Atlas::Objects::Operation
