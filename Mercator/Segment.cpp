// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Segment.h>

#include <Mercator/Terrain.h>

#include <Mercator/tile.h>

#include <iostream>

namespace Mercator {

static float fallOff = 0.15;
static float roughness = 1.5;

Segment::Segment(int res) : m_res(res), m_points(new float[(res+1) * (res+1)])
{
	m_max=m_min=0.0f;
}

#if 0
void Segment::populate(const Matrix<4, 4> & base)
{
#if 0
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
    tmp[0][0] = base(0, 0);
    tmp[0][m_res] = base(1, 0);
    tmp[m_res][0] = base(0, 1);
    tmp[m_res][m_res] = base(1, 1);

    tmp[m_res * 2][m_res] = base(1, 2);
    tmp[m_res * 3][m_res] = base(1, 3);
    tmp[m_res * 2][0] = base(0, 2);
    tmp[m_res * 3][0] = base(0, 3);

    tmp[m_res][m_res * 2] = base(2, 1);
    tmp[m_res][m_res * 3] = base(3, 1);
    tmp[0][m_res * 2] = base(2, 0);
    tmp[0][m_res * 3] = base(3, 0);

    tmp[m_res * 2][m_res * 2] = base(2, 2);
    tmp[m_res * 3][m_res * 2] = base(2, 3);
    tmp[m_res * 2][m_res * 3] = base(3, 2);
    tmp[m_res * 3][m_res * 3] = base(3, 3);

    for(int l = m_res, m = 1, n = 1; l > 1; l >>= 1, m <<= 1, ++n) {
        std::cout << "Pass " << m << " with level " << l << std::endl << std::flush;
        for(int i = 0; i < (m * 3); i++) {
            for(int j = 0; j < (m * 3); j++) {
                // float tot = tmp[j * l][i * l] +
                            // tmp[j * l][(i + 1) * l] +
                            // tmp[(j + 1) * l][i * l] +
                            // tmp[(j + 1) * l][(i + 1) * l];
                // tmp[j * l + l / 2][i * l + l / 2] = tot / 4;
                tmp[j * l + l / 2][i * l + l / 2] = qRMD(tmp[j * l][i * l],
                                                         tmp[j * l][(i + 1) * l],
                                                         tmp[(j + 1) * l][i * l],
                                                         tmp[(j + 1) * l][(i + 1) * l],
                                                         n);
            }
        }

        for(int i = 0; i < (m * 3); ++i) {
            for(int j = 1; j < (m * 3); ++j) {
                // float tot = tmp[j * l][i * l] +
                            // tmp[j * l][(i + 1) * l] +
                            // tmp[j * l + l / 2][i * l + l / 2] +
                            // tmp[j * l - l / 2][i * l + l / 2];
                // tmp[j * l][i * l + l / 2] = tot / 4;
                tmp[j * l][i * l + l / 2] = qRMD(tmp[j * l][i * l],
                                                 tmp[j * l][(i + 1) * l],
                                                 tmp[j * l + l / 2][i * l + l / 2],
                                                 tmp[j * l - l / 2][i * l + l / 2],
                                                 n);
            }
            if (i == 0) { continue; }
            for(int j = 0; j < (m * 3); ++j) {
                float tot = tmp[j * l][i * l] +
                            tmp[(j + 1) * l][i * l] +
                            tmp[j * l + l / 2][i * l - l / 2] +
                            tmp[j * l + l / 2][i * l + l / 2];
                tmp[j * l + l / 2][i * l] = tot / 4;
            }
        }
    }
    for(int i = 0; i < m_res + 1; ++i) {
        // for(int j = 0; j < m_res + 1; ++j) {
            // m_points[j * (m_res + 1) + i] = tmp[m_res + j][m_res + i];
        // }
        memcpy(&m_points[i * (m_res + 1)],
               &tmp[m_res + i][m_res],
               (m_res + 1) * sizeof(float));
    }

#endif
}
#else
void Segment::populate(const Matrix<4, 4> & base)
{
    tile(m_res, fallOff, roughness, base(1, 1), base(2, 1),
                                    base(2, 2), base(1, 2), m_points,
				    m_min, m_max);
}
#endif

float Segment::qRMD(float nn, float fn, float nf, float ff, int depth) const
{
    float tot = nn + fn + nf + ff;
    srand((unsigned int)::fabsf(tot));

    float heightDifference = std::max(std::max(nn, fn), std::max(nf, ff)) -
                             std::min(std::min(nn, fn), std::min(nf, ff));
    float displacement = ((float)rand()) / RAND_MAX - .5f;

    // std::cout << "D: " << tot / 4 << ":" << displacement << ":"
              // << heightDifference << std::endl << std::flush;
    return tot / 4.f + (displacement * heightDifference * roughness) / (::pow(depth,fallOff) + 1.f);
}

} // namespace Mercator
