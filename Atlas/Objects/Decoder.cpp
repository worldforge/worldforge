// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include <string>
#include "Decoder.h"

using namespace Atlas::Message;
using namespace std;

namespace Atlas { namespace Objects {

void Decoder::ObjectArrived(const Object& o)
{
    if (!o.Is(Object::Map)) return;
    if (o.As(Object::Map).find("parent") == o.As(Object::Map).end())
        { UnknownObjectArrived(o); return; }
    string parent = *(m.find("parent"));
    // check parent, then construct appropriate object and call ObjectArrived

}

} } // namespace Atlas::Objects
