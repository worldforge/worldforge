// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "QueuedDecoder.h"

namespace Atlas { namespace Message {

QueuedDecoder::QueuedDecoder()
{
}

void QueuedDecoder::messageArrived(const Element::MapType& obj)
{
    m_objectQueue.push(obj);
}

} } // namespace Atlas::Message
