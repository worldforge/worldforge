// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes, Alistair Riddoch

#ifndef MERCATOR_BASE_POINT_H
#define MERCATOR_BASE_POINT_H

#include <iostream>

namespace Mercator { 

/// \brief Point on the fundamental grid that is used as the basis for terrain.
///
/// The terrain is defined by a sparse grid of evenly spaced points, each of
/// which is defined by an instance of this class. The most fundamental
/// property of each point is its elevation stored as height. Additional
/// parameters for roughness and falloff are often specified.
class BasePoint {
  private:
    float m_height;
    float m_roughness;
    float m_falloff;

  public:
    static const float HEIGHT;
    static const float ROUGHNESS;
    static const float FALLOFF;

    explicit BasePoint(float h = HEIGHT, float r = ROUGHNESS, float f = FALLOFF) :
                 m_height(h), m_roughness(r), m_falloff(f) {}

    const float height() const { return m_height; }
    float & height() { return m_height; }

    const float roughness() const { return m_roughness; }
    float & roughness() { return m_roughness; }

    const float falloff() const { return m_falloff; }
    float & falloff() { return m_falloff; }

    unsigned int seed() const { return (unsigned int)(m_height * 1000.0);}
};

} //namespace Mercator

#endif // MERCATOR_BASE_POINT_H
