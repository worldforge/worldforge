// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Look.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Look::Look()
     : Perceive()
{
    SetAttr("id", string("look"));
    Object::ListType parent;
    parent.push_back(string("perceive"));
    SetAttr("parent", parent);
}

Look Look::Instantiate()
{
    Look value;

    Object::ListType parent;
    parent.push_back(string("look"));
    value.SetAttr("parent", parent);
    value.SetAttr("objtype", string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
