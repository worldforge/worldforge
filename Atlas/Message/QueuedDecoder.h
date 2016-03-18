// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

// $Id$

#ifndef ATLAS_MESSAGE_QUEUEDDECODER_H
#define ATLAS_MESSAGE_QUEUEDDECODER_H

#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Message/Element.h>

#include <queue>

namespace Atlas { namespace Message {

class Element;

typedef std::map<std::string, Element> MapType;

/** Decoder that presents a queue of Atlas::Message::Element.
 *
 * This message decoder puts arrived objects into a queue and allows the
 * application to pop them off the front of the queue, peek at the front of 
 * the queue, as well as checking to see how many objects are currently in the
 * queue.
 *
 * @see DecoderBase
 * @see Element
 * @author Stefanus Du Toit <sdt@gmx.net>
 *
 */
    
class QueuedDecoder : public DecoderBase
{
public:

    QueuedDecoder();

    /// Retrieve the current size of the message queue.    
    size_t queueSize() {
        return m_objectQueue.size();
    }
    /// Pop an object from the front of the message queue.
    MapType popMessage() {
        MapType r = std::move(m_objectQueue.front());
        m_objectQueue.pop();
        return std::move(r);
    }
    /// Peek at the object at the front of the queue.
    const MapType& frontMessage() {
        return m_objectQueue.front();
    }
    /// Clear the message queue.
    void clearQueue();
    
protected:

    /// This adds a message to the queue.
    void messageArrived(MapType obj);
    
private:
    
    std::queue<MapType> m_objectQueue;
};

} } // namespace Atlas::Message

#endif
