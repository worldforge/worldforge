// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/FillShader.h>

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>

namespace Mercator {

FillShader::FillShader() : Shader(true, true)
{
}

FillShader::~FillShader()
{
}

void FillShader::shade(Surface & s) const
{
    unsigned int channels = s.getChannels();
    float * data = s.getData();
    unsigned int size = s.getSegment().getSize();

    unsigned int buflen = size * size * channels;
    for (unsigned int i = 0; i < buflen; ++i) {
        data[i] = 1.f;
    }
}

} // namespace Mercator
