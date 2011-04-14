
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Alistair Riddoch

#include <Mercator/Segment.h>

#include <wfmath/point.h>
#include <wfmath/axisbox.h>

#include <cstdlib>

int main(int argc, char ** argv)
{
    int iterations = 1;

    if (argc > 1) {
        iterations = strtol(argv[1], 0, 10);
    }

    Mercator::Segment s(0,0,64);

    for (int i = 0; i < iterations; ++i) {

        s.populate();

    }

    return 0;
}

// stubs

#include <Mercator/Shader.h>
#include <Mercator/Surface.h>

namespace Mercator {

const float BasePoint::HEIGHT = 8.0;
const float BasePoint::ROUGHNESS = 1.25;
const float BasePoint::FALLOFF = 0.25;

void Surface::populate()
{
}

Surface * Shader::newSurface(const Segment & segment) const
{
    return 0;
}

}
