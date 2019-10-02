// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Id$

#ifndef ATLAS_NET_LOOPBACK_H
#define ATLAS_NET_LOOPBACK_H

#include <Atlas/Bridge.h>

namespace Atlas { namespace Net {

void loopback(Bridge*, Bridge*, Bridge*&, Bridge*&);

} } // namespace Atlas::Net

#endif
