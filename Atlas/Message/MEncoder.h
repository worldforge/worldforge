// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_ENCODER_H
#define ATLAS_MESSAGE_ENCODER_H

#include "../EncoderBase.h"
#include "../Bridge.h"
#include "Object.h"

namespace Atlas { namespace Message {

/** Encoder that transmits Atlas::Message::Object.
 *
 * This encoder can be used to send Atlas::Message::Object objects representing
 * full atlas messages.
 *
 * @see Object
 * @see Atlas::Bridge
 * @see Atlas::EncoderBase
 * @author Stefanus Du Toit <sdt@gmx.net>
 */
class Encoder : public Atlas::EncoderBase
{
public:
    Encoder(Atlas::Bridge*);

    virtual ~Encoder() { }

    /// Send a message (must be a map!) in stream state.
    virtual void streamMessage(const Object& obj);
    /// Send an object as a map item.
    virtual void mapItem(const string&, const Object&);
    /// Send an object as a list item.
    virtual void listItem(const Object&);
};

} } // namespace Atlas::Message

#endif
