// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Talk.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Talk::Talk()
     : Communicate()
{
    SetAttr("id", string("talk"));
    Object::ListType parent;
    parent.push_back(string("communicate"));
    SetAttr("parent", parent);
}

Talk Talk::Instantiate()
{
    Talk value;

    Object::ListType parent;
    parent.push_back(string("talk"));
    value.SetAttr("parent", parent);
    value.SetAttr("objtype", string("instance"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
