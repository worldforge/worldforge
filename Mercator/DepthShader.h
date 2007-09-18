// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_FILL_DEPTH_SHADER_H
#define MERCATOR_FILL_DEPTH_SHADER_H

#include <Mercator/Shader.h>

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
    float m_waterLevel;
    float m_murkyDepth;
  public:
    static const std::string key_waterLevel;
    static const std::string key_murkyDepth;

    static const float default_waterLevel;
    static const float default_murkyDepth;

    explicit DepthShader(float waterLevel = default_waterLevel,
                         float murkyDepth = default_murkyDepth);
    explicit DepthShader(const Parameters & params);
    virtual ~DepthShader();

    const float waterLevel() const { return m_waterLevel; }
    const float murkyDepth() const { return m_murkyDepth; }

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_FILL_DEPTH_SHADER_H
