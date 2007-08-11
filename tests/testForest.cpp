// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <Mercator/Forest.h>
#include <Mercator/Plant.h>
#include <Mercator/Area.h>

#include <iostream>

#include <cassert>

typedef WFMath::Point<2> Point2;

int main()
{
    {
        Mercator::Forest forest(4249162ul);
    }

    {
        Mercator::Forest forest(4249162ul);

        Mercator::Forest::PlantSpecies & species = forest.species();

        // Forest is not yet populated
        assert(forest.getPlants().empty());
        assert(species.empty());
        forest.populate();
        // Forest has zero area, so even when populated it is empty
        assert(forest.getPlants().empty());
        assert(species.empty());

        Mercator::Area* ar = new Mercator::Area(1, false);
        WFMath::Polygon<2> p;
        
        p.addCorner(p.numCorners(), Point2(5, 8));
        p.addCorner(p.numCorners(), Point2(40, -1));
        p.addCorner(p.numCorners(), Point2(45, 16));
        p.addCorner(p.numCorners(), Point2(30, 28));
        p.addCorner(p.numCorners(), Point2(-2, 26));
        p.addCorner(p.numCorners(), Point2(1, 5));
        
        ar->setShape(p);
        forest.setArea(ar);

        forest.populate();
        // Forest has no species, so even when populated it is empty
        assert(forest.getPlants().empty());
        assert(species.empty());

        // TODO(alriddoch) Add some species.
        Mercator::Species pine;
        pine.m_probability = 0.04f;
        pine.m_deviation = 1.f;

        species.push_back(pine);

    //    assert(!forest.getBBox().isValid());
    //    assert(forest.getArea().isValid());
     //   forest.setArea(WFMath::AxisBox<2>(WFMath::Point<2>(-5, -5),
    //                                      WFMath::Point<2>(5, 5)));
 //       assert(forest.getBBox().isValid());
  //      assert(forest.getArea().isValid());

        forest.populate();
        // Forest should now contain some plants
        assert(!forest.getPlants().empty());

        const Mercator::Forest::PlantStore & ps = forest.getPlants();

        Mercator::Forest::PlantStore::const_iterator I = ps.begin();
        for(; I != ps.end(); ++I) {
            Mercator::Forest::PlantColumn::const_iterator J = I->second.begin();
            for(; J != I->second.end(); ++J) {
                const Mercator::Plant & p = J->second;
                std::cout << "Query found plant at [" << I->first
                          << ", " << J->first << "] with height "
                          << p.getHeight();
                std::cout << " displaced to "
                          << (WFMath::Vector<2>(I->first, J->first) +
                              p.getDisplacement())
                          << std::endl << std::flush;
            }
        }
    }
}
