// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_ENTITY_ACCOUNT_H
#define ATLAS_OBJECTS_ENTITY_ACCOUNT_H

#include "AdminEntity.h"


namespace Atlas { namespace Objects { namespace Entity { 

/** Base class for accounts

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/
class Account : public AdminEntity
{
public:
    Account();
    virtual ~Account() { }

    static Account Instantiate();

protected:
};

} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ACCOUNT_H
