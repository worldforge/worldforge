// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_TERRAIN_H
#define MERCATOR_TERRAIN_H

#include <map>
#include <cmath>

#include <Mercator/BasePoint.h>

namespace Mercator {

class Segment;

class Terrain {
  public:
    typedef std::map<int, BasePoint> Pointcolumn;
    typedef std::map<int, Segment *> Segmentcolumn;

    typedef std::map<int, Pointcolumn > Pointstore;
    typedef std::map<int, Segmentcolumn > Segmentstore;

  private:
    const int m_res;

    Pointstore m_basePoints;
    Segmentstore m_segments;

    void remove(int x, int y);
    void invalidate(int x, int y);
  public:
    static const float defaultLevel = 8;
    explicit Terrain(int res = 64);

    float get(float x, float y) const;

    bool getBasePoint(int x, int y, BasePoint& z);
    
    void setBasePoint(int x, int y, const BasePoint& z) {
        m_basePoints[x][y] = z;
        invalidate(x,y);
    }

    Segment * getSegmentSafe(float x, float y) {
        int ix = (int)floor(x / m_res);
        int iy = (int)floor(y / m_res);
        return getSegmentSafe(ix, iy);
    }

    Segment * getSegmentQuik(float x, float y) const {
        int ix = (int)floor(x / m_res);
        int iy = (int)floor(y / m_res);
        return getSegmentQuik(ix, iy);
    }

    const int getRes() const {
        return m_res;
    }

    const Segmentstore & getTerrain() const {
        return m_segments;
    }

    const Pointstore & getPoints() const {
        return m_basePoints;
    }

    Segment * getSegmentSafe(int x, int y, bool force = true);
    Segment * getSegmentQuik(int x, int y) const;

    void refresh(int x, int y);

};

} // namespace Mercator

#endif // MERCATOR_TERRAIN_H
