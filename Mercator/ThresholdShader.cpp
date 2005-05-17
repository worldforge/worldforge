// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/ThresholdShader.h>

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>

namespace Mercator {

HighShader::HighShader(float threshold) : m_threshold(threshold)
{
}

HighShader::HighShader(const Parameters & params)
{
    // FIXME Read the values from the params list
}

HighShader::~HighShader()
{
}

bool HighShader::checkIntersect(const Segment & s) const
{
    if (s.getMax() > m_threshold) {
        return true;
    } else {
        return false;
    }
}

void HighShader::shade(Surface & s) const
{
    unsigned int channels = s.getChannels();
    assert(channels > 0);
    unsigned int colors = channels - 1;
    ColorT * data = s.getData();
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
            data[++j] = colorMax;
        }
        data[++j] = ((height_data[i] > m_threshold) ? colorMax : colorMin);
    }
}

LowShader::LowShader(float threshold) : m_threshold(threshold)
{
}

LowShader::LowShader(const Parameters & params)
{
    // FIXME Read the values from the params list
}

LowShader::~LowShader()
{
}

bool LowShader::checkIntersect(const Segment & s) const
{
    if (s.getMin() < m_threshold) {
        return true;
    } else {
        return false;
    }
}

void LowShader::shade(Surface & s) const
{
    unsigned int channels = s.getChannels();
    assert(channels > 0);
    unsigned int colors = channels - 1;
    ColorT * data = s.getData();
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
            data[++j] = colorMax;
        }
        data[++j] = ((height_data[i] < m_threshold) ? colorMax : colorMin);
    }
}

BandShader::BandShader(float lowThreshold, float highThreshold) : 
    m_lowThreshold(lowThreshold), m_highThreshold(highThreshold)
{
}

BandShader::BandShader(const Parameters & params)
{
    // FIXME Read the values from the params list
}

BandShader::~BandShader()
{
}

bool BandShader::checkIntersect(const Segment & s) const
{
    if ((s.getMin() < m_highThreshold) &&
        (s.getMax() > m_lowThreshold)) {
        return true;
    } else {
        return false;
    }
}

void BandShader::shade(Surface & s) const
{
    unsigned int channels = s.getChannels();
    assert(channels > 0);
    unsigned int colors = channels - 1;
    ColorT * data = s.getData();
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
            data[++j] = colorMax;
        }
        data[++j] = (((height_data[i] > m_lowThreshold) &&
                      (height_data[i] < m_highThreshold)) ? colorMax : colorMin);
    }
}

} // namespace Mercator
