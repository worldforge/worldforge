// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Move.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Move::Move()
     : Set()
{
    SetAttr("id", string("move"));
    Object::ListType parents;
    parents.push_back(string("set"));
    SetAttr("parents", parents);
    SetAttr("specification", string("atlas-game"));
}

Move Move::Instantiate()
{
    Move value;

    Object::ListType parents;
    parents.push_back(string("move"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
