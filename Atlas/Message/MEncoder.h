// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_ENCODER_H
#define ATLAS_MESSAGE_ENCODER_H

#include "../Bridge.h"
#include "Object.h"

namespace Atlas { namespace Message {

/** Atlas message encoder
 *
 * FIXME document this
 *
 * @see Object
 * @see Bridge
 */

class Encoder
{
public:

    Encoder(Atlas::Bridge*);

    virtual ~Encoder() { }

    virtual void SendMessage(const Object& obj);

protected:

    Atlas::Bridge* bridge;
    
};

} } // namespace Atlas::Message

#endif
