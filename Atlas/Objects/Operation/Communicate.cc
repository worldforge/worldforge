// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Communicate.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Communicate::Communicate()
     : Create()
{
    SetId(string("communicate"));
    Object::ListType parents;
    parents.push_back(string("create"));
    SetParents(parents);
}

Communicate Communicate::Instantiate()
{
    Communicate value;

    Object::ListType parents;
    parents.push_back(string("communicate"));
    value.SetParents(parents);
    value.SetObjtype(string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
