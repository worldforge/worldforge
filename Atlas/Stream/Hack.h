// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_HACK_H
#define ATLAS_STREAM_HACK_H

#include "Codec.h"

#include <iostream>

namespace Atlas { namespace UngodlyHack {

Atlas::Stream::Codec<iostream>* GetPacked(iostream&, Atlas::Stream::Bridge*);

} } // Atlas::UngodlyHack

#endif
