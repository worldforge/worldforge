// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Damien McGinnes, Alistair Riddoch

#include "BasePoint.h"
#include "iround.h"

namespace Mercator { 

constexpr float BasePoint::HEIGHT;
constexpr float BasePoint::ROUGHNESS;
constexpr float BasePoint::FALLOFF;

unsigned int BasePoint::seed() const
{
    return I_ROUND(m_height * 1000.0);
}

} //namespace Mercator
