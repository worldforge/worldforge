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
    SetId(string("sniff"));
    Object::ListType parents;
    parents.push_back(string("perceive"));
    SetParents(parents);
}

Sniff Sniff::Instantiate()
{
    Sniff value;

    Object::ListType parents;
    parents.push_back(string("sniff"));
    value.SetParents(parents);
    value.SetObjtype(string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
