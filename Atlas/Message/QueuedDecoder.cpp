// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "QueuedDecoder.h"

namespace Atlas { namespace Message {

QueuedDecoder::QueuedDecoder()
{
}

size_t QueuedDecoder::queueSize()
{
    return m_objectQueue.size();
}

Element::MapType QueuedDecoder::front()
{
    return m_objectQueue.front();
}

Element::MapType QueuedDecoder::pop()
{
    Element::MapType r = m_objectQueue.front();
    m_objectQueue.pop();
    return r;
}

void QueuedDecoder::objectArrived(const Element::MapType& obj)
{
    m_objectQueue.push(obj);
}

} } // namespace Atlas::Message
