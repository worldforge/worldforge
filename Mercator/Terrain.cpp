// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Terrain.h>

#include <Mercator/Matrix.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>

namespace Mercator {

Terrain::Terrain(unsigned int resolution) : m_res(resolution)
{

}

Terrain::~Terrain()
{
    // FIXME CLean up everything. Eliminate the leaks.
}

float Terrain::get(float x, float y) const
{
    int ix = (int)floor(x / m_res);
    int iy = (int)floor(y / m_res);

    Segment * s = getSegment(ix, iy);
    if ((s == 0) || (!s->isValid())) {
        return Terrain::defaultLevel;
    }
    return s->get((int)(x - (ix * m_res)), (int)(y - (iy * m_res)));
}

bool Terrain::getBasePoint(int x, int y, BasePoint& z) const
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

void Terrain::setBasePoint(int x, int y, const BasePoint& z)
{
    m_basePoints[x][y] = z;
    bool pointIsSet[3][3];
    BasePoint existingPoint[3][3];
    for(int i = x - 1, ri = 0; i < x + 2; ++i, ++ri) {
        for(int j = y - 1, rj = 0; j < y + 2; ++j, ++rj) {
            pointIsSet[ri][rj] = getBasePoint(i, j, existingPoint[ri][rj]);
        }
    }
    for(int i = x - 1, ri = 0; i < x + 1; ++i, ++ri) {
        for(int j = y - 1, rj = 0; j < y + 1; ++j, ++rj) {
            Segment * s = getSegment(i, j);
            if (s == 0) { 
                bool complete = pointIsSet[ri][rj] &&
                                pointIsSet[ri + 1][rj + 1] &&
                                pointIsSet[ri + 1][rj] &&
                                pointIsSet[ri][rj + 1];
                if (!complete) {
                    continue;
                }
                s = new Segment(m_res);
                s->setRef(i,j);
                Matrix<2, 2, BasePoint> & cp = s->getControlPoints();
                for(unsigned int k = 0; k < 2; ++k) {
                    for(unsigned int l = 0; l < 2; ++l) {
                        cp(k, l) = existingPoint[ri + k][rj + l];
                    }
                }
                m_segments[i][j] = s;
                continue;
            }
            s->setCornerPoint(ri ? 0 : 1, rj ? 0 : 1, z);
        }
    }
}

Segment * Terrain::getSegment(int x, int y) const
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

void Terrain::addMod(TerrainMod *t) {

    //work out which segments are overlapped by thus mod
    //note that the bbox is expanded by one grid unit because
    //segments share edges. this ensures a mod along an edge
    //will affect both segments.
    int lx=(int)floor((t->bbox().lowCorner()[0] - 1) / m_res);
    int ly=(int)floor((t->bbox().lowCorner()[1] - 1) / m_res);
    int hx=(int)ceil((t->bbox().highCorner()[0] + 1) / m_res);
    int hy=(int)ceil((t->bbox().highCorner()[1] + 1) / m_res);

    for (int i=lx;i<hx;++i) {
        for (int j=ly;j<hy;++j) {
            Segment *s=getSegment(i,j);
            if (s) s->addMod(t->clone());
        }
    }

    //Mods are cloned for each Segment, this prevents a leak
    //reference counting might be cleaner
    delete t;
}

} // namespace Mercator
