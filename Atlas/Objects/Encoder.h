// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_ENCODER_H
#define ATLAS_OBJECTS_ENCODER_H

#include "../EncoderBase.h"

namespace Atlas { namespace Objects {
    
class Encoder : public Atlas::EncoderBase
{
public:
    Encoder(Atlas::Bridge* b) : b(b) { }
    virtual ~Encoder() { }

    virtual void StreamMessage(Atlas::Objects::Root* o) {o->Transmit(b); }
};

} } // namespace Atlas::Objects

#endif
