// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Info.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Info::Info()
     : RootOperation()
{
    SetId(string("info"));
    Object::ListType parents;
    parents.push_back(string("root_operation"));
    SetParents(parents);
}

Info Info::Instantiate()
{
    Info value;

    Object::ListType parents;
    parents.push_back(string("info"));
    value.SetParents(parents);
    value.SetObjtype(string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
