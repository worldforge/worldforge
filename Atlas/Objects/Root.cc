#include "Root.h"

using namespace Atlas;
using namespace Atlas::Message;
using namespace std;

namespace Atlas { namespace Objects {

Root::Root()
{
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
        (*I).value.first = false;
    }
}

MorphObject Root::Get(const string& name)
{
    return attributes[name].second;
}

void Root::Set(const string& name, const MorphObject& object)
{
    attributes[name] = make_pair(false, object);
}

bool Root::Has(const string& name)
{
    if (attributes.find(name) != attributes.end()) return true;
    return false;
}

void Root::Transmit(Atlas::Bridge* b)
{
    Message::Encoder e(b);
    attrmap::iterator I;
    b->StreamMessage(Bridge::MapBegin);
    
    for (I = attributes.begin(); I != attributes.end(); I++) {
        if ((*I).value.first == false) {
            e.MapItem((*I).key, (*I).value.second);
            (*I).value.first = true;
        }
    }
    b->MapEnd();
}

} }
