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
    static const std::string threshold;

    explicit HighShader(float threshold = 1.f);
    explicit HighShader(const Parameters & params);
    virtual ~HighShader();

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

class LowShader : public Shader {
  private:
    float m_threshold;
  public:
    static const std::string threshold;

    explicit LowShader(float threshold = -1.f);
    explicit LowShader(const Parameters & params);
    virtual ~LowShader();

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

class BandShader : public Shader {
  private:
    float m_lowThreshold;
    float m_highThreshold;
  public:
    static const std::string lowThreshold;
    static const std::string highThreshold;

    explicit BandShader(float low_threshold = -1.f, float high_threshold = 1.f);
    explicit BandShader(const Parameters & params);
    virtual ~BandShader();

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_FILL_THRESHOLD_SHADER_H
