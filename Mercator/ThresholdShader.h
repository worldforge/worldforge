// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_FILL_THRESHOLD_SHADER_H
#define MERCATOR_FILL_THRESHOLD_SHADER_H

#include "Shader.h"

namespace Mercator {

/// \brief Surface shader that defines the surface above a given level.
class HighShader : public Shader {
  private:
    /// The level above which the shader renders.
    float m_threshold;
  public:
    /// Key string used when specifying the threshold parameter.
    static const std::string key_threshold;

    /// Default level above which the shader renders.
    static const float default_threshold;

    /// \brief Constructor
    ///
    /// @param threshold level below which the shader renders.
    explicit HighShader(float threshold = default_threshold);
    /// \brief Constructor
    ///
    /// @param params a map of parameters for the shader.
    explicit HighShader(const Parameters & params);
    virtual ~HighShader();

    /// Accessor for level above which the shader renders.
    const float threshold() const { return m_threshold; }

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

/// \brief Surface shader that defines the surface below a given level.
class LowShader : public Shader {
  private:
    /// The level below which the shader renders.
    float m_threshold;
  public:
    /// Key string used when specifying the threshold parameter.
    static const std::string key_threshold;

    /// Default level below which the shader renders.
    static const float default_threshold;

    /// \brief Constructor
    ///
    /// @param threshold level below which the shader renders.
    explicit LowShader(float threshold = default_threshold);
    /// \brief Constructor
    ///
    /// @param params a map of parameters for the shader.
    explicit LowShader(const Parameters & params);
    virtual ~LowShader();

    /// Accessor for level below which the shader renders.
    const float threshold() const { return m_threshold; }

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

/// \brief Surface shader that defines the surface between two levels.
class BandShader : public Shader {
  private:
    /// The level above which the shader renders.
    float m_lowThreshold;
    /// The level below which the shader renders.
    float m_highThreshold;
  public:
    /// Key string used when specifying the low threshold parameter.
    static const std::string key_lowThreshold;
    /// Key string used when specifying the high threshold parameter.
    static const std::string key_highThreshold;

    /// Default level above which the shader renders.
    static const float default_lowThreshold;
    /// Default level below which the shader renders.
    static const float default_highThreshold;

    /// \brief Constructor
    ///
    /// @param low_threshold level above which the shader renders.
    /// @param high_threshold level below which the shader renders.
    explicit BandShader(float low_threshold = default_lowThreshold,
                        float high_threshold = default_highThreshold);
    /// \brief Constructor
    ///
    /// @param params a map of parameters for the shader.
    explicit BandShader(const Parameters & params);
    virtual ~BandShader();

    /// Accessor for the level above which the shader renders.
    const float lowThreshold() const { return m_lowThreshold; }
    /// Accessor for the level below which the shader renders.
    const float highThreshold() const { return m_highThreshold; }

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_FILL_THRESHOLD_SHADER_H
