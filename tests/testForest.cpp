// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <Mercator/Forest.h>
#include <Mercator/Plant.h>

#include <iostream>

#include <cassert>

int main()
{
    {
        Mercator::Forest forest(4249162ul);

        // Forest is not yet populated
        assert(forest.getPlants().empty());
        forest.populate();
        // Forest has zero area, so even when populated it is empty
        assert(forest.getPlants().empty());

        assert(!forest.getBBox().isValid());
        assert(forest.getArea().isValid());
        forest.setArea(WFMath::AxisBox<2>(WFMath::Point<2>(-5, -5),
                                          WFMath::Point<2>(5, 5)));
        assert(forest.getBBox().isValid());
        assert(forest.getArea().isValid());

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
