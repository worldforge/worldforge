#ifndef ATLAS_OBJECTS_ROOT_H
#define ATLAS_OBJECTS_ROOT_H

#include "ObjectType.h"

namespace Atlas { namespace Objects {

class Root
{
public:
    Root(const string& id, const string& name);

    string GetId();
    void SetId(const string& id);
    void RemoveId();
    bool HasId();
    
    string GetName();
    void SetName(const string& name);
    void RemoveName();
    bool HasName();
    
    ObjectType GetObjectType();
    void SetObjectType(ObjectType type);
    bool HasObjectType() { return true; }

    void Transmit(Bridge* b);
    
protected:
    
    string id; bool haveId;
    string name; bool haveName;
    
    ObjectType objectType;
}

} } // namespace Atlas::Objects

#endif
