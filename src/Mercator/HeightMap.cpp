// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iround.h"

#include "HeightMap.h"
#include "Terrain.h"
#include "TerrainMod.h"
#include "Surface.h"
#include "BasePoint.h"
#include "Area.h"
#include "Shader.h"

#include <wfmath/MersenneTwister.h>

#include <cmath>
#include <cassert>
#include <algorithm>

namespace Mercator {

/// \brief Helper to interpolate on a line.
///
/// The line specified is of integer length, and the position specified
/// as an integer. A check is included to avoid calculation if the value
/// at each end is the same.
class LinInterp {
  private:
    /// The length of the line.
    float m_size;
    /// Flag indicating that both points have the same value.
    bool noCalc;
  public:
    /// Values at the two ends.
    float ep1, ep2;
    /// \brief Determine the interpolated value along the line.
    inline float calc(float loc)
    {
        return ((noCalc) ? ep1 :
                ((m_size-loc) * ep1 + loc * ep2));
    }
    /// \brief Constructor
    ///
    /// @param size length of the line.
    /// @param l value at one end of the line.
    /// @param h value at one end of the line.
    LinInterp(float size,float l, float h) : m_size(size), noCalc(false),
              ep1(l/size), ep2(h/size)
    {
        if (l==h) {
            ep1 = l;
            noCalc=true;
        }
    }
};

/// \brief Helper to interpolate in a quad.
///
/// The quad specified is assumed to be square of integer size, and
/// the position specified for interpolation is specified in integer
/// form. A check is included to avoid calculation if the value of each
/// corner is the same.
class QuadInterp {
  private:
    /// The length of one side of the square quad.
    float m_size;
    /// Flag indicating that all points have the same value.
    bool noCalc;
  public:
    /// Values at the four corners.
    float ep1, ep2, ep3, ep4;
    /// \brief Determine the interpolated value within the quad.
    inline float calc(float locX, float locY)
    {
        return  ((noCalc) ? ep1 :
                 (( ep1*(m_size-locX) + ep2 * locX) * (m_size-locY) +
                 ( ep4*(m_size-locX) + ep3 * locX) * (locY) ) / m_size );
    }
    /// \brief Constructor
    ///
    /// @param size length of one side of the square quad.
    /// @param e1 value at one corner of the square quad.
    /// @param e2 value at one corner of the square quad.
    /// @param e3 value at one corner of the square quad.
    /// @param e4 value at one corner of the square quad.
    QuadInterp(float size,float e1, float e2, float e3, float e4)
        : m_size(size), noCalc(false),
          ep1(e1/size), ep2(e2/size), ep3(e3/size), ep4(e4/size)
    {
        if ((e1==e2) && (e3==e4) && (e2==e3)) {
            ep1 = e1;
            noCalc=true;
        }
    }
};


/// \brief Construct an empty height map with the given resolution.
HeightMap::HeightMap(unsigned int resolution) :
        Buffer<float>::Buffer(resolution + 1, 1),
                            m_res(resolution),
                            m_max(std::numeric_limits<float>::lowest()),
                            m_min(std::numeric_limits<float>::max())
{
}


/// \brief Check a value against m_min and m_max and set one of them
/// if appropriate.
///
/// Called by internal functions whenever a new data point is generated.
void HeightMap::checkMaxMin(float h)
{
    if (h<m_min) {
        m_min=h;
    }
    if (h>m_max) {
        m_max=h;
    }
}

// generate a rand num between -0.5...0.5
inline float randHalf(WFMath::MTRand& rng)
{
    //return (float) rand() / RAND_MAX - 0.5f;
    return rng.rand<float>() - 0.5f;
}


/// \brief quasi-Random Midpoint Displacement (qRMD) algorithm.
float HeightMap::qRMD(WFMath::MTRand& rng, float nn, float fn, float ff, float nf,
                    float roughness, float falloff, float depth) const
{
    float max = std::max(std::max(nn, fn), std::max(nf, ff)),
          min = std::min(std::min(nn, fn), std::min(nf, ff)),
          heightDifference = max - min;

    return ((nn+fn+ff+nf)/4.f) + randHalf(rng) * roughness * heightDifference / (1.f+std::pow(depth,falloff));
}

/// \brief One dimensional midpoint displacement fractal.
///
/// Size must be a power of 2.
/// Falloff is the decay of displacement as the fractal is refined.
/// Array is size + 1 long. array[0] and array[size] are filled
/// with the control points for the fractal.
void HeightMap::fill1d(const BasePoint& l, const BasePoint &h,
                     float *array) const
{
    array[0] = l.height();
    array[m_res] = h.height();
    LinInterp li(m_res, l.roughness(), h.roughness());

    // seed the RNG.
    // The RNG is seeded only once for the line and the seed is based on the
    // two endpoints -because they are the common parameters for two adjoining
    // tiles
    //srand((l.seed() * 1000 + h.seed()));
    WFMath::MTRand::uint32 seed[2]={ l.seed(), h.seed() };
    WFMath::MTRand rng(seed, 2);

    // stride is used to step across the array in a deterministic fashion
    // effectively we do the 1/2  point, then the 1/4 points, then the 1/8th
    // points etc. this has to be the same order every time because we call
    // on the RNG at every point
    int stride = m_res/2;

    // depth is used to indicate what level we are on. the displacement is
    // reduced each time we traverse the array.
    float depth=1;

    while (stride) {
        for (int i=stride;i<m_res;i+=stride*2) {
            float hh = array[i-stride];
            float lh = array[i+stride];
            float hd = std::fabs(hh-lh);
            float roughness = li.calc(i);

            //eliminate the problem where hd is nearly zero, leaving a flat section.
            if ((hd*100.f) < roughness) {
                hd+=0.05f * roughness;
            }

            array[i] = ((hh+lh)/2.f) + randHalf(rng) * roughness  * hd / (1.f+std::pow(depth,BasePoint::FALLOFF));
        }
        stride >>= 1;
        depth++;
    }
}

/// \brief Two dimensional midpoint displacement fractal.
///
/// For a tile where edges are to be filled by 1d fractals.
/// Size must be a power of 2, array is (size + 1) * (size + 1) with the
/// corners the control points.
void HeightMap::fill2d(const BasePoint& p1, const BasePoint& p2,
                     const BasePoint& p3, const BasePoint& p4)
{
    //First reset the min and max values, since they will be updated.
    m_max = std::numeric_limits<float>::lowest();
    m_min = std::numeric_limits<float>::max();

    // calculate the edges first. This is necessary so that segments tile
    // seamlessly note the order in which the edges are calculated and the
    // direction. opposite edges are calculated the same way (eg left->right)
    // so that the top of one tile matches the bottom of another, likewise
    // with sides.

    // temporary array used to hold each edge
    std::vector<float> edgeData;
    edgeData.reserve(m_size);
    float* edge = edgeData.data();

    float* points = m_data;

    // calc top edge and copy into m_heightMap
    fill1d(p1,p2,edge);
    for (int i=0;i<=m_res;i++) {
        points[0*m_size + i] = edge[i];
        checkMaxMin(edge[i]);
    }

    // calc left edge and copy into points
    fill1d(p1,p4,edge);
    for (int i=0;i<=m_res;i++) {
        points[i*m_size + 0] = edge[i];
        checkMaxMin(edge[i]);
    }

    // calc right edge and copy into points
    fill1d(p2,p3,edge);
    for (int i=0;i<=m_res;i++) {
        points[i*m_size + m_res] = edge[i];
        checkMaxMin(edge[i]);
    }

    // calc bottom edge and copy into points
    fill1d(p4,p3,edge);
    for (int i=0;i<=m_res;i++) {
        points[m_res*m_size + i] = edge[i];
        checkMaxMin(edge[i]);
    }

    // seed the RNG - this is the 5th and last seeding for the tile.
    // it was seeded once for each edge, now once for the tile.
    //srand(p1.seed()*20 + p2.seed()*15 + p3.seed()*10 + p4.seed()*5);
    WFMath::MTRand::uint32 seed[4]={ p1.seed(), p2.seed(), p3.seed(), p4.seed() };
    WFMath::MTRand rng(seed, 4);

    QuadInterp qi(m_res, p1.roughness(), p2.roughness(), p3.roughness(), p4.roughness());
    QuadInterp falloffQi(m_res, p1.falloff(), p2.falloff(), p3.falloff(), p4.falloff());

    float depth=0;

    // center of points is done separately
    int stride = m_res/2;

    //float roughness = (p1.roughness+p2.roughness+p3.roughness+p4.roughness)/(4.0f);
    float roughness = qi.calc(stride, stride);
    float f = falloffQi.calc(stride, stride);
    points[stride*m_size + stride] = qRMD(rng, points[0 * m_size + stride],
                                        points[stride*m_size + 0],
                                        points[stride*m_size + m_res],
                                        points[m_res*m_size + stride],
                                        roughness,
                                        f, depth);


    checkMaxMin(points[stride*m_size + stride]);

    stride >>= 1;

    // skip across the points and fill in the points
    // alternate cross and plus shapes.
    // this is a diamond-square algorithm.
    while (stride) {
      //Cross shape - + contributes to value at X
      //+ . +
      //. X .
      //+ . +
      for (int i=stride;i<m_res;i+=stride*2) {
          for (int j=stride;j<m_res;j+=stride*2) {
              roughness=qi.calc(i,j);
              f = falloffQi.calc(i, j);
              points[j*m_size + i] = qRMD(rng, points[(i-stride) + (j+stride) * (m_size)],
                                       points[(i+stride) + (j-stride) * (m_size)],
                                       points[(i+stride) + (j+stride) * (m_size)],
                                       points[(i-stride) + (j-stride) * (m_size)],
                                       roughness, f, depth);
              checkMaxMin(points[j*m_size + i]);
          }
      }

      depth++;
      //Plus shape - + contributes to value at X
      //. + .
      //+ X +
      //. + .
      for (int i=stride*2;i<m_res;i+=stride*2) {
          for (int j=stride;j<m_res;j+=stride*2) {
              roughness=qi.calc(i,j);
              f = falloffQi.calc(i, j);
              points[j*m_size + i] = qRMD(rng, points[(i-stride) + (j) * (m_size)],
                                       points[(i+stride) + (j) * (m_size)],
                                       points[(i) + (j+stride) * (m_size)],
                                       points[(i) + (j-stride) * (m_size)],
                                       roughness, f , depth);
              checkMaxMin(points[j*m_size + i]);
          }
      }

      for (int i=stride;i<m_res;i+=stride*2) {
          for (int j=stride*2;j<m_res;j+=stride*2) {
              roughness=qi.calc(i,j);
              f = falloffQi.calc(i, j);
              points[j*m_size + i] = qRMD(rng, points[(i-stride) + (j) * (m_size)],
                                       points[(i+stride) + (j) * (m_size)],
                                       points[(i) + (j+stride) * (m_size)],
                                       points[(i) + (j-stride) * (m_size)],
                                       roughness, f, depth);
              checkMaxMin(points[j*m_size + i]);
          }
      }

      stride>>=1;
      depth++;
    }
}

void HeightMap::getHeight(float x, float z, float &h) const
{
    // FIXME this ignores edges and corners
    assert(x <= m_res);
    assert(x >= 0.0f);
    assert(z <= m_res);
    assert(z >= 0.0f);

    // get index of the actual tile in the segment
    int tile_x = I_ROUND(std::floor(x));
    int tile_z = I_ROUND(std::floor(z));

    // work out the offset into that tile
    float off_x = x - tile_x;
    float off_z = z - tile_z;

    float h1=get(tile_x, tile_z);
    float h2=get(tile_x, tile_z+1);
    float h3=get(tile_x+1, tile_z+1);
    float h4=get(tile_x+1, tile_z);

    // square is broken into two triangles
    // top triangle |/
    if ((off_x - off_z) <= 0.f) {
        h = h1 + (h3-h2) * off_x + (h2-h1) * off_z;
    }
    // bottom triangle /|
    else {
        h = h1 + (h4-h1) * off_x + (h3-h4) * off_z;
    }
}


/// \brief Get an accurate height and normal vector at a given coordinate
/// relative to this segment.
///
/// The height and surface normal are determined by finding the four adjacent
/// height points nearest to the coordinate, and interpolating between
/// those height values. The square area defined by the 4 height points is
/// considered as two triangles for the purposes of interpolation to ensure
/// that the calculated height falls on the surface rendered by a 3D
/// graphics engine from the same heightfield data. The line used to
/// divide the area is defined by the gradient z = x, so the first
/// triangle has relative vertex coordinates (0,0) (1,0) (1,1) and
/// the second triangle has vertex coordinates (0,0) (0,1) (1,1).
void HeightMap::getHeightAndNormal(float x, float z, float& h,
                                 WFMath::Vector<3> &normal) const
{
    // FIXME this ignores edges and corners
    assert(x <= m_res);
    assert(x >= 0.0f);
    assert(z <= m_res);
    assert(z >= 0.0f);
    
    // get index of the actual tile in the segment
    int tile_x = I_ROUND(std::floor(x));
    int tile_z = I_ROUND(std::floor(z));

    // work out the offset into that tile
    float off_x = x - tile_x;
    float off_z = z - tile_z;
 
    float h1=get(tile_x, tile_z);
    float h2=get(tile_x, tile_z+1);
    float h3=get(tile_x+1, tile_z+1);
    float h4=get(tile_x+1, tile_z);

    // square is broken into two triangles
    // top triangle |/
    if ((off_x - off_z) <= 0.f) {
        normal = WFMath::Vector<3>(h2-h3, 1.0f, h1-h2);

        //normal for intersection of both triangles
        if (off_x == off_z) {
            normal += WFMath::Vector<3>(h1-h4, 1.0f, h4-h3);
        }
        normal.normalize();
        h = h1 + (h3-h2) * off_x + (h2-h1) * off_z;
    } 
    // bottom triangle /|
    else {
        normal = WFMath::Vector<3>(h1-h4, 1.0f, h4-h3);
        normal.normalize();
        h = h1 + (h4-h1) * off_x + (h3-h4) * off_z;
    }
}

} // namespace Mercator
