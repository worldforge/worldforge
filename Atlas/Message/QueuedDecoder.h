// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_QUEUEDDECODER_H
#define ATLAS_MESSAGE_QUEUEDDECODER_H

#include <queue>
#include "Object.h"
#include "DecoderBase.h"

namespace Atlas { namespace Message {

/** Atlas message queued decoder
 *
 * This message decoder puts arrived objects into a queue and allows the
 * application to pop them off the front of the queue, peek at the front of 
 * the queue, as well as checking to see how many objects are currently in the
 * queue.
 *
 * @see DecoderBase
 * @see Object
 *
 */
    
class QueuedDecoder : public DecoderBase
{
public:

    QueuedDecoder();
    
    size_t QueueSize();
    Object Pop();
    Object Front();
    void Clear();
    
protected:

    void ObjectArrived(const Object& obj);
    
private:
    
    std::queue<Object> objectQueue;
};

} } // namespace Atlas::Message

#endif
