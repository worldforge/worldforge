// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Combine.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Combine::Combine()
     : Create()
{
    SetAttr("id", string("combine"));
    Object::ListType parent;
    parent.push_back(string("create"));
    SetAttr("parent", parent);
    SetAttr("specification", string("atlas-game"));
}

Combine Combine::Instantiate()
{
    Combine combine;

    Object::ListType parent;
    parent.push_back(string("combine"));
    combine.SetAttr("parent", parent);
    
    return combine;
}

} } } // namespace Atlas::Objects::Operation
