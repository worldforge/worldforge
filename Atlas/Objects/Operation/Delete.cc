// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Delete.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Delete::Delete()
     : Action()
{
    SetAttr("id", string("delete"));
    Object::ListType parent;
    parent.push_back(string("action"));
    SetAttr("parent", parent);
}

} } } // namespace Atlas::Objects::Operation
