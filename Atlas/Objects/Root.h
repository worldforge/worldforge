// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_ROOT_H
#define ATLAS_OBJECTS_ROOT_H

#include <string>
#include "ObjectType.h"

namespace Atlas { namespace Objects {

class Root
{
public:
    Root(const string& id, const string& name);

    virtual ~Root();

    std::string GetId();
    void SetId(const std::string& id);
    void RemoveId();
    bool HasId();
    
    std::string GetName();
    void SetName(const std::string& name);
    void RemoveName();
    bool HasName();
    
    ObjectType GetObjectType();
    void SetObjectType(ObjectType type);
    bool HasObjectType() { return true; }

    void Transmit(Bridge* b);
    
protected:
    
    std::string* id;
    std::string* name;
    
    ObjectType objectType;
};

} } // namespace Atlas::Objects

#endif
