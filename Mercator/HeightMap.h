// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#ifndef MERCATOR_HEIGHTMAP_H
#define MERCATOR_HEIGHTMAP_H

#include "BasePoint.h"
#include "Buffer.h"

#include <wfmath/vector.h>

namespace WFMath {
class MTRand;
}

namespace Mercator {

/// \brief Class storing heightfield and other data for a single fixed size
/// square area of terrain defined by four adjacent BasePoint objects.
class HeightMap : public Buffer<float> {
  private:
    /// Distance between segments
    const int m_res;
    /// Maximum height of any point in this segment
    float m_max;
    /// Minimum height of any point in this segment
    float m_min;

  public:
    explicit HeightMap(unsigned int resolution);
    virtual ~HeightMap() = default;

    /// \brief Accessor for resolution of this segment.
    int getResolution() const {
        return m_res;
    }

    /// \brief Get the height at a relative integer position in the Segment.
    float get(int x, int z) const {
        return m_data[z * (m_res + 1) + x];
    }

    void getHeightAndNormal(float x, float z, float &h,
                    WFMath::Vector<3> &normal) const;
    void getHeight(float x, float z, float &h) const;

    /// \brief Accessor for the maximum height value in this Segment.
    float getMax() const { return m_max; }
    /// \brief Accessor for the minimum height value in this Segment.
    float getMin() const { return m_min; }

    void fill2d(const BasePoint& p1, const BasePoint& p2,
                const BasePoint& p3, const BasePoint& p4);

    void checkMaxMin(float h);

  private:

    void fill1d(const BasePoint& l, const BasePoint &h, float *array) const;

    float qRMD(WFMath::MTRand& rng, float nn, float fn, float ff, float nf,
               float roughness, float falloff, float depth) const;

};


} // namespace Mercator

#endif // MERCATOR_HEIGHTMAP_H
