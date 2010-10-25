// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#include <Mercator/TerrainMod_impl.h>

#include <Mercator/Segment.h>

namespace Mercator {

TerrainMod::Context::Context()
{
}

TerrainMod::Context::~Context()
{
}

void TerrainMod::Context::setId(const std::string& id)
{
    m_id = id;
}

TerrainMod::TerrainMod() : m_context(0)
{
}

TerrainMod::~TerrainMod()
{
    delete m_context;
}

int TerrainMod::addToSegment(Segment & s) const
{
    // if (!checkIntersects(s)) {
        // return -1;
    // }
    return s.addMod(this);
}

void TerrainMod::updateToSegment(Segment & s) const
{
    // if (!checkIntersects(s)) {
        // s.removeMod(this);
        // return;
    // }
    // FIXME There is a chance it doesn't have it yet.
    if (s.updateMod(this) != 0) {
        s.addMod(this);
    }
}

void TerrainMod::removeFromSegment(Segment & s) const
{
    s.removeMod(this);
}

void TerrainMod::setContext(TerrainMod::Context * c)
{
    m_context = c;
}

template class LevelTerrainMod<WFMath::Ball<2> >;
template class LevelTerrainMod<WFMath::RotBox<2> >;

CraterTerrainMod::CraterTerrainMod(const WFMath::Ball<3> &s) : m_shape(s)
{
    WFMath::AxisBox<3> bb=m_shape.boundingBox();
    ab = WFMath::AxisBox<2> (
                WFMath::Point<2>(bb.lowerBound(0), bb.lowerBound(1)),
                WFMath::Point<2>(bb.upperBound(0), bb.upperBound(1))
           );
}

CraterTerrainMod::~CraterTerrainMod()
{
}
    
WFMath::AxisBox<2> CraterTerrainMod::bbox() const
{
    return ab;
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

} // namespace Mercator
