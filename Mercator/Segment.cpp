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
#if 1
    // A somewhat wacky proof of concept interpolation
    for(int i = 0; i < m_res; ++i) {
        float near = nn + ((float)i / m_res) * (fn - nn);
        float far = nf + ((float)i / m_res) * (ff - nf);
        for(int j = 0; j < m_res; ++j) {
            m_points[j * m_res + i] = near + ((float)j / m_res) * (far - near);
        }
    }
#else
    float tmp[3 * m_res + 1][3 * m_res +1];
    tmp[0][0] = nn;
    tmp[0][m_res] = nn;
    tmp[m_res][0] = nn;
    tmp[m_res][m_res] = nn;

    tmp[m_res * 2][m_res] = fn;
    tmp[m_res * 3][m_res] = fn;
    tmp[m_res * 2][0] = fn;
    tmp[m_res * 3][0] = fn;

    tmp[m_res][m_res * 2] = nf;
    tmp[m_res][m_res * 3] = nf;
    tmp[0][m_res * 2] = nf;
    tmp[0][m_res * 3] = nf;

    tmp[m_res * 2][m_res * 2] = ff;
    tmp[m_res * 3][m_res * 2] = ff;
    tmp[m_res * 2][m_res * 3] = ff;
    tmp[m_res * 3][m_res * 3] = ff;

    for(int l = m_res, m = 1; l > 0; l >>= 1, m <<= 1) {
        for(int 0 = 1; i < (m * 3); i++) {
            for(int 0 = 1; j < (m * 3); j++) {
                float tot = tmp[j * l][i * l] +
                            tmp[j * l][(i + 1) * l] +
                            tmp[(j + 1) * l][i * l] +
                            tmp[(j + 1) * l][(i + 1) * l];
                tmp[j * l + l / 2][i * l + l / 2] = tot / 4;
            }
        }
    }

#endif
}

} // namespace Mercator
