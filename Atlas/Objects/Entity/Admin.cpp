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
    SetId(string("admin"));
    Object::ListType parents;
    parents.push_back(string("account"));
    SetParents(parents);
}

Admin Admin::Instantiate()
{
    Admin value;

    Object::ListType parents;
    parents.push_back(string("admin"));
    value.SetParents(parents);
    value.SetObjtype(string("object"));
    
    return value;
}

} } } // namespace Atlas::Objects::Entity
