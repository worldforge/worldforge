// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef MERCATOR_FOREST_H
#define MERCATOR_FOREST_H

#include <wfmath/axisbox.h>
#include <Mercator/RandCache.h>

#include <map>

namespace Mercator {

class Plant;

/// This is the core class for any area to be populated with vegetation
class Forest {
  public:
    typedef std::map<int, Plant> PlantColumn;
    typedef std::map<int, PlantColumn> PlantStore;
  private:
    WFMath::AxisBox<2> m_area;
    PlantStore m_plants;
    unsigned long m_seed;
    RandCache m_randCache;
  public:
    explicit Forest(unsigned long seed = 0);
    ~Forest();

    const WFMath::AxisBox<2> & getArea() const {
        return m_area;
    }

    const PlantStore & getPlants() const {
        return m_plants;
    }

    void setArea(const WFMath::AxisBox<2> & area);
    void setVolume(const WFMath::AxisBox<3> &);

    void populate();
};

}

#endif // MERCATOR_FOREST_H
