// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_OPERATION_COMBINE_H
#define ATLAS_OBJECTS_OPERATION_COMBINE_H

#include "Create.h"


namespace Atlas { namespace Objects { namespace Operation { 

/** Combine existing objects into new objects.

This is how normal characters create objects.

*/
class Combine : public Create
{
public:
    /// Construct a Combine class definition.
    Combine();
    /// Default destructor.
    virtual ~Combine() { }

    /// Create a new instance of Combine.
    static Combine Instantiate();

protected:

};

} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_COMBINE_H
