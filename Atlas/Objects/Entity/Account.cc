// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Account.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Entity { 

Account::Account()
     : AdminEntity()
{
    SetId(string("account"));
    Object::ListType parents;
    parents.push_back(string("admin_entity"));
    SetParents(parents);
    SetPassword(string(""));
}

Account Account::Instantiate()
{
    Account value;

    Object::ListType parents;
    parents.push_back(string("account"));
    value.SetParents(parents);
    value.SetObjtype(string("object"));
    
    return value;
}

} } } // namespace Atlas::Objects::Entity
