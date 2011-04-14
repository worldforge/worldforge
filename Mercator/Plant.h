// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef MERCATOR_PLANT_H
#define MERCATOR_PLANT_H

#include <wfmath/point.h>
#include <wfmath/quaternion.h>

#include <string>

namespace Mercator {

/// \brief This is the simple class for representing instances of vegetation.
///
/// It is used in instances of class Forest to represent each instance
/// of vegetation created when the Forest is populated.
class Plant {
  private:
    /// Position of the vegetation relative to its grid point.
    WFMath::Point<2> m_displacement;
    /// Orientation of the vegetation.
    WFMath::Quaternion m_orientation;
    /// Height of the vegetation.
    float m_height;
  public:
    /// Height is initialised explicitly to zero.
    Plant();
    /// Nothing special in the destructor.
    ~Plant();

    /// Accessor for displacement from grid point.
    const WFMath::Point<2> & getDisplacement() const {
        return m_displacement;
    }

    /// Set the displacement to a new value.
    void setDisplacement(const WFMath::Point<2> & d) {
        m_displacement = d;
    }

    /// Accessor for orientation.
    const WFMath::Quaternion & getOrientation() const {
        return m_orientation;
    }

    /// Set the orientation to a new value.
    void setOrientation(const WFMath::Quaternion & o) {
        m_orientation = o;
    }

    /// Accessor for height.
    const float getHeight() const {
        return m_height;
    }

    /// Set the height to a new value.
    void setHeight(float h) {
        m_height = h;
    }

    /// Set a named parameter value for this plant.
    void setParameter(const std::string &, float) { }
};

}

#endif // MERCATOR_PLANT_H
