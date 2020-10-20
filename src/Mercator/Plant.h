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
struct Plant {
    /// Position of the vegetation relative to its grid point.
    WFMath::Point<2> m_displacement;
    /// Orientation of the vegetation.
    WFMath::Quaternion m_orientation;
    /// Height of the vegetation.
    float m_height;

};

}

#endif // MERCATOR_PLANT_H
