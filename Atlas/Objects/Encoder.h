// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_ENCODER_H
#define ATLAS_OBJECTS_ENCODER_H

#include "../EncoderBase.h"

namespace Atlas { namespace Objects {

/** Objects hierarchy encoder
 *
 * This Encoder can be used to send objects in the Atlas::Objects hierarchy to
 * a certain Bridge (e.g. a codec).
 *
 * Simply call the streamMessage member with a pointer to the object to be
 * sent.
 *
 * @see Atlas::Objects::Decoder
 * @author Stefanus Du Toit <sdt@gmx.net>
 */
class Encoder : public Atlas::EncoderBase
{
public:
    /// The default constructor.
    /// @param b The Bridge to which objects are to be sent.
    Encoder(Atlas::Bridge* b) : EncoderBase(b) { }
    /// The default destructor.
    virtual ~Encoder() { }

    /// Send an object to the bridge.
    /// @param o The object that is to be sent.
    virtual void streamMessage(const Atlas::Objects::Root& o)
    {
        b->streamMessage(Bridge::mapBegin);
        o->sendContents(b);
        b->mapEnd();
    }
};

} } // namespace Atlas::Objects

#endif
