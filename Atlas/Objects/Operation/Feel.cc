// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Feel.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Feel::Feel()
     : Perception()
{
    SetAttr("id", string("feel"));
    Object::ListType parent;
    SetAttr("parent", parent);
}

} } } // namespace Atlas::Objects::Operation
