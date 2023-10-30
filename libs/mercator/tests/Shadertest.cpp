// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>

template <class ShaderType>
int shadeTest(Mercator::Segment & segment)
{
    ShaderType shader;
    Mercator::Surface surface(segment, shader);

    if (surface.getChannels() != 4) {
        std::cerr << "Surface does not have 4 channels."
                  << std::endl << std::flush;
        return 1;
    }

    if (shader.checkIntersect(segment)) {
        surface.populate();
    }
    return 0;
}

int main()
{
    Mercator::Segment segment(0,0,Mercator::defaultResolution);
    segment.populate();

    int errorCount = 0;
    errorCount += shadeTest<Mercator::FillShader>(segment);
    errorCount += shadeTest<Mercator::HighShader>(segment);
    errorCount += shadeTest<Mercator::LowShader>(segment);
    errorCount += shadeTest<Mercator::BandShader>(segment);
    errorCount += shadeTest<Mercator::DepthShader>(segment);
    errorCount += shadeTest<Mercator::GrassShader>(segment);

    return (errorCount ? 1 : 0);
}
