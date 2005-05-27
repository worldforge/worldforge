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
    static const std::string waterLevel;
    static const std::string murkyDepth;

    explicit DepthShader(float waterLevel = 0.f, float murkyDepth = -64.f);
    explicit DepthShader(const Parameters & params);
    virtual ~DepthShader();

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_FILL_DEPTH_SHADER_H
