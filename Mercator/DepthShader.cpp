// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/DepthShader.h>

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>

namespace Mercator {

DepthShader::DepthShader(float waterLevel, float murkyDepth) : 
              m_waterLevel(waterLevel), m_murkyDepth(murkyDepth)
{
}

DepthShader::~DepthShader()
{
}

bool DepthShader::checkIntersect(Surface & s) const
{
    if (s.m_segment.getMin() < m_waterLevel) {
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
            data[++j] = colorMax - (unsigned char)(colorMax * ((depth - 
                        m_murkyDepth) / (m_waterLevel - m_murkyDepth)));
        }
    }
}

} // namespace Mercator
