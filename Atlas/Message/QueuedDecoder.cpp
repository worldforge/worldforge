// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "QueuedDecoder.h"

using namespace std;

namespace Atlas { namespace Message {

QueuedDecoder::QueuedDecoder()
{
}

size_t QueuedDecoder::QueueSize()
{
    return objectQueue.size();
}

Object QueuedDecoder::Front()
{
    return objectQueue.front();
}

Object QueuedDecoder::Pop()
{
    Object r = objectQueue.front();
    objectQueue.pop();
    return r;
}

void QueuedDecoder::ObjectArrived(const Object& obj)
{
    objectQueue.push(obj);
}

} } // namespace Atlas::Message
