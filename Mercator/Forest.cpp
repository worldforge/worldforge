// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iround.h"

#include <Mercator/Forest.h>
#include <Mercator/Plant.h>
#include <Mercator/Area.h>

#include <wfmath/MersenneTwister.h>
#include <wfmath/intersect.h>

#include <iostream>
#include <cmath>

namespace Mercator {


/// \brief Construct a new forest with the given seed.
Forest::Forest(unsigned long seed) : 
    m_area(NULL),
    m_seed(seed), 
    m_randCache(seed, new ZeroSpiralOrdering())
{
}

/// \brief Destruct a forest.
///
/// All contained vegetation is lost, so references to contained
/// vegetation must not be maintained if this is likely to occur.
Forest::~Forest()
{
}

void Forest::setArea(Area* area)
{
    m_area = area;
}

static const float plant_chance = 0.04;
static const float plant_min_height = 5;
static const float plant_height_range = 20;


/// \brief This function uses a pseudo-random technique to populate the
/// forest with trees. This algorithm as the following essental properties:
///
///   - It is repeatable. It can be repeated on the client and the server,
///     and give identical results.
///
///   - It is location independant. It gives the same results even if the
///     forest is in a different place.
///
///   - It is shape and size independant. A given area of the forest is
///     the same even if the borders of the forest change.
///
///   - It is localisable. It is possible to only partially populate the
///     the forest, and still get the same results in that area.
///
/// This function will have no effect if the area defining the forest remains
/// uninitialised. Any previously generated contents are erased.
/// For each instance a new seed is used to ensure it is repeatable, and
/// height, displacement and orientation are calculated.
void Forest::populate()
{
    if (!m_area) return;
    WFMath::AxisBox<2> bbox(m_area->bbox());
    
    // Fill the plant store with plants.
    m_plants.clear();
    WFMath::MTRand rng;

    int lx = I_ROUND(bbox.lowCorner().x()),
        ly = I_ROUND(bbox.lowCorner().y()),
        hx = I_ROUND(bbox.highCorner().x()),
        hy = I_ROUND(bbox.highCorner().y());

    for(int j = ly; j < hy; ++j) {
        for(int i = lx; i < hx; ++i) {
            double prob=m_randCache(i,j);
            if (prob < plant_chance) {
                if (!m_area->contains(i,j)) continue;
                
//                std::cout << "Plant at [" << i << ", " << j << "]"
//                          << std::endl << std::flush;
                rng.seed((int)(prob / plant_chance * 123456)); //this is a bit of a hack
                Plant & plant = m_plants[i][j];
                plant.setHeight(rng() * plant_height_range + plant_min_height);
                plant.setDisplacement(WFMath::Point<2>(rng() - 0.5f,
                                                       rng() - 0.5f));
                plant.setOrientation(WFMath::Quaternion(2, rng() * 2 * WFMath::Pi));
            }
        }
    }
}

}
