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
    Object::ListType parent;
    parent.push_back(string("set"));
    SetAttr("parent", parent);
    SetAttr("specification", string("atlas-game"));
}

} } } // namespace Atlas::Objects::Operation
