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
    static const std::string key_threshold;

    static const float default_threshold = 1.f;

    explicit HighShader(float threshold = default_threshold);
    explicit HighShader(const Parameters & params);
    virtual ~HighShader();

    const float threshold() const { return m_threshold; }

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

class LowShader : public Shader {
  private:
    float m_threshold;
  public:
    static const std::string key_threshold;

    static const float default_threshold = -1.f;

    explicit LowShader(float threshold = default_threshold);
    explicit LowShader(const Parameters & params);
    virtual ~LowShader();

    const float threshold() const { return m_threshold; }

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

class BandShader : public Shader {
  private:
    float m_lowThreshold;
    float m_highThreshold;
  public:
    static const std::string key_lowThreshold;
    static const std::string key_highThreshold;

    static const float default_lowThreshold = -1.f;
    static const float default_highThreshold = 1.f;

    explicit BandShader(float low_threshold = default_lowThreshold,
                        float high_threshold = default_highThreshold);
    explicit BandShader(const Parameters & params);
    virtual ~BandShader();

    const float lowThreshold() const { return m_lowThreshold; }
    const float highThreshold() const { return m_highThreshold; }

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_FILL_THRESHOLD_SHADER_H
