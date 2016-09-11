
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Alistair Riddoch

#include <Mercator/Terrain.h>
#include <Mercator/FillShader.h>
#include <Mercator/Segment.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <cassert>

typedef WFMath::Point<2> Point2;

void testFillShader()
{
    Mercator::Terrain terrain(Mercator::Terrain::SHADED, 16);

    Mercator::Shader::Parameters params;
    
    Mercator::FillShader* dshade = new Mercator::FillShader();
    delete dshade;
    dshade = new Mercator::FillShader(params);
    terrain.addShader(dshade, 0);
    
    terrain.setBasePoint(0, 0, -20);
    terrain.setBasePoint(0, 1, 1);
    terrain.setBasePoint(1, 0, 2);
    terrain.setBasePoint(1, 1, 0.5);
    
    Mercator::Segment* seg = terrain.getSegment(0,0);
    
    seg->populate();
    seg->populateSurfaces();
}

int main()
{
    testFillShader();

    return 0;
}
