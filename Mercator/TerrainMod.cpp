// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#include <Mercator/TerrainMod_impl.h>

#include <Mercator/Segment.h>

namespace Mercator {

TerrainMod::TerrainMod()
{
}

TerrainMod::~TerrainMod()
{
}

int TerrainMod::addToSegment(Segment & s) const
{
    if (!checkIntersects(s)) {
        return -1;
    }
    return s.addMod(this);
}

void TerrainMod::updateToSegment(Segment & s) const
{
    if (!checkIntersects(s)) {
        s.removeMod(this);
        return;
    }
    if (s.updateMod(this) != 0) {
        s.addMod(this);
    }
}

void TerrainMod::removeFromSegment(Segment & s) const
{
    s.removeMod(this);
}

template class LevelTerrainMod<WFMath::Ball >;
template class LevelTerrainMod<WFMath::Polygon >;
template class LevelTerrainMod<WFMath::RotBox >;

template class AdjustTerrainMod<WFMath::Ball >;
template class AdjustTerrainMod<WFMath::Polygon >;
template class AdjustTerrainMod<WFMath::RotBox >;

template class SlopeTerrainMod<WFMath::Ball >;
template class SlopeTerrainMod<WFMath::Polygon >;
template class SlopeTerrainMod<WFMath::RotBox >;

template class CraterTerrainMod<WFMath::Ball >;
template class CraterTerrainMod<WFMath::Polygon >;
template class CraterTerrainMod<WFMath::RotBox >;

} // namespace Mercator
