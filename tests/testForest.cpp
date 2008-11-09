// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <Mercator/Forest.h>
#include <Mercator/Plant.h>
#include <Mercator/Area.h>

#include <iostream>

#ifdef NDEBUG
#undef NDEBUG
#endif
#define DEBUG

#include <cassert>

typedef WFMath::Point<2> Point2;

void dumpPlants(const Mercator::Forest::PlantStore & plants)
{
    Mercator::Forest::PlantStore::const_iterator I = plants.begin();
    for(; I != plants.end(); ++I) {
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

int countPlants(const Mercator::Forest::PlantStore & plants)
{
    int plant_count = 0;
    Mercator::Forest::PlantStore::const_iterator I = plants.begin();
    for(; I != plants.end(); ++I) {
        plant_count += I->second.size();
    }
    return plant_count;
}

int main()
{
    // Test constructor
    {
        Mercator::Forest mf;
    }

    // Test constructor
    {
        Mercator::Forest mf(23);
    }

    // Test getArea()
    {
        Mercator::Forest mf;

        Mercator::Area * a = mf.getArea();

        assert(a == 0);
    }

    // Test species()
    {
        Mercator::Forest mf;

        Mercator::Forest::PlantSpecies & mps = mf.species();

        assert(mps.empty());
    }

    {
        Mercator::Forest forest(4249162ul);

        Mercator::Forest::PlantSpecies & species = forest.species();

        const Mercator::Forest::PlantStore & plants = forest.getPlants();

        // Forest is not yet populated
        assert(plants.empty());
        assert(species.empty());
        forest.populate();
        // Forest has zero area, so even when populated it is empty
        assert(plants.empty());
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
        assert(plants.empty());
        assert(species.empty());

        {
            Mercator::Species pine;
            pine.m_probability = 0.04;
            pine.m_deviation = 1.f;

            species.push_back(pine);
        }

        forest.populate();
        // Forest should now contain some plants
        assert(!plants.empty());

        dumpPlants(plants);

        int plant_count = countPlants(plants);

        {
            Mercator::Species oak;
            oak.m_probability = 0.02;
            oak.m_deviation = 1.f;

            species.push_back(oak);
        }

        forest.populate();
        // Forest should now contain some plants
        assert(!plants.empty());
        assert(countPlants(plants) > plant_count);

        dumpPlants(plants);

        std::cout << countPlants(plants) << "," << plant_count
                  << std::endl << std::flush;

    }
}
