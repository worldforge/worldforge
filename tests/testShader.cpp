// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>

template <class ShaderType>
void shadeTest(Mercator::Segment & segment)
{
    ShaderType shader;
    Mercator::Surface surface(segment, shader);

    surface.populate();
}

int main()
{
    Mercator::Segment segment;
    segment.populate();

    shadeTest<Mercator::FillShader>(segment);
    shadeTest<Mercator::HighShader>(segment);
    shadeTest<Mercator::LowShader>(segment);
    shadeTest<Mercator::BandShader>(segment);
}
