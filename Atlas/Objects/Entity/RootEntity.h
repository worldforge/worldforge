// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_ENTITY_ROOTENTITY_H
#define ATLAS_OBJECTS_ENTITY_ROOTENTITY_H

#include "../Root.h"
#include "../Macros.h"

namespace Atlas { namespace Objects { namespace Entity {

class RootEntity : public Atlas::Objects::Root
{
public:
    RootEntity();
    
    virtual Atlas::Message::Object GetAttr(const std::string& name) const;
    virtual void SetAttr(const std::string&, const Atlas::Message::Object&);
    virtual bool HasAttr(const std::string&) const;

    METHODS(std::string, loc)

    METHODS(Atlas::Message::Object::ListType, pos)
        
    METHODS(Atlas::Message::Object::ListType, velocity)
    
protected:
    ATTRIBUTE(std::string, loc)
    ATTRIBUTE(Atlas::Message::Object::ListType, pos)
    ATTRIBUTE(Atlas::Message::Object::ListType, velocity)
};

} } } // namespace Atlas::Objects::Entity

#endif
