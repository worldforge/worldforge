// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Error.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Error::Error()
     : Info()
{
    SetId(string("error"));
    Object::ListType parents;
    parents.push_back(string("info"));
    SetParents(parents);
}

Error Error::Instantiate()
{
    Error value;

    Object::ListType parents;
    parents.push_back(string("error"));
    value.SetParents(parents);
    value.SetObjtype(string("op"));
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
