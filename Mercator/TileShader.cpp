// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include <Mercator/TileShader.h>

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>

namespace Mercator {

TileShader::TileShader()
{
}

TileShader::~TileShader()
{
}

bool TileShader::checkIntersect(const Segment & s) const
{
    return true;
}

void TileShader::shade(Surface & s) const
{
    TileShader::Shaderstore::const_iterator I = m_subShaders.begin();
    TileShader::Shaderstore::const_iterator Iend = m_subShaders.end();
    for (; I != Iend; ++I) {
        Surface * sf = I->second->newSurface(s.getSegment());
        // shade it
        // for each point, if its opaquish, stick its tile down
        // need a mapping of subShader index to tile symbol
    }
}

} // namespace Mercator
