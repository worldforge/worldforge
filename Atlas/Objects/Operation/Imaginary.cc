// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Imaginary.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Operation { 

Imaginary::Imaginary()
     : Perception()
{
    SetAttr("id", string("imaginary"));
    Object::ListType parent;
    parent.push_back(string("perception"));
    SetAttr("parent", parent);
}

Imaginary Imaginary::Instantiate()
{
    Imaginary value;

    Object::ListType parent;
    parent.push_back(string("imaginary"));
    value.SetAttr("parent", parent);
    
    return value;
}

} } } // namespace Atlas::Objects::Operation
