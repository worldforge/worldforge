// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_TERRAIN_H
#define MERCATOR_TERRAIN_H

#include <map>

namespace Mercator {

class Segment;

class Terrain {
  private:
    const int m_res;

    std::map<int, std::map<int, double> > m_basePoints;
  public:
    static const double defaultLevel = 8;
    explicit Terrain(int res = 64);

    void setBasePoint(int x, int y, double z) {
        m_basePoints[x][y] = z;
    }

    Segment * getSegmentSafe(int x, int y);
    Segment * getSegmentQuik(int x, int y);
};

} // namespace Mercator

#endif // MERCATOR_TERRAIN_H
