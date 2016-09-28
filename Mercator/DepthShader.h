// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_FILL_DEPTH_SHADER_H
#define MERCATOR_FILL_DEPTH_SHADER_H

#include "Shader.h"

namespace Mercator {

/// \brief Shader to give the appearance of deep murky water.
///
/// Areas of terrain covered by water should be darker as less
/// distinct as they get deeper. This shader adds a uniform alpha blended
/// surface below water level which becomes more opaque as it gets deeper
/// concealing the underlying appearance of the terrain from a viewer
/// at the surface.
class DepthShader : public Shader {
  private:
    /// The level of the surface of the water.
    float m_waterLevel;
    /// The depth at which the bottom becomes completely obscured.
    float m_murkyDepth;
  public:
    /// Key string used when specifying the water level parameter.
    static const std::string key_waterLevel;
    /// Key string used when specifying the murky depth parameter.
    static const std::string key_murkyDepth;

    /// Default level of the surface of the water.
    static const float default_waterLevel;
    /// Default depth at which the bottom becomes completely obscured.
    static const float default_murkyDepth;

    /// \brief Constructor
    ///
    /// @param waterLevel level of the surface of the water.
    /// @param murkyDepth depth at which the bottom becomes completely obscured.
    explicit DepthShader(float waterLevel = default_waterLevel,
                         float murkyDepth = default_murkyDepth);
    /// \brief Constructor
    ///
    /// @param params a map of parameters for the shader.
    explicit DepthShader(const Parameters & params);
    virtual ~DepthShader();

    /// Accessor for the level of the surface of the water.
    const float waterLevel() const { return m_waterLevel; }
    /// Accessor for the depth at which the bottom becomes completely obscured.
    const float murkyDepth() const { return m_murkyDepth; }

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_FILL_DEPTH_SHADER_H
