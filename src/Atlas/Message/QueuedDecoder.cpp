// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

// $Id$

#include <Atlas/Message/QueuedDecoder.h>

namespace Atlas { namespace Message {

void QueuedDecoder::messageArrived(MapType obj)
{
    m_objectQueue.push(std::move(obj));
}

} } // namespace Atlas::Message
