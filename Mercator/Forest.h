// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef MERCATOR_FOREST_H
#define MERCATOR_FOREST_H

#include <Mercator/RandCache.h>

#include <wfmath/axisbox.h>
#include <wfmath/polygon.h>

#include <map>

namespace Mercator {

class Plant;

/// \brief This is the core class for any area to be populated with vegetation.
///
/// Each instance of vegetation is represented by the Plant class,
/// and are stored in a 2 dimensional mapping container, which allows
/// the entire contents to be iterated, or a given square two dimenstional
/// area to be examined using the STL map API.
class Forest {
  public:
    /// \brief STL map to store a sparse array of Plant objects.
    ///
    /// Integer key is effectively used as an index.
    typedef std::map<int, Plant> PlantColumn;
    /// \brief STL map to store a sparse array of PlantColumn objects.
    ///
    /// The end effect is a sparse two dimensional array of plant objects
    /// which can be efficiently queried and scanned using STL iterators.
    typedef std::map<int, PlantColumn> PlantStore;
  private:
    /// Axis aligned rectangle which bounds the area covered by the forest.
    WFMath::AxisBox<2> m_bbox;
    /// Convex polygon defining the boundary of the forest.
    WFMath::Polygon<2> m_area;
    /// 2D spatial container with all the vegetation instances in.
    PlantStore m_plants;
    /// Seed value used to initialise the random number generator.
    unsigned long m_seed;
    /// Cache for optimising random number generation.
    RandCache m_randCache;

    void areaFromBBox();
    void bBoxFromArea();
  public:
    explicit Forest(unsigned long seed = 0);
    ~Forest();

    /// \brief Accessor for bounding rectangle.
    /// @return A const reference to the rectangle.
    const WFMath::AxisBox<2> & getBBox() const {
        return m_bbox;
    }

    /// \brief Accessor for polygonal area.
    /// @return A const reference to the polygon.
    const WFMath::Polygon<2> & getArea() const {
        return m_area;
    }

    /// \brief Accessor for container of vegetation.
    /// @return A const reference to the container.
    const PlantStore & getPlants() const {
        return m_plants;
    }

    void setArea(const WFMath::AxisBox<2> & area);
    void setArea(const WFMath::Polygon<2> & area);
    void setVolume(const WFMath::AxisBox<3> &);

    void populate();
};

}

#endif // MERCATOR_FOREST_H
