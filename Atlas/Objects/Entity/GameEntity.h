// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_ENTITY_GAMEENTITY_H
#define ATLAS_OBJECTS_ENTITY_GAMEENTITY_H

#include "RootEntity.h"


namespace Atlas { namespace Objects { namespace Entity { 

/** All In Game classes and objects

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/
class GameEntity : public RootEntity
{
public:
    /// Construct a GameEntity class definition.
    GameEntity();
    /// Default destructor.
    virtual ~GameEntity() { }

    /// Create a new instance of GameEntity.
    static GameEntity Instantiate();

protected:

};

} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_GAMEENTITY_H
