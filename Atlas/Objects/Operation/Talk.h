// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_OPERATION_TALK_H
#define ATLAS_OBJECTS_OPERATION_TALK_H

#include "Communicate.h"


namespace Atlas { namespace Objects { namespace Operation { 

/** used for talking

need to relook at that 'Magic voice that steps on something crunchy and makes magic voice which...' -IRC log

*/
class Talk : public Communicate
{
public:
    Talk();
    virtual ~Talk() { }

    static Talk Instantiate();

protected:
};

} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_TALK_H
