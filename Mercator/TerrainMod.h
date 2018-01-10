// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes, Alistair Riddoch

#ifndef MERCATOR_TERRAIN_MOD_H
#define MERCATOR_TERRAIN_MOD_H

#include "Effector.h"

#include <wfmath/intersect.h>
#include <wfmath/ball.h>

namespace Mercator {

class Segment;

/// \brief Base class for modifiers to the procedurally generated terrain.
///
/// Anything that modifies the terrain implements this interface.
class TerrainMod : public Effector
{
protected:
    /// \brief Function used to apply this mod to existing points
    ///
    /// This makes the basic mods much more powerful without the need for
    /// extra classes. It completely obsoletes AdjustTerrainMod, which is
    /// now the same as LevelTerrainMod with this function changed from
    /// set() to sum()
    effector_func m_function;
public:
    TerrainMod();

    ~TerrainMod() override;

    /// \brief Change the function used to apply this mod to existing points
    void setFunction(effector_func f) {
        m_function = f;
    }

    /// \brief Apply this modifier on a terrain segment
    ///
    /// The segment is at x,y in local coordinates.
    /// Output is placed into point.
    virtual void apply(float &point, int x, int z) const = 0;
};

/// \brief Terrain modifier which is defined by a shape variable.
///
/// This template extends TerrainMod by adding the ability to query the
/// bounding box of the shape that defines this modification to the terrain.
template <template <int> class Shape>
class ShapeTerrainMod : public TerrainMod
{
public:
    /// \brief Constructor
    ///
    /// @param s shape of the modifier.
    explicit ShapeTerrainMod(const Shape<2> &s);

    ~ShapeTerrainMod() override;

    bool checkIntersects(const Segment& s) const override;

    void setShape(const Shape<2> & s);
protected:
    /// \brief Shape of the modifier.
    Shape<2> m_shape;
};


/// \brief Terrain modifier that defines an area of fixed height.
///
/// This modifier sets all points inside the shape to the same altitude
template <template <int> class Shape>
class LevelTerrainMod : public ShapeTerrainMod<Shape>
{
public:
    /// \brief Constructor
    ///
    /// @param level The height level of all points affected.
    /// @param s shape of the modifier.
    LevelTerrainMod(float level, const Shape<2> &s)
        : ShapeTerrainMod<Shape>(s), m_level(level) {}

    /// \brief Copy constructor.
    LevelTerrainMod(LevelTerrainMod&) = delete;

    virtual ~LevelTerrainMod();

    virtual void apply(float &point, int x, int z) const;

    void setShape(float level, const Shape<2> & s);

protected:
    /// \brief The height level of all points affected.
    float m_level;
};

/// \brief Terrain modifier that defines an area of adjusted height.
///
/// This modifier changes the altitude of all points inside the shape
/// by the same amount.
template <template <int> class Shape>
class AdjustTerrainMod : public ShapeTerrainMod<Shape>
{
public:

    /// \brief Constructor
    ///
    /// @param dist adjustment to the height of all points affected.
    /// @param s shape of the modifier.
    AdjustTerrainMod(float dist, const Shape<2> &s)
        : ShapeTerrainMod<Shape>(s), m_dist(dist) {}

    /// \brief Copy constructor.
    AdjustTerrainMod(AdjustTerrainMod&) = delete;

    virtual ~AdjustTerrainMod();

    virtual void apply(float &point, int x, int z) const;

    void setShape(float dist, const Shape<2> & s);

protected:
    /// \brief Adjustment to the height of all points affected.
    float m_dist;
};

/// \brief Terrain modifier that defines an area of sloped height.
///
/// This modifier creates a sloped area. The center point is set to a level
/// and all other points are set based on specified gradients.
template <template <int> class Shape>
class SlopeTerrainMod : public ShapeTerrainMod<Shape>
{
public:

    /// \brief Constructor
    ///
    /// @param level the height of the centre point.
    /// @param dx the rate of change of the height along X.
    /// @param dz the rate of change of the height along Z.
    /// @param s shape of the modifier.
    SlopeTerrainMod(float level, float dx, float dz, const Shape<2> &s)
        : ShapeTerrainMod<Shape>(s), m_level(level), m_dx(dx), m_dz(dz) {}

    /// \brief Copy constructor.
    SlopeTerrainMod(SlopeTerrainMod&) = delete;

    virtual ~SlopeTerrainMod();

    virtual void apply(float &point, int x, int z) const;

    void setShape(float level, float dx, float dz, const Shape<2> & s);

protected:
    /// \brief The height of the centre point.
    float m_level;
    /// \brief The rate of change of the height along X.
    float m_dx;
    /// \brief The rate of change of the height along Z.
    float m_dz;
};

/// \brief Terrain modifier that defines a crater.
///
/// This modifier creates an area where a sphere shaped volume has been
/// subtracted from the terrain surface to create a spherical crater.
template <template <int> class Shape>
class CraterTerrainMod : public ShapeTerrainMod<Shape>
{
public:
    /// \brief Constructor
    ///
    /// @param s Sphere that defines the shape of the crater.
    CraterTerrainMod(float level, const Shape<2> &s)
        : ShapeTerrainMod<Shape>(s), m_level(level) {}

    /// \brief Copy constructor.
    CraterTerrainMod(CraterTerrainMod&) = delete;

    virtual ~CraterTerrainMod();

    virtual void apply(float &point, int x, int z) const;

    void setShape(float level, const Shape<2> & s);

protected:
    /// \brief The height level of the crater center
    float m_level;
};

} //namespace Mercator

#endif // MERCATOR_TERRAIN_MOD_H
