// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_MESSAGE_LAYER_H
#define ATLAS_MESSAGE_LAYER_H

#include "../Bridge.h"
#include "Object.h"

namespace Atlas { namespace Message {

/** Atlas message layer

FIXME document this

@see Object
@see Bridge
*/

class Layer : public Atlas::Bridge
{
    public:

    Layer(Atlas::Bridge*);

    private:

    Atlas::Bridge* bridge;
};

} } // Atlas::Message namespace

#endif
