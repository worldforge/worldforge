
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Alistair Riddoch

#include <Mercator/Terrain.h>
#include <Mercator/DepthShader.h>
#include <Mercator/Segment.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <cassert>

typedef WFMath::Point<2> Point2;

void testDepthShader()
{
    Mercator::Terrain terrain(Mercator::Terrain::SHADED, 16);

    Mercator::Shader::Parameters params;
    params[Mercator::DepthShader::key_waterLevel] = 0.f;
    params[Mercator::DepthShader::key_murkyDepth] = -12.f;
    
    Mercator::DepthShader* dshade = new Mercator::DepthShader();
    delete dshade;
    dshade = new Mercator::DepthShader(params);
    terrain.addShader(dshade, 0);
    
    terrain.setBasePoint(0, 0, -20);
    terrain.setBasePoint(0, 1, 1);
    terrain.setBasePoint(1, 0, 2);
    terrain.setBasePoint(1, 1, 0.5);
    terrain.setBasePoint(2, 0, 2);
    terrain.setBasePoint(2, 1, 0.5);
    
    Mercator::Segment* seg = terrain.getSegmentAtIndex(0,0);
    
    seg->populateSurfaces();
    seg->populate();
    seg->populateSurfaces();

    seg = terrain.getSegmentAtIndex(1,0);
    dshade->checkIntersect(*seg);
}

int main()
{
    testDepthShader();

    return 0;
}
