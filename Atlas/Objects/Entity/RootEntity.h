// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_ENTITY_ROOTENTITY_H
#define ATLAS_OBJECTS_ENTITY_ROOTENTITY_H

#include "../../Message/Object.h"
#include "../Root.h"

namespace Atlas { namespace Objects { namespace Entity {

class RootEntity : public Atlas::Objects::Root
{
public:
    RootEntity();
    RootEntity(int id, const string& loc,
               const list<Atlas::Message::Object>& pos,
               const list<Atlas::Message::Object>& velocity);
    virtual ~RootEntity();

protected:
};

} } } // namespace Atlas::Objects::Entity

#endif
