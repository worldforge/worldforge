#include "RootEntity.h"

using namespace std;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Entity {

RootEntity::RootEntity()
{
    loc = "";
    fill_n(pos.begin(), 3, 0.0);
    fill_n(velocity.begin(), 3, 0.0);
}

Object RootEntity::Get(const string& name)
{
    if (name == "loc") return GetLoc();
    if (name == "pos") return GetPos();
    if (name == "velocity") return GetVelocity();
    return Root::Get(name);
}

void Set(const string& name, const Object& obj)
{
    if (name == "loc") {SetLoc(name, obj.As(Object::String)); return; }
    if (name == "pos") {SetPos(name, obj.As(Object::List)); return; }
    if (name == "velocity") {SetVelocity(name, obj.As(Object::List)); return; }
    Root::Set(name, obj);
}

bool RootEntity::Has(const string& name)
{
    if (name == "loc") return HasLoc();
    if (name == "pos") return HasPos();
    if (name == "velocity") return HasVelocity();

    return Root::Has(name);
}

string RootEntity::GetLoc()
{
    return loc;
}

void RootEntity::SetLoc(const string& s)
{
    loc = s;
}

bool RootEntity::HasLoc()
{
    return true;
}

list<Object> RootEntity::GetPos()
{
    return pos;
}

void RootEntity::SetPos(const list<Object>& p)
{
    pos = p;
}

bool RootEntity::HasPos()
{
    return true;
}

list<Object> RootEntity::GetVelocity()
{
    return velocity;
}

void RootEntity::SetVelocity(const list<Object>& v)
{
    velocity = v;
}

bool RootEntity::HasVelocity()
{
    return true;
}

} } } // namespace Atlas::Objects::Entity
