// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>

#include <iostream>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <cassert>

int terrain_mod_context_test(Mercator::Terrain & terrain)
{
    const WFMath::Ball<2> circ2(WFMath::Point<2>(0.0,0.0), 12.0);
    Mercator::TerrainMod * mp = new Mercator::LevelTerrainMod<WFMath::Ball>(10.0f, circ2);
    terrain.updateMod(1, mp);

    mp->setContext(new Mercator::TerrainMod::Context);
    mp->context()->setId("foo");

    terrain.updateMod(1, nullptr);

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
    terrain.updateMod(1, mp1);

    Mercator::Segment * segment = terrain.getSegmentAtIndex(0, 0);

    assert(segment->getMods().size() == 1);

    Mercator::TerrainMod * mp1_1 = new Mercator::LevelTerrainMod<WFMath::Ball>(10.0f, circ2);
    terrain.updateMod(1, mp1_1);
    assert(segment->getMods().size() == 1);
    assert(segment->getMods().find(1)->second == mp1_1);
    delete mp1;

    const WFMath::RotBox<2> rot(
          WFMath::Point<2>(-80.,-130.) ,
          WFMath::Vector<2>(150.0,120.0),
          WFMath::RotMatrix<2>().rotation(WFMath::numeric_constants<WFMath::CoordType>::pi()/4));
    Mercator::TerrainMod * mp2 = new Mercator::LevelTerrainMod<WFMath::RotBox>(10.0f, rot);
    terrain.updateMod(2, mp2);

    const WFMath::Ball<2> ball(WFMath::Point<2>(80, 80), 10);
    Mercator::CraterTerrainMod<WFMath::Ball> * mp3 = new Mercator::CraterTerrainMod<WFMath::Ball>(-5.f, ball);
    terrain.updateMod(3, mp3);


    if (segment == 0) {
        std::cerr << "Segment not created by addition of required basepoints"
                  << std::endl << std::flush;
        return 1;
    }

    segment->populate();

    segment = terrain.getSegmentAtIndex(1, 1);

    if (segment == 0) {
        std::cerr << "Segment not created by addition of required basepoints"
                  << std::endl << std::flush;
        return 1;
    }

    segment->populate();

    assert(segment->isValid());

    terrain.updateMod(3, mp3);

    assert(!segment->isValid());

    //Check that the stored bbox is correctly updated when calling updateMod().
    WFMath::AxisBox<2> mp3_rect1 = mp3->bbox();
    mp3->setShape(-5.f, WFMath::Ball<2>(WFMath::Point<2>(-80, 80), 10));
    WFMath::AxisBox<2> mp3_rect2 = terrain.updateMod(3, mp3);
    assert(mp3_rect1 == mp3_rect2);
    WFMath::AxisBox<2> mp3_rect3 = mp3->bbox();
    mp3->setShape(-5.f, WFMath::Ball<2>(WFMath::Point<2>(-80, -80), 10));
    WFMath::AxisBox<2> mp3_rect4 = terrain.updateMod(3, mp3);
    assert(mp3_rect3 == mp3_rect4);

    terrain.updateMod(1, nullptr);

    delete mp1_1;

    terrain.updateMod(2, nullptr);

    delete mp2;

    terrain.updateMod(3, nullptr);

    delete mp3;

    return terrain_mod_context_test(terrain);
}
