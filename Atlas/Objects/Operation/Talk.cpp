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
    SetId(string("talk"));
    Object::ListType parents;
    parents.push_back(string("communicate"));
    SetParents(parents);
}

Talk Talk::Instantiate()
{
    Talk value;

    Object::ListType parents;
    parents.push_back(string("talk"));
    value.SetParents(parents);
    value.SetObjtype(string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
