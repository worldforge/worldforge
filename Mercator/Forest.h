// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef MERCATOR_FOREST_H
#define MERCATOR_FOREST_H

#include <wfmath/axisbox.h>

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
  public:
    Forest();
    ~Forest();

    const PlantStore & getPlants() const {
        return m_plants;
    }

    void set_area(const WFMath::AxisBox<2> & area);
    void set_volume(const WFMath::AxisBox<3> &);

    void populate();
};

}

#endif // MERCATOR_FOREST_H
