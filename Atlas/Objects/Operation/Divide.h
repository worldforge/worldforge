// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_OPERATION_DIVIDE_H
#define ATLAS_OBJECTS_OPERATION_DIVIDE_H

#include "Create.h"


namespace Atlas { namespace Objects { namespace Operation { 

/** Divide existing object into pieces.

One of the pieces might be original object modified.

*/
class Divide : public Create
{
public:
    Divide();
    virtual ~Divide() { }

    static Divide Instantiate();

protected:

};

} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_DIVIDE_H
