// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Admin.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Entity { 

Admin::Admin()
     : Account()
{
    SetAttr("id", string("admin"));
    Object::ListType parent;
    parent.push_back(string("account"));
    SetAttr("parent", parent);
}

Admin Admin::Instantiate()
{
    Admin value;

    Object::ListType parent;
    parent.push_back(string("admin"));
    value.SetAttr("parent", parent);
    value.SetAttr("objtype", string("object"));
    
    return value;
}

} } } // namespace Atlas::Objects::Entity
