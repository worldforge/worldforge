// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_ENTITY_ADMIN_H
#define ATLAS_OBJECTS_ENTITY_ADMIN_H

#include "Account.h"

namespace Atlas { namespace Objects { namespace Entity { 

/** Privileged accounts

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/
class Admin : public Account
{
public:
    Admin();
    virtual ~Admin() { }

    static Admin Instantiate();

protected:
};

} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ADMIN_H
