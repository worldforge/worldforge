// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "Root.h"

using namespace Atlas;
using namespace Atlas::Message;
using namespace std;

namespace Atlas { namespace Objects {

Root::Root()
{
    SetAttr("id", 0);
}

Root::~Root()
{
}

bool Root::HasAttr(const string& name) const
{
    return (attributes.find(name) == attributes.end());
}

const Object& Root::GetAttr(const string& name) const
{
    return (*attributes.find(name))
}

void Root::SetAttr(const string& name, const Atlas::Message::Object& attr)
{
    attributes[name] = attr;
}

void Root::RemoveAttr(const string& name)
{
    attributes.erase(name);
}

Object Root::AsObject() const
{
    return Object(attributes);
}

void Root::SendContents(Bridge* b) const
{
    Message::Encoder e(b);
    typedef map<string, Object>::const_iterator Iter;
    for (Iter I = attributes.begin(); I != attributes.end(); I++) e.MapItem(*I);
}

} } // namespace Atlas::Objects
