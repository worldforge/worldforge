// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_OPERATION_PERCEPTION_H
#define ATLAS_OBJECTS_OPERATION_PERCEPTION_H

#include "Info.h"


namespace Atlas { namespace Objects { namespace Operation { 

/** Character perceives something.

Base operator for all kind of perceptions

*/
class Perception : public Info
{
public:
    Perception();
    virtual ~Perception() { }

    static Perception Instantiate();

protected:
};

} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_PERCEPTION_H
