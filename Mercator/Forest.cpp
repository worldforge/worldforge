// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iround.h"

#include <Mercator/Forest.h>

#include <Mercator/Plant.h>

#include <wfmath/MersenneTwister.h>
#include <wfmath/intersect.h>

#include <iostream>
#include <cmath>

namespace Mercator {


/// \brief Construct a new forest with the given seed.
Forest::Forest(unsigned long seed) : m_seed(seed), 
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

/// Calculate the convex polygon described by an axis aligned box.
void Forest::areaFromBBox()
{
    const WFMath::Point<2> & lc = m_bbox.lowCorner();
    const WFMath::Point<2> & hc = m_bbox.highCorner();

    m_area.clear();
    m_area.addCorner(0, lc);
    m_area.addCorner(0, WFMath::Point<2>(lc.x(), hc.y()));
    m_area.addCorner(0, hc);
    m_area.addCorner(0, WFMath::Point<2>(hc.x(), lc.y()));
}

/// Calculate axis aligned box the bounds the convex polygon.
void Forest::bBoxFromArea()
{
}

/// Define the area of the forest in terms of an axis aligned rectangle.
void Forest::setArea(const WFMath::AxisBox<2> & area)
{
    m_bbox = area;
    areaFromBBox();
}

/// Define the area of the forest as a convex polygon.
void Forest::setArea(const WFMath::Polygon<2> & area)
{
    m_area = area;
}

/// Define the area of the forest in terms of an axis aligned box.
/// @param vol an axis aligned box a section of which defines the area of the
/// forest.
void Forest::setVolume(const WFMath::AxisBox<3> & vol)
{
    m_bbox = WFMath::AxisBox<2>(WFMath::Point<2>(vol.lowCorner().x(),
                                                 vol.lowCorner().y()),
                                WFMath::Point<2>(vol.highCorner().x(),
                                                 vol.highCorner().y()));
    areaFromBBox();
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
    if (!m_bbox.isValid()) {
        return;
    }
    // Fill the plant store with plants.
    m_plants.clear();
    WFMath::MTRand rng;

    int lx = I_ROUND(m_bbox.lowCorner().x()),
        ly = I_ROUND(m_bbox.lowCorner().y()),
        hx = I_ROUND(m_bbox.highCorner().x()),
        hy = I_ROUND(m_bbox.highCorner().y());

    for(int j = ly; j < hy; ++j) {
        for(int i = lx; i < hx; ++i) {
            double prob=m_randCache(i,j);
            if (prob < plant_chance) {
                if (!Contains(m_area,WFMath::Point<2>(i,j),false)) continue;
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
