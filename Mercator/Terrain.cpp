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
    for(int i = x - 2; i < x + 2; ++i) {
        for(int j = y - 2; j < y + 2; ++j) {
            remove(i, j);
        }
    }
}

void Terrain::refresh(int x, int y)
{
    for(int i = x - 2; i < x + 2; ++i) {
        for(int j = y - 2; j < y + 2; ++j) {
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

bool Terrain::getBasePoint(int x, int y, float & z)
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

void Terrain::getAvgBasePoint(int x, int y, float & z)
{
    float total = 0.f;
    int num = 0;
    for(int i = (x - 1); i < (x + 2); i++) {
        for(int j = (y - 1); j < (y + 2); j++) {
            float val;
            if (getBasePoint(i, j, val)) {
                total += val;
                ++num;
            }
        }
    }
    if (num == 0) {
        z = Terrain::defaultLevel; 
    } else {
        z = total / num;
    }
}

Segment * Terrain::getSegmentSafe(int x, int y, bool force)
{
    Segment * s = getSegmentQuik(x, y);
    if (s != 0) {
        return s;
    }
    float base[16];
    for(int i = 0; i < 16; ++i) { base[i] = Terrain::defaultLevel; }
    bool complete = getBasePoint(x,     y,     base[1*4 + 1]) &&
                    getBasePoint(x + 1, y,     base[1*4 + 2]) &&
                    getBasePoint(x,     y + 1, base[2*4 + 1]) &&
                    getBasePoint(x + 1, y + 1, base[2*4 + 2]);
    if (!getBasePoint(x - 1, y - 1, base[0*4 + 0])) {
        getAvgBasePoint(x - 1, y - 1, base[0*4 + 0]);
    }
    if (!getBasePoint(x - 1, y + 0, base[1*4 + 0])) {
        getAvgBasePoint(x - 1, y + 0, base[1*4 + 0]);
    }
    if (!getBasePoint(x - 1, y + 1, base[2*4 + 0])) {
        getAvgBasePoint(x - 1, y + 1, base[2*4 + 0]);
    }
    if (!getBasePoint(x - 1, y + 2, base[3*4 + 0])) {
        getAvgBasePoint(x - 1, y + 2, base[3*4 + 0]);
    }
    if (!getBasePoint(x + 0, y - 1, base[0*4 + 1])) {
        getAvgBasePoint(x + 0, y - 1, base[0*4 + 1]);
    }
    if (!getBasePoint(x + 0, y + 2, base[3*4 + 1])) {
        getAvgBasePoint(x + 0, y + 2, base[3*4 + 1]);
    }
    if (!getBasePoint(x + 1, y - 1, base[0*4 + 2])) {
        getAvgBasePoint(x + 1, y - 1, base[0*4 + 2]);
    }
    if (!getBasePoint(x + 1, y + 2, base[3*4 + 2])) {
        getAvgBasePoint(x + 1, y + 2, base[3*4 + 2]);
    }
    if (!getBasePoint(x + 2, y - 1, base[0*4 + 3])) {
        getAvgBasePoint(x + 2, y - 1, base[0*4 + 3]);
    }
    if (!getBasePoint(x + 2, y + 0, base[1*4 + 3])) {
        getAvgBasePoint(x + 2, y + 0, base[1*4 + 3]);
    }
    if (!getBasePoint(x + 2, y + 1, base[2*4 + 3])) {
        getAvgBasePoint(x + 2, y + 1, base[2*4 + 3]);
    }
    if (!getBasePoint(x + 2, y + 2, base[3*4 + 3])) {
        getAvgBasePoint(x + 2, y + 2, base[3*4 + 3]);
    }
    if (force || complete) {
        s = new Segment(m_res);
        s->populate(base); // , fn, ff, nf);
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
