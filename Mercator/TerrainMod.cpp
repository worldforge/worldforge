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

CraterTerrainMod::CraterTerrainMod(const WFMath::Ball<3> &s) :
      m_shape(s),
      m_intersectShape(WFMath::Point<2>(s.center()[0], s.center()[1]),
                       s.radius())
{
    WFMath::AxisBox<3> bb=m_shape.boundingBox();
    m_box = WFMath::AxisBox<2> (
                WFMath::Point<2>(bb.lowerBound(0), bb.lowerBound(1)),
                WFMath::Point<2>(bb.upperBound(0), bb.upperBound(1))
           );
}

CraterTerrainMod::~CraterTerrainMod()
{
}

bool CraterTerrainMod::checkIntersects(const Segment& s) const
{
    return WFMath::Intersect(m_intersectShape, s.getRect(), false) ||
        WFMath::Contains(s.getRect(), m_shape.center(), false);
}
    
void CraterTerrainMod::apply(float &point, int x, int y) const
{
    if (Contains(m_shape,WFMath::Point<3>(x,y,point),true)) {
        float d = m_shape.radius() * m_shape.radius() -
                  (m_shape.getCenter()[0] - x) * (m_shape.getCenter()[0] - x) -
                  (m_shape.getCenter()[1] - y) * (m_shape.getCenter()[1] - y); 

        if (d >= 0.0)
            point = m_shape.getCenter()[2] - sqrt(d);
    }
}
    
TerrainMod * CraterTerrainMod::clone() const
{
    return new CraterTerrainMod(m_shape);
}

void CraterTerrainMod::setShape(const WFMath::Ball<3> & s)
{
    
    WFMath::AxisBox<3> bb=m_shape.boundingBox();
    m_box = WFMath::AxisBox<2> (
                WFMath::Point<2>(bb.lowerBound(0), bb.lowerBound(1)),
                WFMath::Point<2>(bb.upperBound(0), bb.upperBound(1))
           );
    m_intersectShape = WFMath::Ball<2>(WFMath::Point<2>(s.center()[0],
                                                        s.center()[1]),
                                       s.radius());
}

} // namespace Mercator
