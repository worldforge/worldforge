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
    Object::ListType parent;
    SetAttr("parents", parent);
    SetAttr("id", string("root"));
}

Root::Root(const string& id)
{
    Object::ListType parent;
    parent.push_back(string("root"));
    SetAttr("parents", parent);
    SetAttr("id", id);
    SetAttr("objtype", string("class"));
}

Root::~Root()
{
}

Root Root::Instantiate()
{
    Root root("");
    return root;
}

bool Root::HasAttr(const string& name) const
{
    return (attributes.find(name) == attributes.end());
}

const Object& Root::GetAttr(const string& name) const
    throw (NoSuchAttrException) 
{
    if (attributes.find(name) == attributes.end())
        throw NoSuchAttrException(name);
    return ((*attributes.find(name)).second);
}

void Root::SetAttr(const string& name, const Object& attr)
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
    for (Iter I = attributes.begin(); I != attributes.end(); I++)
        e.MapItem((*I).first, (*I).second);
}

} } // namespace Atlas::Objects
