// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <Mercator/Forest.h>

#include <Mercator/Plant.h>

namespace Mercator {

Forest::Forest()
{
}

Forest::~Forest()
{
}

void Forest::set_area(const WFMath::AxisBox<2> & area)
{
    m_area = area;
}


void Forest::set_volume(const WFMath::AxisBox<3> & vol)
{
    m_area = WFMath::AxisBox<2>(WFMath::Point<2>(vol.lowCorner().x(),
                                                 vol.lowCorner().y()),
                                WFMath::Point<2>(vol.highCorner().x(),
                                                 vol.highCorner().y()));

}

void Forest::populate()
{
    // Fill the plant store with plants.
    m_plants.clear();
}

}
