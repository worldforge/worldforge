
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Alistair Riddoch

#include <Mercator/Terrain.h>
#include <Mercator/GrassShader.h>
#include <Mercator/Segment.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <cassert>

typedef WFMath::Point<2> Point2;

void testGrassShader()
{
    Mercator::Terrain terrain(Mercator::Terrain::SHADED, 16);

    Mercator::Shader::Parameters params;
    params[Mercator::GrassShader::key_lowThreshold] = 2.f;
    params[Mercator::GrassShader::key_highThreshold] = 15.f;
    params[Mercator::GrassShader::key_cutoff] = 0.4f;
    params[Mercator::GrassShader::key_intercept] = 4.f;
    
    Mercator::GrassShader* dshade = new Mercator::GrassShader();
    delete dshade;
    dshade = new Mercator::GrassShader(params);
    terrain.addShader(dshade, 0);
    
    terrain.setBasePoint(0, 0, 20);
    terrain.setBasePoint(0, 1, 1);
    terrain.setBasePoint(1, 0, 2);
    terrain.setBasePoint(1, 1, 0.5);
    terrain.setBasePoint(2, 0, 2);
    terrain.setBasePoint(2, 1, 0.5);
    
    Mercator::Segment* seg = terrain.getSegment(0,0);
    
    seg->populateSurfaces();
    seg->populate();
    seg->populateSurfaces();

    // This segment is too low to shade
    seg = terrain.getSegment(1,0);
    dshade->checkIntersect(*seg);
}

int main()
{
    testGrassShader();

    return 0;
}
