// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_ENTITY_ROOTENTITY_H
#define ATLAS_OBJECTS_ENTITY_ROOTENTITY_H

#include "../Root.h"

namespace Atlas { namespace Objects { namespace Entity {

class RootEntity : public Atlas::Objects::Root
{
public:
    virtual Atlas::Message::Object Get(const std::string& name);
    virtual void Set(const std::string&, const Atlas::Message::Object&);
    virtual bool Has(const std::string&);

    virtual std::string GetLoc();
    virtual void SetLoc(const std::string&);
    virtual bool HasLoc();

    virtual std::list<Atlas::Message::Object> GetPos();
    virtual void SetPos(const list<Atlas::Message::Object>&);
    virtual bool HasPos();

    virtual std::list<Atlas::Message::Object> GetVelocity();
    virtual void SetVelocity(const list<Atlas::Message::Object>&);
    virtual bool HasVelocity();
    
protected:
    std::string loc;
    std::list<Atlas::Message::Object> pos;
    std::list<Atlas::Message::Object> velocity;
};

} } // namespace Atlas::Objects::Entity

#endif
