// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "Terrain.h"

#include "Segment.h"

namespace Mercator {

void Terrain::remove(int x, int y)
{
    Segmentstore::iterator I = m_segments.find(x);
    if (I == m_segments.end()) {
        return;
    }
    Segmentcolumn & column = I->second;
    Segmentcolumn::iterator J = column.find(y);
    if (J != column.end()) {
        delete J->second;
        column.erase(J);
    }
    if (column.empty()) {
        m_segments.erase(I);
    }
}

void Terrain::invalidate(int x, int y)
{
    remove(x,   y);
    remove(x-1, y);
    remove(x,   y-1);
    remove(x-1, y-1);
}

Terrain::Terrain(int res) : m_res(res)
{

}

double Terrain::get(double x, double y) const
{
    int ix = (int)floor(x / m_res);
    int iy = (int)floor(y / m_res);

    Segment * s = getSegmentQuik(ix, iy);
    if (s == 0) {
        return Terrain::defaultLevel;
    }
    return s->get((int)(x - (ix * m_res)), (int)(y - (iy * m_res)));
}

bool Terrain::getBasePoint(int x, int y, double & z)
{
    Pointstore::const_iterator I = m_basePoints.find(x);
    if (I == m_basePoints.end()) {
        return false;
    }
    Pointcolumn::const_iterator J = I->second.find(y);
    if (J == I->second.end()) {
        return false;
    }
    z = J->second;
    return true;
}

Segment * Terrain::getSegmentSafe(int x, int y, bool force)
{
    Segment * s = getSegmentQuik(x, y);
    if (s != 0) {
        return s;
    }
    double nn = Terrain::defaultLevel, fn = Terrain::defaultLevel,
           ff = Terrain::defaultLevel, nf = Terrain::defaultLevel;
    bool complete = getBasePoint(x,     y,     nn) &&
                    getBasePoint(x + 1, y,     fn) &&
                    getBasePoint(x,     y + 1, nf) &&
                    getBasePoint(x + 1, y + 1, ff);
    if (force || complete) {
        s = new Segment(m_res);
        s->populate(nn, fn, ff, nf);
        m_segments[x][y] = s;
        return s;
    }
    return 0;
}

Segment * Terrain::getSegmentQuik(int x, int y) const
{
    Segmentstore::const_iterator I = m_segments.find(x);
    if (I == m_segments.end()) {
        return 0;
    }
    Segmentcolumn::const_iterator J = I->second.find(y);
    if (J == I->second.end()) {
        return 0;
    }
    return J->second;
}

void Terrain::refresh(int x, int y)
{
    getSegmentSafe(x,     y,     false);
    getSegmentSafe(x - 1, y,     false);
    getSegmentSafe(x,     y - 1, false);
    getSegmentSafe(x - 1, y - 1, false);
}

} // namespace Mercator
