// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_OBJECT_LAYER_H
#define ATLAS_OBJECT_LAYER_H

#include "../Bridge.h"
#include "Object.h"

namespace Atlas { namespace Object {

/** Atlas object layer

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

} } // Atlas::Object namespace

#endif
