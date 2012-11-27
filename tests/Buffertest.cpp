// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Alistair Riddoch

#include <Mercator/Buffer.h>
#include <Mercator/Segment.h>

int main()
{
    Mercator::Segment s(0,0,64);
    Mercator::Buffer<float> b(s);

    return 0;
}

// stubs

namespace Mercator {

Segment::Segment(int x, int y, unsigned int resolution) :
                 m_res(resolution),
                 m_size(m_res+1),
                 m_xRef(x),
                 m_yRef(y)
{
}

Segment::~Segment()
{
}

const float BasePoint::HEIGHT = 8.0;
const float BasePoint::ROUGHNESS = 1.25;
const float BasePoint::FALLOFF = 0.25;

}
