// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes

#ifndef MERCATOR_TERRAIN_MOD_H
#define MERCATOR_TERRAIN_MOD_H

#include <wfmath/intersect.h>
#include <wfmath/axisbox.h>

namespace Mercator {

class TerrainMod
{
public:
    //apply this modifier on a terrain segment at x,y in local coordinates
    //output is placed into point
    virtual void apply(float &point, int x, int y) const = 0;

    //get the boundingbox of the modifier
    virtual WFMath::AxisBox<2> bbox() const = 0;

    virtual TerrainMod *clone() = 0;
};

template <typename Shape>
class ShapeTerrainMod : public TerrainMod
{
public:
    ShapeTerrainMod(const Shape &s) : m_shape(s) {}
    virtual ~ShapeTerrainMod() {}
    
    virtual WFMath::AxisBox<2> bbox() const { return m_shape.boundingBox(); }
    
protected:
    Shape m_shape;
};


//this modifier sets all points inside the shape to the same altitude
template <typename Shape>
class LevelTerrainMod : public ShapeTerrainMod<Shape>
{
public:

    LevelTerrainMod(float level, const Shape &s) 
        : ShapeTerrainMod<Shape>(s), m_level(level) {}
    
    virtual ~LevelTerrainMod() {}
    
    virtual void apply(float &point, int x, int y) const {
        if (Contains(m_shape,WFMath::Point<2>(x,y),true)) {
            point = m_level;
        }
    }

    virtual TerrainMod *clone() {
        return new LevelTerrainMod<Shape>(m_level, m_shape);
    }

private:
    LevelTerrainMod(LevelTerrainMod&) {}
    
protected:
    float m_level;
};

//this modifier changes the altitude of all points inside the shape
//by the same amount
template <typename Shape>
class AdjustTerrainMod : public ShapeTerrainMod<Shape>
{
public:

    AdjustTerrainMod(float dist, const Shape &s) 
        : ShapeTerrainMod<Shape>(s), m_dist(dist) {}
    
    virtual ~AdjustTerrainMod() {}
    
    virtual void apply(float &point, int x, int y) const {
        if (Contains(m_shape,WFMath::Point<2>(x,y),true)) {
            point += m_dist;
        }
    }
    
    virtual TerrainMod *clone() {
        return new AdjustTerrainMod<Shape>(m_dist, m_shape);
    }

private:
    AdjustTerrainMod(AdjustTerrainMod&) {}

protected:
    float m_dist;
};

//this modifier creates a sloped area. The center point is
//set to a level and all other points are set based on specified gradients
template <typename Shape>
class SlopeTerrainMod : public ShapeTerrainMod<Shape>
{
public:

    SlopeTerrainMod(float level, float dx, float dy, const Shape &s) 
        : ShapeTerrainMod<Shape>(s), m_level(level), m_dx(dx), m_dy(dy) {}
    
    virtual ~SlopeTerrainMod() {}
    
    virtual void apply(float &point, int x, int y) const {
        if (Contains(m_shape,WFMath::Point<2>(x,y),true)) {
            point = m_level + (m_shape.getCenter()[0] - x) * m_dx 
                            + (m_shape.getCenter()[1] - y) * m_dy;
        }
    }
    
    virtual TerrainMod *clone() {
        return new SlopeTerrainMod<Shape>(m_level, m_dx, m_dy, m_shape);
    }

private:
    SlopeTerrainMod(SlopeTerrainMod&) {}

protected:
    float m_level, m_dx, m_dy;
};
            
} //namespace Mercator

#endif // MERCATOR_TERRAIN_MOD_H
