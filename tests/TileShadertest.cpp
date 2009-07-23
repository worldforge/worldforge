// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/TileShader.h>

int main()
{
    Mercator::Segment segment(0,0,Mercator::defaultResolution);
    segment.populate();

    Mercator::TileShader tileShader;
    tileShader.addShader(new Mercator::FillShader(), 0);
    tileShader.addShader(new Mercator::BandShader(-2.f, 1.5f), 1);
    tileShader.addShader(new Mercator::GrassShader(1.f, 80.f, .5f, 1.f), 2);
    tileShader.addShader(new Mercator::DepthShader(0.f, -10.f), 3);
    tileShader.addShader(new Mercator::HighShader(110.f), 4);
    Mercator::Surface * surface = tileShader.newSurface(segment);
    surface->populate();
}
