// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Alistair Riddoch

#include <Mercator/Segment.h>

#include <wfmath/point.h>
#include <wfmath/axisbox.h>

int main()
{
    Mercator::Segment s(0,0,64);

    s.populate();
    s.populateNormals();
    s.populateSurfaces();
    s.invalidate();

    s.populate();
    s.populateNormals();
    s.populateSurfaces();

    float h;
    WFMath::Vector<3> normal;
    s.getHeightAndNormal(17.0, 23.0, h, normal);
    s.getHeightAndNormal(17.9, 23.1, h, normal);
    s.getHeightAndNormal(17.1, 23.9, h, normal);

    int lx, ly, hx, hy;
    s.clipToSegment(WFMath::AxisBox<2>(WFMath::Point<2>(50,50), WFMath::Point<2>(100,100)), lx, hx, ly, hy);

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
