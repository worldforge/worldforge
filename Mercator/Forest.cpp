// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Mercator/Forest.h>

#include <Mercator/Plant.h>

#include <wfmath/MersenneTwister.h>
#include <wfmath/intersect.h>

#include <iostream>
#include <cmath>

#ifdef HAVE_LRINTF
    #define I_ROUND(x) (::lrintf(x)) 
#elif defined(HAVE_RINTF)
    #define I_ROUND(x) ((int)::rintf(x)) 
#else
    #define I_ROUND(x) ((int)::rint(x)) 
#endif

namespace Mercator {

Forest::Forest(unsigned long seed) : m_seed(seed), 
    m_randCache(seed, new ZeroSpiralOrdering())
{
}

Forest::~Forest()
{
}

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

void Forest::bBoxFromArea()
{
}

void Forest::setArea(const WFMath::AxisBox<2> & area)
{
    m_bbox = area;
    areaFromBBox();
}

void Forest::setArea(const WFMath::Polygon<2> & area)
{
    m_area = area;
}

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


void Forest::populate()
// This function uses a pseudo-random technique to populate the forrest with
// trees. This algorithm as the following essental properties:
//
//   * It is repeatable. It can be repeated on the client and the server,
//     and give identical results.
//
//   * It is location independant. It gives the same results even if the
//     forest is in a different place.
//
//   * It is shape and size independant. A given area of the forest is
//     the same even if the borders of the forest change.
//
//   * It is localisable. It is possible to only partially populate the
//     the forest, and still get the same results in that area.
//
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
