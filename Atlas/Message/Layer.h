// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Stefanus Du Toit

#ifndef ATLAS_MESSAGE_LAYER_H
#define ATLAS_MESSAGE_LAYER_H

#include "Encoder.h"
#include "DecoderBase.h"

namespace Atlas { namespace Message {

/** Atlas message layer

FIXME document this

@see DecoderBase
@see Encoder
@see Object

*/

class Layer : public Encoder, public DecoderBase
{
public:
    Layer(Atlas::Bridge*);
    virtual ~Layer() { }
};

} } // Atlas::Message namespace

#endif
