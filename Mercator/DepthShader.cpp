// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/DepthShader.h>

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>

namespace Mercator {

DepthShader::DepthShader(float waterLevel, float murkyDepth) : Shader(true, true),
              m_waterLevel(waterLevel), m_murkyDepth(murkyDepth)
{
}

DepthShader::~DepthShader()
{
}

void DepthShader::shade(Surface & s) const
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
        float depth = height_data[i];
        if (depth > m_waterLevel) {
            data[++j] = 0.f;
        } else if (depth < m_murkyDepth) {
            data[++j] = 1.f;
        } else {
            data[++j] = 1.f - (depth - m_murkyDepth) / (m_waterLevel - m_murkyDepth);
        }
    }
}

} // namespace Mercator
