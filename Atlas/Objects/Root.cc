#include <string>
#include "Root.h"

using namespace std;

namespace Atlas { namespace Objects {

Root::Root()
{
}

Root::~Root()
{
    if (id != NULL) delete id;
    if (name != NULL) delete name;
}

string Root::GetId()
{
    if (id != NULL) return *id;
    // what to do here?
}

void Root::SetId(const string& value)
{
    if (id != NULL) *id = value; else {
        id = new string;
        *id = value;
    }
}

void Root::RemoveId()
{
    if (id != NULL) {
        delete id;
        id = NULL;
    }
}

bool Root::HasId()
{
    return (id != NULL);
}

string Root::GetName()
{
    if (name != NULL) return *name;
    // what to do here?
}

void Root::SetName(const string& value)
{
    if (name != NULL) *name = value; else {
        name = new string;
        *name = value;
    }
}

void Root::RemoveName()
{
    if (name != NULL) {
        delete name;
        name = NULL;
    }
}

bool Root::HasName()
{
    return (name != NULL);
}

} } // namespace Atlas::Objects
