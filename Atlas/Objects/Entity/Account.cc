// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Account.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Entity { 

Account::Account()
     : AdminEntity()
{
    SetId(string("account"));
    Object::ListType parents;
    parents.push_back(string("admin_entity"));
    SetParents(parents);
    SetPassword(string(""));
}

Account Account::Instantiate()
{
    Account value;

    Object::ListType parents;
    parents.push_back(string("account"));
    value.SetParents(parents);
    value.SetObjtype(string("object"));
    
    return value;
}

bool Account::HasAttr(const string& name) const
{
    if (name == "password") return true;
    return AdminEntity::HasAttr(name);
}

Object Account::GetAttr(const string& name) const
    throw (NoSuchAttrException)
{
    if (name == "password") return attr_password;
    return AdminEntity::GetAttr(name);
}

void Account::SetAttr(const string& name, const Object& attr)
{
    if (name == "password") { SetPassword(attr.AsString()); return; }
    AdminEntity::SetAttr(name, attr);
}

void Account::RemoveAttr(const string& name)
{
    if (name == "password") return;
    AdminEntity::RemoveAttr(name);
}

void Account::SendContents(Bridge* b)
{
    SendPassword(b);
    AdminEntity::SendContents(b);
}

Object Account::AsObject() const
{
    Object::MapType m = AdminEntity::AsObject().AsMap();
    m["password"] = Object(attr_password);
    return Object(m);
}

} } } // namespace Atlas::Objects::Entity
