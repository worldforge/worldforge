#include "../Message/Encoder.h"
#include "Root.h"

using namespace Atlas;
using namespace Atlas::Message;
using namespace std;

namespace Atlas { namespace Objects {

Root::Root()
{
    id = "";
}

Root::~Root()
{
}

void Root::Clear()
{
    attributes.clear();
}

void Root::Reset()
{
    attrmap::iterator I;
    
    for (I = attributes.begin(); I != attributes.end(); I++) {
        (*I).second.first = false;
    }
}

Object Root::Get(const string& name)
{
    if (name == "id") return GetId();
    
    return attributes[name].second;
}

void Root::Set(const string& name, const Object& object)
{
    if (name == "id") { SetId(object.As(Object::String)); return; }
    
    attributes[name] = make_pair(false, object);
}

bool Root::Has(const string& name)
{
    if (name == "id") return HasId();
    if (attributes.find(name) != attributes.end()) return true;
    return false;
}

void Root::Transmit(Atlas::Bridge* b)
{
    Message::Encoder e(b);
    attrmap::iterator I;
    b->StreamMessage(Bridge::MapBegin);

    for (I = attributes.begin(); I != attributes.end(); I++) {
        if ((*I).second.first == false) {
            e.MapItem((*I).first, (*I).second.second);
            (*I).second.first = true;
        }
    }
    b->MapEnd();
}

string Root::GetId()
{
    return id;
}

void Root::SetId(const string& s)
{
    id = s;
}

bool Root::HasId()
{
    return true;
}

} }
