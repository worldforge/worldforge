// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_FILL_THRESHOLD_SHADER_H
#define MERCATOR_FILL_THRESHOLD_SHADER_H

#include <Mercator/Shader.h>

namespace Mercator {

class HighShader : public Shader {
  private:
    float m_threshold;
  public:
    explicit HighShader(float threshold = 1.f);
    virtual ~HighShader();

    virtual bool checkIntersect(Surface &) const;
    virtual void shade(Surface &) const;
};

class LowShader : public Shader {
  private:
    float m_threshold;
  public:
    explicit LowShader(float threshold = -1.f);
    virtual ~LowShader();

    virtual bool checkIntersect(Surface &) const;
    virtual void shade(Surface &) const;
};

class BandShader : public Shader {
  private:
    float m_lowThreshold;
    float m_highThreshold;
  public:
    explicit BandShader(float lowThreshold = -1.f, float highThreshold = 1.f);
    virtual ~BandShader();

    virtual bool checkIntersect(Surface &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_FILL_THRESHOLD_SHADER_H
