// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "Terrain.h"

namespace Mercator {

Terrain::Terrain(int res) : m_res(res)
{

}

Segment * Terrain::getSegmentSafe(int x, int y)
{
    return 0;
}

Segment * Terrain::getSegmentQuik(int x, int y)
{
    return 0;
}

} // namespace Mercator
