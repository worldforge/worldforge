// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_TERRAIN_H
#define MERCATOR_TERRAIN_H

#include <map>

#include <cmath>

namespace Mercator {

class Segment;

class Terrain {
  public:
    typedef std::map<int, double> Pointcolumn;
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
    static const double defaultLevel = 8;
    explicit Terrain(int res = 64);

    double get(double x, double y) const;

    bool getBasePoint(int x, int y, double & z);
    
    void setBasePoint(int x, int y, double z) {
        m_basePoints[x][y] = z;
        invalidate(x,y);
    }

    Segment * getSegmentSafe(double x, double y) {
        int ix = (int)floor(x / m_res);
        int iy = (int)floor(y / m_res);
        return getSegmentSafe(ix, iy);
    }

    Segment * getSegmentQuik(double x, double y) const {
        int ix = (int)floor(x / m_res);
        int iy = (int)floor(y / m_res);
        return getSegmentQuik(ix, iy);
    }

    const Segmentstore & getTerrain() const {
        return m_segments;
    }

    Segment * getSegmentSafe(int x, int y, bool force = true);
    Segment * getSegmentQuik(int x, int y) const;

    void refresh(int x, int y);
};

} // namespace Mercator

#endif // MERCATOR_TERRAIN_H
