// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_OPERATION_DISAPPEARANCE_H
#define ATLAS_OBJECTS_OPERATION_DISAPPEARANCE_H

#include "Sight.h"


namespace Atlas { namespace Objects { namespace Operation { 

/** Character sees something disappearing: it literally disappears or has it gone too far to be visible

Base operator for all kind of perceptions

*/
class Disappearance : public Sight
{
public:
    /// Construct a Disappearance class definition.
    Disappearance();
    /// Default destructor.
    virtual ~Disappearance() { }

    /// Create a new instance of Disappearance.
    static Disappearance Instantiate();

protected:

};

} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_DISAPPEARANCE_H
