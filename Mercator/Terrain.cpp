// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Terrain.h>

#include <Mercator/Matrix.h>
#include <Mercator/Segment.h>

namespace Mercator {

void Terrain::invalidateSegment(int x, int y)
{
    Segmentstore::iterator I = m_segments.find(x);
    if (I == m_segments.end()) {
        return;
    }
    Segmentcolumn & column = I->second;
    Segmentcolumn::iterator J = column.find(y);
    if (J != column.end()) {
        J->second->invalidate();
        column.erase(J);
    }
}

void Terrain::invalidatePoint(int x, int y)
{
    for(int i = x - 1; i < x + 1; ++i) {
        for(int j = y - 1; j < y + 1; ++j) {
            invalidateSegment(x, y);
        }
    }
}

void Terrain::refresh(int x, int y)
{
    for(int i = x - 1; i < x + 1; ++i) {
        for(int j = y - 1; j < y + 2; ++j) {
            getSegmentSafe(i, j, false);
        }
    }
}

Terrain::Terrain(int res) : m_res(res)
{

}

float Terrain::get(float x, float y) const
{
    int ix = (int)floor(x / m_res);
    int iy = (int)floor(y / m_res);

    Segment * s = getSegmentQuik(ix, iy);
    if (s == 0) {
        return Terrain::defaultLevel;
    }
    return s->get((int)(x - (ix * m_res)), (int)(y - (iy * m_res)));
}

bool Terrain::getBasePoint(int x, int y, BasePoint& z)
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
    if ((s != 0) && s->isValid()) {
        return s;
    }
    
    Matrix<2, 2, BasePoint> base;
    bool complete = getBasePoint(x,     y,     base(0, 0)) &&
                    getBasePoint(x + 1, y,     base(1, 0)) &&
                    getBasePoint(x,     y + 1, base(0, 1)) &&
                    getBasePoint(x + 1, y + 1, base(1, 1));
    if (force || complete) {
        if (!s) {
            s = new Segment(m_res);
        }
        s->populate(base);
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

} // namespace Mercator
