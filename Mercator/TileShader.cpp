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

void TileShader::shade(Surface & surface) const
{
    ColorT * sdata = surface.getData();
    int sdata_len = surface.getSize() * surface.getSize();

    TileShader::Shaderstore::const_iterator I = m_subShaders.begin();
    TileShader::Shaderstore::const_iterator Iend = m_subShaders.end();
    for (; I != Iend; ++I) {
        Surface * subs = I->second->newSurface(surface.getSegment());
        I->second->shade(*subs);
        ColorT * subsdata = subs->getData();
        int channels = subs->getChannels();
        
        for (int i = 0; i < sdata_len; ++i) {
            if (subsdata[i * channels + channels - 1] > 127) {
                sdata[i] = I->first;
            }
        }
        delete subs;
    }
}

} // namespace Mercator
