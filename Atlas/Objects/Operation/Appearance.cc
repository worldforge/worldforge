// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Appearance.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Appearance::Appearance()
     : Sight()
{
    SetAttr("id", string("appearance"));
    Object::ListType parent;
    parent.push_back(string("sight"));
    SetAttr("parent", parent);
}

Appearance Appearance::Instantiate()
{
    Appearance value;

    Object::ListType parent;
    parent.push_back(string("appearance"));
    value.SetAttr("parent", parent);
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
