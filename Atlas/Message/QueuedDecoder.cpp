// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "QueuedDecoder.h"

using namespace std;

namespace Atlas {

QueuedDecoder::QueuedDecoder()
{
}

size_t QueuedDecoder::queueSize()
{
    return objectQueue.size();
}

Object QueuedDecoder::front()
{
    return objectQueue.front();
}

Object QueuedDecoder::pop()
{
    Object r = objectQueue.front();
    objectQueue.pop();
    return r;
}

void QueuedDecoder::objectArrived(const Object& obj)
{
    objectQueue.push(obj);
}

} // namespace Atlas
