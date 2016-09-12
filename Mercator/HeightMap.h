// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#ifndef MERCATOR_HEIGHTMAP_H
#define MERCATOR_HEIGHTMAP_H

#include "Mercator.h"
#include "BasePoint.h"

#include <wfmath/vector.h>
#include <wfmath/axisbox.h>

namespace WFMath {
class MTRand;
}

namespace Mercator {

// This class will need to be reference counted if we want the code to
// be able to hold onto it, as currently they get deleted internally
// whenever height points are asserted.

/// \brief Class storing heightfield and other data for a single fixed size
/// square area of terrain defined by four adjacent BasePoint objects.
class HeightMap {
  private:
    /// Distance between segments
    const int m_res;
    /// Size of segment, m_res + 1
    const int m_size;
    /// Pointer to buffer containing height points
    float * m_points;
    /// Maximum height of any point in this segment
    float m_max;
    /// Minimum height of any point in this segment
    float m_min;

  public:
    explicit HeightMap(unsigned int resolution, float min, float max);
    ~HeightMap();

    /// \brief Accessor for resolution of this segment.
    const int getResolution() const {
        return m_res;
    }

    /// \brief Accessor for array size of this segment.
    const int getSize() const {
        return m_size;
    }

    /// \brief Accessor for buffer containing height points.
    const float * getPoints() const {
        return m_points;
    }

    /// \brief Accessor for write access to buffer containing height points.
    float * getPoints() {
        return m_points;
    }

    /// \brief Get the height at a relative integer position in the Segment.
    float get(int x, int y) const {
        return m_points[y * (m_res + 1) + x];
    }

    void getHeightAndNormal(float x, float y, float &h, 
                    WFMath::Vector<3> &normal) const;

    /// \brief Accessor for the maximum height value in this Segment.
    float getMax() const { return m_max; }
    /// \brief Accessor for the minimum height value in this Segment.
    float getMin() const { return m_min; }

    /// \brief The 2d area covered by this segment
    WFMath::AxisBox<2> getRect() const;

    /// \brief The 3d box covered by this segment
    WFMath::AxisBox<3> getBox() const;

    void fill2d(const BasePoint& p1, const BasePoint& p2,
                const BasePoint& p3, const BasePoint& p4);

  private:
    void checkMaxMin(float h);

    void fill1d(const BasePoint& l, const BasePoint &h, float *array) const;

    float qRMD(WFMath::MTRand& rng, float nn, float fn, float ff, float nf,
               float roughness, float falloff, float depth) const;

};

} // namespace Mercator

#endif // MERCATOR_HEIGHTMAP_H
