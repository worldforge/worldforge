// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>

#include <iostream>

#include <cassert>

int terrain_mod_context_test(Mercator::Terrain & terrain)
{
    const WFMath::Ball<2> circ2(WFMath::Point<2>(0.0,0.0), 12.0);
    Mercator::TerrainMod * mp = new Mercator::LevelTerrainMod<WFMath::Ball>(10.0f, circ2);
    terrain.addMod(mp);

    mp->setContext(new Mercator::TerrainMod::Context);
    mp->context()->setId("foo");

    terrain.removeMod(mp);

    delete mp;

    return 0;
}

int main()
{
    Mercator::Terrain terrain(Mercator::Terrain::SHADED);

    terrain.setBasePoint(0, 0, 2.8);
    terrain.setBasePoint(1, 0, 7.1);
    terrain.setBasePoint(2, 0, 7.1);
    terrain.setBasePoint(0, 1, 0.2);
    terrain.setBasePoint(1, 1, 0.2);
    terrain.setBasePoint(2, 1, 0.2);
    terrain.setBasePoint(0, 2, 14.7);
    terrain.setBasePoint(1, 2, 14.7);
    terrain.setBasePoint(2, 2, 14.7);

    const WFMath::Ball<2> circ2(WFMath::Point<2>(0.0,0.0), 12.0);
    Mercator::TerrainMod * mp1 = new Mercator::LevelTerrainMod<WFMath::Ball>(10.0f, circ2);
    terrain.addMod(mp1);

    const WFMath::RotBox<2> rot(WFMath::Point<2>(-80.,-130.) ,
                                WFMath::Vector<2>(150.0,120.0),
                                WFMath::RotMatrix<2>().rotation(WFMath::Pi/4));
    Mercator::TerrainMod * mp2 = new Mercator::LevelTerrainMod<WFMath::RotBox>(10.0f, rot);
    terrain.addMod(mp2);

    const WFMath::Ball<2> ball(WFMath::Point<2>(80, 80), 10);
    Mercator::CraterTerrainMod<WFMath::Ball> * mp3 = new Mercator::CraterTerrainMod<WFMath::Ball>(-5.f, ball);
    terrain.addMod(mp3);

    Mercator::Segment * segment = terrain.getSegment(0, 0);

    if (segment == 0) {
        std::cerr << "Segment not created by addition of required basepoints"
                  << std::endl << std::flush;
        return 1;
    }

    segment->populate();

    segment = terrain.getSegment(1, 1);

    if (segment == 0) {
        std::cerr << "Segment not created by addition of required basepoints"
                  << std::endl << std::flush;
        return 1;
    }

    segment->populate();

    assert(segment->isValid());

    terrain.updateMod(mp3);

    assert(!segment->isValid());

    //Check that the stored bbox is correctly updated when calling updateMod().
    WFMath::AxisBox<2> mp3_rect1 = mp3->bbox();
    mp3->setShape(-5.f, WFMath::Ball<2>(WFMath::Point<2>(-80, 80), 10));
    WFMath::AxisBox<2> mp3_rect2 = terrain.updateMod(mp3);
    assert(mp3_rect1 == mp3_rect2);
    WFMath::AxisBox<2> mp3_rect3 = mp3->bbox();
    mp3->setShape(-5.f, WFMath::Ball<2>(WFMath::Point<2>(-80, -80), 10));
    WFMath::AxisBox<2> mp3_rect4 = terrain.updateMod(mp3);
    assert(mp3_rect3 == mp3_rect4);

    terrain.removeMod(mp1);

    delete mp1;

    terrain.removeMod(mp2);

    delete mp2;

    terrain.removeMod(mp3);

    delete mp3;

    return terrain_mod_context_test(terrain);
}
