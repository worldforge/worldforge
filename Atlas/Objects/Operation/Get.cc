// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Get.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Get::Get()
     : Action()
{
    SetAttr("id", string("get"));
    Object::ListType parent;
    parent.push_back(string("action"));
    SetAttr("parent", parent);
}

Get Get::Instantiate()
{
    Get get;

    Object::ListType parent;
    parent.push_back(string("get"));
    get.SetAttr("parent", parent);
    
    return get;
}

} } } // namespace Atlas::Objects::Operation
