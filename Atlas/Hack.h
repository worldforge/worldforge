// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_HACK_H
#define ATLAS_HACK_H

#include "Codec.h"

#include <iostream>

namespace Atlas { namespace UngodlyHack {

Atlas::Codec<iostream>* GetPacked(iostream&, Atlas::Bridge*);
Atlas::Codec<iostream>* GetXML(iostream&, Atlas::Bridge*);

} } // Atlas::UngodlyHack namespace

#endif
