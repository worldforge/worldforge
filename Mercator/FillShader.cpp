// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "FillShader.h"

#include "Segment.h"
#include "Surface.h"

namespace Mercator {

FillShader::FillShader()
{
}

FillShader::FillShader(const Parameters & params)
{
}

FillShader::~FillShader()
{
}

bool FillShader::checkIntersect(const Segment &) const
{
    return true;
}

void FillShader::shade(Surface & s) const
{
    unsigned int channels = s.getChannels();
    ColorT * data = s.getData();
    unsigned int size = s.getSegment().getSize();

    unsigned int buflen = size * size * channels;
    for (unsigned int i = 0; i < buflen; ++i) {
        data[i] = colorMax;
    }
}

} // namespace Mercator
