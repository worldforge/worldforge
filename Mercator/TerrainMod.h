// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes, Alistair Riddoch

#ifndef MERCATOR_TERRAIN_MOD_H
#define MERCATOR_TERRAIN_MOD_H

#include <wfmath/intersect.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>

namespace Mercator {

class Segment;

/// \brief Base class for modifiers to the procedurally generated terrain.
///
/// Anything that modifies the terrain implements this interface.
class TerrainMod
{
public:
    class Context {
      public:
        Context();

        virtual ~Context();

        const std::string & id() { return m_id; }

        void setId(const std::string &);
      protected:
        std::string m_id;
    };

    Context * context() const { return m_context; }

    void setContext(Context *);

    /// Accessor for the bounding box of the geometric shape.
    const WFMath::AxisBox<2> & bbox() const
    {
        return m_box;
    }

    TerrainMod();

    virtual ~TerrainMod();

    int addToSegment(Segment &) const;
    void updateToSegment(Segment &) const;
    void removeFromSegment(Segment &) const;

    /// \brief Apply this modifier on a terrain segment
    ///
    /// The segment is at x,y in local coordinates.
    /// Output is placed into point.
    virtual void apply(float &point, int x, int y) const = 0;

    /// \brief Create a copy of this modifier.
    virtual TerrainMod *clone() const = 0;
  protected:
    Context * m_context;
    /// The bounding box of the geometric shape.
    WFMath::AxisBox<2> m_box;

};

/// \brief Terrain modifier which is defined by a shape variable.
///
/// This template extends TerrainMod by adding the ability to query the
/// bounding box of the shape that defines this modification to the terrain.
template <typename Shape>
class ShapeTerrainMod : public TerrainMod
{
public:
    /// \brief Constructor
    ///
    /// @param s shape of the modifier.
    ShapeTerrainMod(const Shape &s);
    virtual ~ShapeTerrainMod(); // {}

protected:
    /// \brief Shape of the modifier.
    Shape m_shape;
};


/// \brief Terrain modifier that defines an area of fixed height.
///
/// This modifier sets all points inside the shape to the same altitude
template <typename Shape>
class LevelTerrainMod : public ShapeTerrainMod<Shape>
{
public:
    /// \brief Constructor
    ///
    /// @param level The height level of all points affected.
    /// @param s shape of the modifier.
    LevelTerrainMod(float level, const Shape &s)
        : ShapeTerrainMod<Shape>(s), m_level(level) {}

    virtual ~LevelTerrainMod();

    virtual void apply(float &point, int x, int y) const;
    virtual TerrainMod *clone() const;

private:
    /// \brief Copy constructor.
    LevelTerrainMod(LevelTerrainMod&); // {}

protected:
    /// \brief The height level of all points affected.
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

    /// \brief Constructor
    ///
    /// @param dist adjustment to the height of all points affected.
    /// @param s shape of the modifier.
    AdjustTerrainMod(float dist, const Shape &s)
        : ShapeTerrainMod<Shape>(s), m_dist(dist) {}

    virtual ~AdjustTerrainMod();

    virtual void apply(float &point, int x, int y) const;
    virtual TerrainMod *clone() const;

private:
    /// \brief Copy constructor.
    AdjustTerrainMod(AdjustTerrainMod&) {}

protected:
    /// \brief Adjustment to the height of all points affected.
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

    /// \brief Constructor
    ///
    /// @param level the height of the centre point.
    /// @param dx the rate of change of the height along X.
    /// @param dy the rate of change of the height along Y.
    /// @param s shape of the modifier.
    SlopeTerrainMod(float level, float dx, float dy, const Shape &s)
        : ShapeTerrainMod<Shape>(s), m_level(level), m_dx(dx), m_dy(dy) {}

    virtual ~SlopeTerrainMod();

    virtual void apply(float &point, int x, int y) const;
    virtual TerrainMod *clone() const;

private:
    /// \brief Copy constructor.
    SlopeTerrainMod(SlopeTerrainMod&) {}

protected:
    /// \brief The height of the centre point.
    float m_level;
    /// \brief The rate of change of the height along X.
    float m_dx;
    /// \brief The rate of change of the height along Y.
    float m_dy;
};

/// \brief Terrain modifier that defines a crater.
///
/// This modifier creates an area where a sphere shaped volume has been
/// subtracted from the terrain surface to create a spherical crater.
class CraterTerrainMod : public TerrainMod
{
public:
    /// \brief Constructor
    ///
    /// @param s Sphere that defines the shape of the crater.
    CraterTerrainMod(const WFMath::Ball<3> &s);

    virtual ~CraterTerrainMod(); // {}

    virtual void apply(float &point, int x, int y) const;
    virtual TerrainMod *clone() const;

private:
    /// \brief Copy constructor.
    CraterTerrainMod(CraterTerrainMod&) {}

    /// \brief Sphere that defines the shape of the crater.
    WFMath::Ball<3> m_shape;

};

} //namespace Mercator

#endif // MERCATOR_TERRAIN_MOD_H
