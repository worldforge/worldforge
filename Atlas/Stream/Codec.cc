// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "Codec.h"

using namespace Atlas::Net;
using namespace Atlas::Stream;

Atlas::Stream::Codec::Codec(Socket *socket, Filter *filter)
 : socket(socket), filter(filter)
{
}

Atlas::Stream::Codec::~Codec()
{
    // should it delete filter, socket, or both?
    // garbage collection policy needs thought
    // reference counting could be the lazy way out
}
