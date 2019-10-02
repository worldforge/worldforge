// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iround.h"

#include "DepthShader.h"

#include "Segment.h"
#include "Surface.h"

#include <cmath>

#include <cassert>

namespace Mercator {

const std::string DepthShader::key_waterLevel("waterLevel");
const std::string DepthShader::key_murkyDepth("murkyDepth");

const float DepthShader::default_waterLevel = 0.f;
const float DepthShader::default_murkyDepth = -64.f;


DepthShader::DepthShader(float waterLevel, float murkyDepth) : 
             m_waterLevel(waterLevel), m_murkyDepth(murkyDepth)
{
}

DepthShader::DepthShader(const Parameters & params) :
             m_waterLevel(default_waterLevel), m_murkyDepth(default_murkyDepth)
{
    Parameters::const_iterator I = params.find(key_waterLevel);
    Parameters::const_iterator Iend = params.end();
    if (I != Iend) {
        m_waterLevel = I->second;
    }
    I = params.find(key_murkyDepth);
    if (I != Iend) {
        m_murkyDepth = I->second;
    }
}

DepthShader::~DepthShader()
{
}

bool DepthShader::checkIntersect(const Segment & s) const
{
    if (s.getMin() < m_waterLevel) {
        return true;
    } else {
        return false;
    }
}

void DepthShader::shade(Surface & s) const
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
        float depth = height_data[i];
        if (depth > m_waterLevel) {
            data[++j] = colorMin;
        } else if (depth < m_murkyDepth) {
            data[++j] = colorMax;
        } else {
            data[++j] = colorMax - I_ROUND(colorMax * ((depth - m_murkyDepth)
                                              / (m_waterLevel - m_murkyDepth)));
        }
    }
}

} // namespace Mercator
