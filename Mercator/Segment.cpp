// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "Segment.h"

#include "Terrain.h"

namespace Mercator {

Segment::Segment(int res) : m_res(res), m_points(new float[res * res])
{
}

void Segment::populate(float nn, float fn, float ff, float nf)
{
    // A somewhat wacky proof of concept interpolation
    for(int i = 0; i < m_res; ++i) {
        float near = nn + ((float)i / m_res) * (fn - nn);
        float far = nf + ((float)i / m_res) * (ff - nf);
        for(int j = 0; j < m_res; ++j) {
            m_points[j * m_res + i] = near + ((float)j / m_res) * (far - near);
        }
    }
}

} // namespace Mercator
