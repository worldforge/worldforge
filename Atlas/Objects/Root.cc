// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "../Message/Encoder.h"
#include "Root.h"

using namespace Atlas;
using namespace Atlas::Message;
using namespace std;

namespace Atlas { namespace Objects {

Root::Root()
{
    INIT(id, "")
}

Root::~Root()
{
}

void Root::Clear()
{
    INIT(id, "")

    attributes.clear();
}

void Root::Reset()
{
    RESET(id)

    attrmap::iterator I;
    for (I = attributes.begin(); I != attributes.end(); I++) {
        (*I).second.first = false;
    }
}

Object Root::GetAttr(const string& name) const
{
    GETATTR(id)
    
    attrmap::const_iterator I;
    if ((I = attributes.find(name)) != attributes.end()) return
        I->second.second;
    return Object();
}

void Root::SetAttr(const string& name, const Object& object)
{
    SETATTR(id, String)
    
    attributes[name] = make_pair(false, object);
}

bool Root::HasAttr(const string& name) const
{
    HASATTR(id)
    if (attributes.find(name) != attributes.end()) return true;
    return false;
}

void Root::Transmit(Atlas::Bridge* b)
{
    Message::Encoder e(b);
    attrmap::iterator I;
    b->StreamMessage(Bridge::MapBegin);

    TRANSMIT(id)
    for (I = attributes.begin(); I != attributes.end(); I++) {
        if ((*I).second.first == false) {
            e.MapItem((*I).first, (*I).second.second);
            (*I).second.first = true;
        }
    }
    b->MapEnd();
}

IMPL_METHODS(Root, string, id)

} }
