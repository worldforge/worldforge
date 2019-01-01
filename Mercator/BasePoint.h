// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes, Alistair Riddoch

#ifndef MERCATOR_BASE_POINT_H
#define MERCATOR_BASE_POINT_H

#include <iostream>

namespace Mercator { 

/// \brief Point on the fundamental grid that is used as the basis for terrain.
///
/// The terrain is defined by a sparse grid of evenly spaced points, each of
/// which is defined by an instance of this class. The most fundamental
/// property of each point is its elevation stored as height, as this is also
/// used to seed the random number generators. Additional parameters for
/// roughness and falloff are often specified.
class BasePoint {
  private:
    /// The height at the base point.
    float m_height;
    /// The roughness at the base point.
    float m_roughness;
    /// The falloff at the base point.
    float m_falloff;

  public:
    /// Default height at the base point.
    static constexpr float HEIGHT = 8.0;
    /// Default roughness at the base point.
    static constexpr float ROUGHNESS = 1.25;
    /// Default falloff at the base point.
    static constexpr float FALLOFF = 0.25;

    /// \brief Constructor
    ///
    /// @param h height at the base point.
    /// @param r roughness at the base point.
    /// @param f falloff at the base point.
    explicit BasePoint(float h = HEIGHT,
                       float r = ROUGHNESS,
                       float f = FALLOFF) :
             m_height(h), m_roughness(r), m_falloff(f) {}

    /// Accessor for the height at the base point.
    float height() const { return m_height; }
    /// Accessor for the height at the base point.
    float & height() { return m_height; }

    /// Accessor for the roughness at the base point.
    float roughness() const { return m_roughness; }
    /// Accessor for the roughness at the base point.
    float & roughness() { return m_roughness; }

    /// Accessor for the falloff at the base point.
    float falloff() const { return m_falloff; }
    /// Accessor for the falloff at the base point.
    float & falloff() { return m_falloff; }

    /// Calculate the random seed used at this base point.
    //unsigned int seed() const { return (unsigned int)(m_height * 1000.0);}
    unsigned int seed() const;
};

} //namespace Mercator

#endif // MERCATOR_BASE_POINT_H
