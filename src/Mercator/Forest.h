// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef MERCATOR_FOREST_H
#define MERCATOR_FOREST_H

#include "RandCache.h"

#include <wfmath/axisbox.h>
#include <wfmath/polygon.h>

#include <map>
#include <string>

namespace Mercator {

class Plant;
class Area;

class SpeciesParameter;

typedef std::map<std::string, SpeciesParameter> ParameterDict;

/// \brief A set of constraints on a plant parameter.
class SpeciesParameter {
  public:
    /// \brief The minimum value a parameter should take.
    float min;
    /// \brief The range of values a parameter should take.
    float range;
};

/// \brief Data about a species of plant in a Forest.
///
/// Species objects encapsulate all the data required to determine
/// where instances of a given species occur, and how they are scattered
/// in a Forest.
class Species {
  public:
    /// Probability that this species will occur at each grid node.
    float m_probability;

    /// Multiplyer for how deviated from the grid items should be.
    float m_deviation;

    /// Arbitrary parameters.
    ParameterDict m_parameters;
};

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

    /// STL vector of plant species in this forest.
    typedef std::vector<Species> PlantSpecies;
  private:
    /// Area of terrain affected by the presence of this forest.
    Area* m_area;
    
    /// List of species in this forest.
    PlantSpecies m_species;
    /// 2D spatial container with all the vegetation instances in.
    PlantStore m_plants;
    /// Seed value used to initialise the random number generator.
    unsigned long m_seed;
    /// Cache for optimising random number generation.
    RandCache m_randCache;

  public:
    explicit Forest(unsigned long seed = 0);
    ~Forest();

    /// \brief Accessor for polygonal area.
    Area* getArea() const {
        return m_area;
    }

    /// Accessor for list of species in this forest.
    PlantSpecies & species() {
        return m_species;
    }

    /// \brief Accessor for container of vegetation.
    /// @return A const reference to the container.
    const PlantStore & getPlants() const {
        return m_plants;
    }

    void setArea(Area* a);

    void populate();
};

}

#endif // MERCATOR_FOREST_H
