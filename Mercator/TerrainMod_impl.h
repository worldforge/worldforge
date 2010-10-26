// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes, Alistair Riddoch

#ifndef MERCATOR_TERRAIN_MOD_IMPL_H
#define MERCATOR_TERRAIN_MOD_IMPL_H

#include <Mercator/TerrainMod.h>

#include <Mercator/Segment.h>

namespace Mercator {

template <typename Shape>
ShapeTerrainMod<Shape>::ShapeTerrainMod(const Shape &s) : m_shape(s)
{
    m_box = m_shape.boundingBox();
}


template <typename Shape> ShapeTerrainMod<Shape>::~ShapeTerrainMod()
{
}

template <typename Shape>
bool ShapeTerrainMod<Shape>::checkIntersects(const Segment& s) const
{
    return WFMath::Intersect(m_shape, s.getRect(), false) ||
        WFMath::Contains(s.getRect(), m_shape.getCorner(0), false);
}
    
template <typename Shape> LevelTerrainMod<Shape>::~LevelTerrainMod()
{
}
    
template <typename Shape>
void LevelTerrainMod<Shape>::apply(float &point, int x, int y) const
{
    if (Contains(this->m_shape,WFMath::Point<2>(x,y),true)) {
        point = m_level;
    }
}

template <typename Shape>
TerrainMod * LevelTerrainMod<Shape>::clone() const
{
    return new LevelTerrainMod<Shape>(m_level, this->m_shape);
}

template <typename Shape> AdjustTerrainMod<Shape>::~AdjustTerrainMod()
{
}
    
template <typename Shape>
void AdjustTerrainMod<Shape>::apply(float &point, int x, int y) const
{
    if (Contains(this->m_shape,WFMath::Point<2>(x,y),true)) {
        point += m_dist;
    }
}
    
template <typename Shape>
TerrainMod * AdjustTerrainMod<Shape>::clone() const
{
    return new AdjustTerrainMod<Shape>(m_dist, this->m_shape);
}

template <typename Shape> SlopeTerrainMod<Shape>::~SlopeTerrainMod()
{
}
    
template <typename Shape>
void SlopeTerrainMod<Shape>::apply(float &point, int x, int y) const
{
    if (Contains(this->m_shape,WFMath::Point<2>(x,y),true)) {
        point = m_level + (this->m_shape.getCenter()[0] - x) * m_dx 
                        + (this->m_shape.getCenter()[1] - y) * m_dy;
    }
}
    
template <typename Shape>
TerrainMod * SlopeTerrainMod<Shape>::clone() const
{
    return new SlopeTerrainMod<Shape>(m_level, m_dx, m_dy, this->m_shape);
}
    
} //namespace Mercator

#endif // MERCATOR_TERRAIN_MOD_IMPL_H
