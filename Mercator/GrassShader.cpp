// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iround.h"

#include <Mercator/GrassShader.h>

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>

#include <cmath>

#include <cassert>

namespace Mercator {

const std::string GrassShader::key_lowThreshold("lowThreshold");
const std::string GrassShader::key_highThreshold("highThreshold");
const std::string GrassShader::key_cutoff("cutoff");
const std::string GrassShader::key_intercept("intercept");

const float GrassShader::default_lowThreshold = 1.f;
const float GrassShader::default_highThreshold = 20.f;
const float GrassShader::default_cutoff = 1.f;
const float GrassShader::default_intercept = 2.f;

GrassShader::GrassShader(float lowThreshold, float highThreshold,
                         float cutoff, float intercept) :
             m_lowThreshold(lowThreshold), m_highThreshold(highThreshold),
             m_cutoff(cutoff), m_intercept(intercept)
{
}

GrassShader::GrassShader(const Parameters & params) :
             m_lowThreshold(default_lowThreshold),
             m_highThreshold(default_highThreshold),
             m_cutoff(default_cutoff),
             m_intercept(default_intercept)
{
    Parameters::const_iterator I = params.find(key_lowThreshold);
    Parameters::const_iterator Iend = params.end();
    if (I != Iend) {
        m_lowThreshold = I->second;
    }
    I = params.find(key_highThreshold);
    if (I != Iend) {
        m_highThreshold = I->second;
    }
    I = params.find(key_cutoff);
    if (I != Iend) {
        m_cutoff = I->second;
    }
    I = params.find(key_intercept);
    if (I != Iend) {
        m_intercept = I->second;
    }

}

GrassShader::~GrassShader()
{
}

inline ColorT GrassShader::slopeToAlpha(float height, float slope) const
{
    if ((height < m_lowThreshold) ||
        (height > m_highThreshold) ||
        (slope > m_intercept)) {
        return colorMin;
    } else if (slope < m_cutoff) {
        return colorMax;
    } else {
        return (ColorT)(colorMax * ((slope - m_cutoff) / (m_intercept - m_cutoff)));
    }
}

bool GrassShader::checkIntersect(const Segment & s) const
{
    if ((s.getMin() < m_highThreshold) &&
        (s.getMax() > m_lowThreshold)) {
        return true;
    } else {
        return false;
    }
}

void GrassShader::shade(Surface & s) const
{
    unsigned int channels = s.getChannels();
    assert(channels > 0);
    unsigned int chanAlpha = channels - 1;
    const Segment & seg = s.getSegment();
    ColorT * data = s.getData();
    const float * height_data = seg.getPoints();
    if (height_data == 0) {
        std::cerr << "WARNING: Mercator: Attempting to shade empty segment."
                  << std::endl << std::flush;
        return;
    }
    unsigned int size = seg.getSize();
    unsigned int res = seg.getResolution();

    unsigned int data_count = size * size * channels;
    for (unsigned int i = 0; i < data_count; ++i) {
        data[i] = colorMax;
    }

    // Deal with corner points
    s(0, 0, chanAlpha) = slopeToAlpha(seg.get(0,0), 0.f);
    s(res, 0, chanAlpha) = slopeToAlpha(seg.get(res,0), 0.f);
    s(0, res, chanAlpha) = slopeToAlpha(seg.get(0,res), 0.f);
    s(res, res, chanAlpha) = slopeToAlpha(seg.get(res,res), 0.f);

    for (unsigned int i = 1; i < res; ++i) {
        float height = seg.get(i, 0);
        float avgSlope = (std::fabs(seg.get(i - 1, 0) - height) +
                          std::fabs(seg.get(i + 1, 0) - height)) / 2.f;
        s(i, 0, chanAlpha) = slopeToAlpha(height, avgSlope);

        height = seg.get(i, res);
        avgSlope = (std::fabs(seg.get(i - 1, res) - height) +
                    std::fabs(seg.get(i + 1, res) - height)) / 2.f;
        s(i, res, chanAlpha) = slopeToAlpha(height, avgSlope);

        height = seg.get(0, i);
        avgSlope = (std::fabs(seg.get(0, i - 1) - height) +
                    std::fabs(seg.get(0, i + 1) - height)) / 2.f;
        s(0, i, chanAlpha) = slopeToAlpha(height, avgSlope);

        height = seg.get(res, i);
        avgSlope = (std::fabs(seg.get(res, i - 1) - height) +
                    std::fabs(seg.get(res, i + 1) - height)) / 2.f;
        s(res, i, chanAlpha) = slopeToAlpha(height, avgSlope);
        for (unsigned int j = 1; j < res; ++j) {
            height = seg.get(i, j);
            avgSlope = (std::fabs(seg.get(i + 1, j    ) - height) +
                        std::fabs(seg.get(i    , j + 1) - height) +
                        std::fabs(seg.get(i - 1, j    ) - height) +
                        std::fabs(seg.get(i    , j - 1) - height)) / 4.f;
            s(i, j, chanAlpha) = slopeToAlpha(height, avgSlope);
        }
    }
}

} // namespace Mercator
