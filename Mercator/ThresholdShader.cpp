// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/ThresholdShader.h>

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>

namespace Mercator {

HighShader::HighShader(float threshold) : Shader(true, true),
                                          m_threshold(threshold)
{
}

HighShader::~HighShader()
{
}

void HighShader::shade(Surface & s) const
{
    unsigned int channels = s.getChannels();
    assert(channels > 0);
    unsigned int colors = channels - 1;
    float * data = s.getData();
    const float * height_data = s.getSegment().getPoints();
    if (height_data == 0) {
        std::cerr << "WARNING: Mercator: Attempting to shade empty segment."
                  << std::endl << std::flush;
        return;
    }
    unsigned int size = s.getSegment().getSize();

    unsigned int count = size * size;
    int j = -1;
    for (unsigned int i = 0; i < count; ++i) {
        for (unsigned int k = 0; k < colors; ++k) {
            data[++j] = 1.f;
        }
        data[++j] = ((height_data[i] > m_threshold) ? 1.f : 0.f);
    }
}

LowShader::LowShader(float threshold) : Shader(true, true),
                                        m_threshold(threshold)
{
}

LowShader::~LowShader()
{
}

void LowShader::shade(Surface & s) const
{
    unsigned int channels = s.getChannels();
    assert(channels > 0);
    unsigned int colors = channels - 1;
    float * data = s.getData();
    const float * height_data = s.getSegment().getPoints();
    if (height_data == 0) {
        std::cerr << "WARNING: Mercator: Attempting to shade empty segment."
                  << std::endl << std::flush;
        return;
    }
    unsigned int size = s.getSegment().getSize();

    unsigned int count = size * size;
    int j = -1;
    for (unsigned int i = 0; i < count; ++i) {
        for (unsigned int k = 0; k < colors; ++k) {
            data[++j] = 1.f;
        }
        data[++j] = ((height_data[i] < m_threshold) ? 1.f : 0.f);
    }
}

BandShader::BandShader(float lowThreshold, float highThreshold) : Shader(true, true),
    m_lowThreshold(lowThreshold), m_highThreshold(highThreshold)
{
}

BandShader::~BandShader()
{
}

void BandShader::shade(Surface & s) const
{
    unsigned int channels = s.getChannels();
    assert(channels > 0);
    unsigned int colors = channels - 1;
    float * data = s.getData();
    const float * height_data = s.getSegment().getPoints();
    if (height_data == 0) {
        std::cerr << "WARNING: Mercator: Attempting to shade empty segment."
                  << std::endl << std::flush;
        return;
    }
    unsigned int size = s.getSegment().getSize();

    unsigned int count = size * size;
    int j = -1;
    for (unsigned int i = 0; i < count; ++i) {
        for (unsigned int k = 0; k < colors; ++k) {
            data[++j] = 1.f;
        }
        data[++j] = (((height_data[i] > m_lowThreshold) &&
                      (height_data[i] < m_highThreshold)) ? 1.f : 0.f);
    }
}

} // namespace Mercator
