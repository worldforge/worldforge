// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_OPERATION_COMMUNICATE_H
#define ATLAS_OBJECTS_OPERATION_COMMUNICATE_H

#include "Create.h"


namespace Atlas { namespace Objects { namespace Operation { 

/** Base operator for all kind of communication.

Map editor is main user for this in client side. Server uses this inside "info" operation to tell client about new things.

*/
class Communicate : public Create
{
public:
                /// Construct a Communicate class definition.
    Communicate();
                /// Default destructor.
    virtual ~Communicate() { }

                /// Create a new instance of Communicate.
    static Communicate Instantiate();

protected:

};

} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_COMMUNICATE_H
