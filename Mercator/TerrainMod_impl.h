// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes, Alistair Riddoch

#ifndef MERCATOR_TERRAIN_MOD_IMPL_H
#define MERCATOR_TERRAIN_MOD_IMPL_H

#include <Mercator/TerrainMod.h>

#include <Mercator/Segment.h>

namespace Mercator {

template <template <int> class Shape>
ShapeTerrainMod<Shape>::ShapeTerrainMod(const Shape<2> &s) : m_shape(s)
{
    m_box = m_shape.boundingBox();
}


template <template <int> class Shape> ShapeTerrainMod<Shape>::~ShapeTerrainMod()
{
}

template <template <int> class Shape>
bool ShapeTerrainMod<Shape>::checkIntersects(const Segment& s) const
{
    return WFMath::Intersect(m_shape, s.getRect(), false) ||
        WFMath::Contains(s.getRect(), m_shape.getCorner(0), false);
}
    
template <template <int> class Shape>
void ShapeTerrainMod<Shape>::setShape(const Shape<2> & s)
{
    m_shape = s;
    m_box = m_shape.boundingBox();
}

template <template <int> class Shape> LevelTerrainMod<Shape>::~LevelTerrainMod()
{
}
    
template <template <int> class Shape>
void LevelTerrainMod<Shape>::apply(float &point, int x, int y) const
{
    if (Contains(this->m_shape,WFMath::Point<2>(x,y),true)) {
        point = this->m_function(point, m_level);
    }
}

template <template <int> class Shape>
TerrainMod * LevelTerrainMod<Shape>::clone() const
{
    return new LevelTerrainMod<Shape>(m_level, this->m_shape);
}

template <template <int> class Shape>
void LevelTerrainMod<Shape>::setShape(float level, const Shape<2> & s)
{
    ShapeTerrainMod<Shape>::setShape(s);
    m_level = level;
}

template <template <int> class Shape> AdjustTerrainMod<Shape>::~AdjustTerrainMod()
{
}
    
template <template <int> class Shape>
void AdjustTerrainMod<Shape>::apply(float &point, int x, int y) const
{
    if (Contains(this->m_shape,WFMath::Point<2>(x,y),true)) {
        point += m_dist;
    }
}
    
template <template <int> class Shape>
TerrainMod * AdjustTerrainMod<Shape>::clone() const
{
    return new AdjustTerrainMod<Shape>(m_dist, this->m_shape);
}

template <template <int> class Shape>
void AdjustTerrainMod<Shape>::setShape(float dist, const Shape<2> & s)
{
    ShapeTerrainMod<Shape>::setShape(s);
    m_dist = dist;
}

template <template <int> class Shape> SlopeTerrainMod<Shape>::~SlopeTerrainMod()
{
}
    
template <template <int> class Shape>
void SlopeTerrainMod<Shape>::apply(float &point, int x, int y) const
{
    if (Contains(this->m_shape,WFMath::Point<2>(x,y),true)) {
        float level = m_level + (this->m_shape.getCenter()[0] - x) * m_dx 
                              + (this->m_shape.getCenter()[1] - y) * m_dy;
        point = this->m_function(point, level);
    }
}
    
template <template <int> class Shape>
TerrainMod * SlopeTerrainMod<Shape>::clone() const
{
    return new SlopeTerrainMod<Shape>(m_level, m_dx, m_dy, this->m_shape);
}

template <template <int> class Shape>
void SlopeTerrainMod<Shape>::setShape(float level, float dx, float dy, const Shape<2> & s)
{
    ShapeTerrainMod<Shape>::setShape(s);
    m_level = level;
    m_dx = dx;
    m_dy = dy;
}


template <template <int> class Shape> CraterTerrainMod<Shape>::~CraterTerrainMod()
{
}
    
template <template <int> class Shape>
void CraterTerrainMod<Shape>::apply(float &point, int x, int y) const
{
    if (Contains(this->m_shape,WFMath::Point<2>(x,y),true)) {
        point += m_level;
    }
}
    
template <template <int> class Shape>
TerrainMod * CraterTerrainMod<Shape>::clone() const
{
    return new CraterTerrainMod<Shape>(m_level, this->m_shape);
}

template <template <int> class Shape>
void CraterTerrainMod<Shape>::setShape(float level, const Shape<2> & s)
{
    ShapeTerrainMod<Shape>::setShape(s);
    m_level = level;
}


} //namespace Mercator

#endif // MERCATOR_TERRAIN_MOD_IMPL_H
