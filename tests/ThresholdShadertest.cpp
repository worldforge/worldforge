// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Alistair Riddoch

#include <Mercator/Terrain.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/Segment.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <cassert>

typedef WFMath::Point<2> Point2;

void testHighShader()
{
    Mercator::Terrain terrain(Mercator::Terrain::SHADED, 16);

    Mercator::Shader::Parameters params;
    params[Mercator::HighShader::key_threshold] = 5.f;
    
    Mercator::HighShader* dshade = new Mercator::HighShader();
    delete dshade;
    dshade = new Mercator::HighShader(params);
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

    seg = terrain.getSegment(1,0);
    dshade->checkIntersect(*seg);
}

void testLowShader()
{
    Mercator::Terrain terrain(Mercator::Terrain::SHADED, 16);

    Mercator::Shader::Parameters params;
    params[Mercator::LowShader::key_threshold] = 5.f;
    
    Mercator::LowShader* dshade = new Mercator::LowShader();
    delete dshade;
    dshade = new Mercator::LowShader(params);
    terrain.addShader(dshade, 0);
    
    terrain.setBasePoint(0, 0, 2);
    terrain.setBasePoint(0, 1, 1);
    terrain.setBasePoint(1, 0, 20);
    terrain.setBasePoint(1, 1, 10);
    terrain.setBasePoint(2, 0, 15);
    terrain.setBasePoint(2, 1, 19);
    
    Mercator::Segment* seg = terrain.getSegment(0,0);
    
    seg->populateSurfaces();
    seg->populate();
    seg->populateSurfaces();

    seg = terrain.getSegment(1,0);
    dshade->checkIntersect(*seg);
}

void testBandShader()
{
    Mercator::Terrain terrain(Mercator::Terrain::SHADED, 16);

    Mercator::Shader::Parameters params;
    params[Mercator::BandShader::key_lowThreshold] = 2.f;
    params[Mercator::BandShader::key_highThreshold] = 8.f;
    
    Mercator::BandShader* dshade = new Mercator::BandShader();
    delete dshade;
    dshade = new Mercator::BandShader(params);
    terrain.addShader(dshade, 0);
    
    terrain.setBasePoint(0, 0, 2);
    terrain.setBasePoint(0, 1, 1);
    terrain.setBasePoint(1, 0, 20);
    terrain.setBasePoint(1, 1, 10);
    terrain.setBasePoint(2, 0, 15);
    terrain.setBasePoint(2, 1, 19);
    
    Mercator::Segment* seg = terrain.getSegment(0,0);
    
    seg->populateSurfaces();
    seg->populate();
    seg->populateSurfaces();

    seg = terrain.getSegment(1,0);
    dshade->checkIntersect(*seg);
}

int main()
{
    testHighShader();
    testLowShader();
    testBandShader();

    return 0;
}
