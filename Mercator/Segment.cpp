// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "Segment.h"

#include "Terrain.h"

namespace Mercator {

Segment::Segment(int res) : m_res(res), m_points(new double[res * res])
{
}

void Segment::populate(double nn, double fn, double ff, double nf)
{
    // A somewhat wacky proof of concept interpolation
    for(int i = 0; i < m_res; ++i) {
        double near = nn + ((double)i / m_res) * (fn - nn);
        double far = nf + ((double)i / m_res) * (ff - nf);
        for(int j = 0; j < m_res; ++j) {
            m_points[j * m_res + i] = near + ((double)j / m_res) * (far - near);
        }
    }
}

} // namespace Mercator
