// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Sniff.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Sniff::Sniff()
     : Perceive()
{
    SetAttr("id", string("sniff"));
    Object::ListType parent;
    parent.push_back(string("perceive"));
    SetAttr("parent", parent);
}

Sniff Sniff::Instantiate()
{
    Sniff value;

    Object::ListType parent;
    parent.push_back(string("sniff"));
    value.SetAttr("parent", parent);
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
