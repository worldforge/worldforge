// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes, Alistair Riddoch

#ifndef MERCATOR_TERRAIN_MOD_H
#define MERCATOR_TERRAIN_MOD_H

#include <wfmath/intersect.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>

namespace Mercator {

/// \brief Base class for modifiers to the procedurally generated terrain.
///
/// Anything that modifies the terrain implements this interface.
class TerrainMod
{
public:
    virtual ~TerrainMod();
    //apply this modifier on a terrain segment at x,y in local coordinates
    //output is placed into point
    virtual void apply(float &point, int x, int y) const = 0;

    //get the boundingbox of the modifier
    virtual WFMath::AxisBox<2> bbox() const = 0;

    virtual TerrainMod *clone() const = 0;
};

/// \brief Terrain modifier which is defined by a shape variable.
///
/// This template extends TerrainMod by adding the ability to query the
/// bounding box of the shape that defines this modification to the terrain.
template <typename Shape>
class ShapeTerrainMod : public TerrainMod
{
public:
    ShapeTerrainMod(const Shape &s) : m_shape(s) {}
    virtual ~ShapeTerrainMod(); // {}

    virtual WFMath::AxisBox<2> bbox() const; // { return m_shape.boundingBox(); }

protected:
    Shape m_shape;
};


/// \brief Terrain modifier that defines an area of fixed height.
///
/// This modifier sets all points inside the shape to the same altitude
template <typename Shape>
class LevelTerrainMod : public ShapeTerrainMod<Shape>
{
public:

    LevelTerrainMod(float level, const Shape &s)
        : ShapeTerrainMod<Shape>(s), m_level(level) {}

    virtual ~LevelTerrainMod();

    virtual void apply(float &point, int x, int y) const;
    virtual TerrainMod *clone() const;

private:
    LevelTerrainMod(LevelTerrainMod&); // {}

protected:
    float m_level;
};

/// \brief Terrain modifier that defines an area of adjusted height.
///
/// This modifier changes the altitude of all points inside the shape
/// by the same amount.
template <typename Shape>
class AdjustTerrainMod : public ShapeTerrainMod<Shape>
{
public:

    AdjustTerrainMod(float dist, const Shape &s)
        : ShapeTerrainMod<Shape>(s), m_dist(dist) {}

    virtual ~AdjustTerrainMod();

    virtual void apply(float &point, int x, int y) const;
    virtual TerrainMod *clone() const;

private:
    AdjustTerrainMod(AdjustTerrainMod&) {}

protected:
    float m_dist;
};

/// \brief Terrain modifier that defines an area of sloped height.
///
/// This modifier creates a sloped area. The center point is set to a level
/// and all other points are set based on specified gradients.
template <typename Shape>
class SlopeTerrainMod : public ShapeTerrainMod<Shape>
{
public:

    SlopeTerrainMod(float level, float dx, float dy, const Shape &s)
        : ShapeTerrainMod<Shape>(s), m_level(level), m_dx(dx), m_dy(dy) {}

    virtual ~SlopeTerrainMod();

    virtual void apply(float &point, int x, int y) const;
    virtual TerrainMod *clone() const;

private:
    SlopeTerrainMod(SlopeTerrainMod&) {}

protected:
    float m_level, m_dx, m_dy;
};

/// \brief Terrain modifier that defines a crater.
///
/// This modifier creates an area where a sphere shaped volume has been
/// subtracted from the terrain surface to create a spherical crater.
class CraterTerrainMod : public TerrainMod
{
public:

    CraterTerrainMod(const WFMath::Ball<3> &s) : m_shape(s) {
        WFMath::AxisBox<3> bb=m_shape.boundingBox();
        ab = WFMath::AxisBox<2> (
                    WFMath::Point<2>(bb.lowerBound(0), bb.lowerBound(1)),
                    WFMath::Point<2>(bb.upperBound(0), bb.upperBound(1))
               );
    }

    virtual ~CraterTerrainMod(); // {}

    virtual WFMath::AxisBox<2> bbox() const;
    virtual void apply(float &point, int x, int y) const;
    virtual TerrainMod *clone() const;

private:
    CraterTerrainMod(CraterTerrainMod&) {}

    WFMath::Ball<3> m_shape;
    WFMath::AxisBox<2> ab;

};

} //namespace Mercator

#endif // MERCATOR_TERRAIN_MOD_H
