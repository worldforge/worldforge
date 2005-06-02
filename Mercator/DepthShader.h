// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_FILL_DEPTH_SHADER_H
#define MERCATOR_FILL_DEPTH_SHADER_H

#include <Mercator/Shader.h>

namespace Mercator {

class DepthShader : public Shader {
  private:
    float m_waterLevel;
    float m_murkyDepth;
  public:
    static const std::string key_waterLevel;
    static const std::string key_murkyDepth;

    static const float default_waterLevel = 0.f;
    static const float default_murkyDepth = -64.f;

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
