// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_OPERATION_IMAGINARY_H
#define ATLAS_OBJECTS_OPERATION_IMAGINARY_H

#include "Perception.h"


namespace Atlas { namespace Objects { namespace Operation { 

/** When something is not yet implemented in server, then character can pretend to do something ;-)

Base operator for all kind of perceptions

*/
class Imaginary : public Perception
{
public:
    Imaginary();
    virtual ~Imaginary() { }

    static Imaginary Instantiate();



protected:


};

} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_IMAGINARY_H
