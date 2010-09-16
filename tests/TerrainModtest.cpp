// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>

#include <iostream>

int terrain_mod_context_test(Mercator::Terrain & terrain)
{
    const WFMath::Ball<2> circ2(WFMath::Point<2>(0.0,0.0), 12.0);
    Mercator::LevelTerrainMod<WFMath::Ball<2> > mod2(10.0f, circ2);
    Mercator::TerrainMod * mp = terrain.addMod(mod2);

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
    Mercator::LevelTerrainMod<WFMath::Ball<2> > mod2(10.0f, circ2);
    terrain.addMod(mod2);

    const WFMath::RotBox<2> rot(WFMath::Point<2>(-80.,-130.) ,
                                WFMath::Vector<2>(150.0,120.0),
                                WFMath::RotMatrix<2>().rotation(WFMath::Pi/4));
    Mercator::LevelTerrainMod<WFMath::RotBox<2> > mod3(10.0f, rot);
    terrain.addMod(mod3);

    const WFMath::Ball<3> ball(WFMath::Point<3>(80, 80, 0), 10);
    Mercator::CraterTerrainMod mod4(ball);
    terrain.addMod(mod4);

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

    return terrain_mod_context_test(terrain);
}
