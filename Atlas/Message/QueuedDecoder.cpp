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

Element QueuedDecoder::front()
{
    return m_objectQueue.front();
}

Element QueuedDecoder::pop()
{
    Element r = m_objectQueue.front();
    m_objectQueue.pop();
    return r;
}

void QueuedDecoder::objectArrived(const Element& obj)
{
    m_objectQueue.push(obj);
}

} } // namespace Atlas::Message
