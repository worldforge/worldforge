// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_OPERATION_SMELL_H
#define ATLAS_OBJECTS_OPERATION_SMELL_H

#include "Perception.h"


namespace Atlas { namespace Objects { namespace Operation { 

/** Character smells something

Base operator for all kind of perceptions

*/
class Smell : public Perception
{
public:
    Smell();
    virtual ~Smell() { }

    static Smell Instantiate();



protected:


};

} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_SMELL_H
