// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iround.h"

#include <Mercator/Segment.h>
#include <Mercator/Terrain.h>
#include <Mercator/TerrainMod.h>
#include <Mercator/Surface.h>
#include <Mercator/BasePoint.h>
#include <Mercator/Area.h>
#include <Mercator/Shader.h>

#include <wfmath/MersenneTwister.h>

#include <cmath>
#include <cassert>

namespace Mercator {


/// \brief Helper to interpolate on a line.
///
/// The line specified is of integer length, and the position specified
/// as an integer. A check is included to avoid calculation if the value
/// at each end is the same.
class LinInterp {
  private:
    /// The length of the line.
    int m_size;
    /// Flag indicating that both points have the same value.
    bool noCalc;
  public:
    /// Values at the two ends.
    float ep1, ep2;
    /// \brief Determine the interpolated value along the line.
    inline float calc(int loc) 
    {
        return ((noCalc) ? ep1 : ((m_size-loc) * ep1 + loc * ep2));
    }
    /// \brief Constructor
    ///
    /// @param size length of the line.
    /// @param l value at one end of the line.
    /// @param h value at one end of the line.
    LinInterp(int size,float l, float h) : m_size(size), noCalc(false), 
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
    int m_size;
    /// Flag indicating that all points have the same value.
    bool noCalc;
  public:
    /// Values at the four corners.
    float ep1, ep2, ep3, ep4;
    /// \brief Determine the interpolated value within the quad.
    inline float calc(int locX, int locY) 
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
    QuadInterp(int size,float e1, float e2, float e3, float e4)
        : m_size(size), noCalc(false),
          ep1(e1/size), ep2(e2/size), ep3(e3/size), ep4(e4/size) 
    {
        if ((e1==e2) && (e3==e4) && (e2==e3)) {
            ep1 = e1;
            noCalc=true;
        }
    } 
};      

/// \brief Construct an empty segment with the given resolution.
///
/// Generally it is not necessary to call this from outside the Mercator
/// library Segment objects are created as required. The Segment is
/// constructed without allocating any storage for heightfield or surface
/// normal data. The m_min and m_max members are initialised to extreme
/// values, and should be set to appropriate using setMinMax() as soon as
/// possible after construction. Similarly the control points should be
/// set soon after construction.
Segment::Segment(int x, int y, unsigned int resolution) :
                            m_res(resolution), m_size(m_res+1),
                            m_xRef(x), m_yRef(y),
                            m_points(0), m_normals(0),
                            m_max(-1000000.f), m_min(1000000.0f)
{
}

/// \brief Destruct the Segment.
///
/// Generally it is not necessary to delete Segment objects from application
/// code, as Segment instances are owned by the Terrain object.
/// Storage allocated for heightfield and surface normals is implicitly
/// deleted as well as all surfaces.
Segment::~Segment()
{
    clearMods();
    if (m_points != 0) {
        delete [] m_points;
    }
    if (m_normals != 0) {
        delete [] m_normals;
    }
    
    Segment::Surfacestore::const_iterator I = m_surfaces.begin();
    Segment::Surfacestore::const_iterator Iend = m_surfaces.end();
    for(; I != Iend; ++I) {
        delete I->second;
    }
    
}

/// \brief Populate the Segment with heightfield data.
///
/// Storage for the heightfield data is allocated if necessary, the 
/// qRMD algorithm is used to calculate the heightfield data, and
/// required modifications are applied.
void Segment::populate() // const Matrix<2, 2, BasePoint> & base)
{
    if (m_points == 0) {
        m_points = new float[m_size * m_size];
    }
    fill2d(m_controlPoints(0, 0), m_controlPoints(1, 0), 
           m_controlPoints(1, 1), m_controlPoints(0, 1));

    ModList::iterator I = m_modList.begin();
    ModList::iterator Iend = m_modList.end();
    for (; I != Iend; ++I) {
        applyMod(*I);
    }
}

/// \brief Mark the contents of this Segment as stale.
///
/// This is called internally whenever changes occur that mean that the
/// heightfield and surface normal data are no longer valid.
/// If surface normal storage is deallocated, and if the points argument
/// is true the heightfield storage is also deallocated.
void Segment::invalidate(bool points)
{
    if (points && m_points != 0) {
        delete [] m_points;
        m_points = 0;
    }
    if (m_normals != 0) {
        delete [] m_normals;
        m_normals = 0;
    }

    invalidateSurfaces();
}

/// \brief Mark surfaces as stale.
///
/// This is called internally from Segment::invalidate() when changes occur
/// that mean the surface data is no longer valid. The Surface::invalidate()
/// method is called for each surface.
void Segment::invalidateSurfaces()
{
    Segment::Surfacestore::const_iterator I = m_surfaces.begin();
    Segment::Surfacestore::const_iterator Iend = m_surfaces.end();
    for(; I != Iend; ++I) {
        I->second->invalidate();
    }
}

/// \brief Populate the Segment with surface normal data.
///
/// Storage for the normals is allocated if necessary, and the average
/// normal at each heightpoint is calculated. The middle normals are
/// calculated first, followed by the boundaries which are done in
/// 2 dimensions to ensure that there is no visible seam between segments.
void Segment::populateNormals()
{
    assert(m_points != NULL);

    if (m_normals == 0) {
        m_normals = new float[m_size * m_size * 3];
    }

    float * np = m_normals;
    
    // Fill in the damn normals
    float h1,h2,h3,h4;
    for (int j = 1; j < m_res; ++j) {
        for (int i = 1; i < m_res; ++i) {
           h1 = get(i - 1, j);
           h2 = get(i, j + 1);
           h3 = get(i + 1, j);
           h4 = get(i, j - 1);
           
           // Caclulate the normal vector.
           np[j * m_size * 3 + i * 3]     = (h1 - h3) / 2.f;
           np[j * m_size * 3 + i * 3 + 1] = (h4 - h2) / 2.f;
           np[j * m_size * 3 + i * 3 + 2] = 1.0;
        }
    }

    //edges have one axis pegged to 0
    
    //top and bottom boundary
    for (int i=1; i < m_res; ++i) {
        h1 = get(i - 1, 0);
        h2 = get(i + 1, 0);
        
        np[i * 3]     = (h1 - h2) / 2.f;
        np[i * 3 + 1] = 0.0;
        np[i * 3 + 2] = 1.0;
 
        h1 = get(i - 1, m_res);
        h2 = get(i + 1, m_res);
        
        np[m_res * m_size * 3 + i * 3]     = (h1 - h2) / 2.f;
        np[m_res * m_size * 3 + i * 3 + 1] = 0.0f;
        np[m_res * m_size * 3 + i * 3 + 2] = 1.0f;
    }
    
    //left and right boundary
    for (int j=1; j < m_res; ++j) {
        h1 = get(0, j - 1);
        h2 = get(0, j + 1);
        
        np[j * m_size * 3]     = 0;
        np[j * m_size * 3 + 1] = (h1 - h2) / 2.f;
        np[j * m_size * 3 + 2] = 1.f;
 
        h1 = get(m_res, j - 1);
        h2 = get(m_res, j + 1);

        np[j * m_size * 3 + m_res * 3]     = 0.f;
        np[j * m_size * 3 + m_res * 3 + 1] = (h1 - h2) / 2.f;
        np[j * m_size * 3 + m_res * 3 + 2] = 1.f;
    }

    //corners - these are all treated as flat
    //so the normal points straight up
    np[0] = 0.f;
    np[1] = 0.f;
    np[2] = 1.f;

    np[m_res * m_size * 3]     = 0.f;
    np[m_res * m_size * 3 + 1] = 0.f;
    np[m_res * m_size * 3 + 2] = 1.f;

    np[m_res * 3]     = 0.f;
    np[m_res * 3 + 1] = 0.f;
    np[m_res * 3 + 2] = 1.f;
    
    np[m_res * m_size * 3 + m_res * 3]     = 0.f;
    np[m_res * m_size * 3 + m_res * 3 + 1] = 0.f;
    np[m_res * m_size * 3 + m_res * 3 + 2] = 1.f;
}

/// \brief Populate the surfaces associated with this Segment.
///
/// Call Surface::populate() for each Surface in turn.
void Segment::populateSurfaces()
{
    Surfacestore::const_iterator I = m_surfaces.begin();
    Surfacestore::const_iterator Iend = m_surfaces.end();

    for (; I != Iend; ++I) {
        I->second->populate();
    }
}


// generate a rand num between -0.5...0.5
inline float randHalf(WFMath::MTRand& rng)
{
    //return (float) rand() / RAND_MAX - 0.5f;
    return rng() - 0.5;
}


/// \brief quasi-Random Midpoint Displacement (qRMD) algorithm.
float Segment::qRMD(WFMath::MTRand& rng, float nn, float fn, float ff, float nf,
                    float roughness, float falloff, float depth) const
{
    float max = std::max(std::max(nn, fn), std::max(nf, ff)),
          min = std::min(std::min(nn, fn), std::min(nf, ff)),
          heightDifference = max - min;
 
    return ((nn+fn+ff+nf)/4.f) + randHalf(rng) * roughness * heightDifference / (1.f+std::pow(depth,falloff));
}

/// \brief Check a value against m_min and m_max and set one of them
/// if appropriate.
///
/// Called by internal functions whenever a new data point is generated.
inline void Segment::checkMaxMin(float h)
{
    if (h<m_min) {
        m_min=h;
    }
    if (h>m_max) {
        m_max=h;
    }
}

/// \brief One dimensional midpoint displacement fractal.
///
/// Size must be a power of 2.
/// Falloff is the decay of displacement as the fractal is refined.
/// Array is size + 1 long. array[0] and array[size] are filled
/// with the control points for the fractal.
void Segment::fill1d(const BasePoint& l, const BasePoint &h, 
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
void Segment::fill2d(const BasePoint& p1, const BasePoint& p2, 
                     const BasePoint& p3, const BasePoint& p4)
{
    assert(m_points!=0);
    
    // int line = m_res+1;
    
    // calculate the edges first. This is necessary so that segments tile
    // seamlessly note the order in which the edges are calculated and the
    // direction. opposite edges are calculated the same way (eg left->right)
    // so that the top of one tile matches the bottom of another, likewise
    // with sides.
    
    // temporary array used to hold each edge
    float * edge = new float[m_size];
    
    // calc top edge and copy into m_points
    fill1d(p1,p2,edge);
    for (int i=0;i<=m_res;i++) {
        m_points[0*m_size + i] = edge[i];
        checkMaxMin(edge[i]);
    }

    // calc left edge and copy into m_points
    fill1d(p1,p4,edge);
    for (int i=0;i<=m_res;i++) {
        m_points[i*m_size + 0] = edge[i];
        checkMaxMin(edge[i]);
    }
   
    // calc right edge and copy into m_points
    fill1d(p2,p3,edge);
    for (int i=0;i<=m_res;i++) {
        m_points[i*m_size + m_res] = edge[i];
        checkMaxMin(edge[i]);
    }

    // calc bottom edge and copy into m_points
    fill1d(p4,p3,edge);
    for (int i=0;i<=m_res;i++) {
        m_points[m_res*m_size + i] = edge[i];
        checkMaxMin(edge[i]);
    }
    
    // seed the RNG - this is the 5th and last seeding for the tile.
    // it was seeded once for each edge, now once for the tile.
    //srand(p1.seed()*20 + p2.seed()*15 + p3.seed()*10 + p4.seed()*5);
    WFMath::MTRand::uint32 seed[4]={ p1.seed(), p2.seed(), p3.seed(), p4.seed() };
    WFMath::MTRand rng(seed, 4);

    QuadInterp qi(m_res, p1.roughness(), p2.roughness(), p3.roughness(), p4.roughness());

    float f = BasePoint::FALLOFF;
    float depth=0;
    
    // center of m_points is done separately
    int stride = m_res/2;

    //float roughness = (p1.roughness+p2.roughness+p3.roughness+p4.roughness)/(4.0f);
    float roughness = qi.calc(stride, stride);
    m_points[stride*m_size + stride] = qRMD(rng, m_points[0 * m_size + stride],
                                        m_points[stride*m_size + 0],
                                        m_points[stride*m_size + m_res],
                                        m_points[m_res*m_size + stride],
                                        roughness,
                                        f, depth);
                    

    checkMaxMin(m_points[stride*m_size + stride]);

    stride >>= 1;

    // skip across the m_points and fill in the points
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
              m_points[j*m_size + i] = qRMD(rng, m_points[(i-stride) + (j+stride) * (m_size)],
                                       m_points[(i+stride) + (j-stride) * (m_size)],
                                       m_points[(i+stride) + (j+stride) * (m_size)],
                                       m_points[(i-stride) + (j-stride) * (m_size)],
                                       roughness, f, depth);
              checkMaxMin(m_points[j*m_size + i]);
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
              m_points[j*m_size + i] = qRMD(rng, m_points[(i-stride) + (j) * (m_size)],
                                       m_points[(i+stride) + (j) * (m_size)],
                                       m_points[(i) + (j+stride) * (m_size)],
                                       m_points[(i) + (j-stride) * (m_size)], 
                                       roughness, f , depth);
              checkMaxMin(m_points[j*m_size + i]);
          }
      }
               
      for (int i=stride;i<m_res;i+=stride*2) {
          for (int j=stride*2;j<m_res;j+=stride*2) {
              roughness=qi.calc(i,j);
              m_points[j*m_size + i] = qRMD(rng, m_points[(i-stride) + (j) * (m_size)],
                                       m_points[(i+stride) + (j) * (m_size)],
                                       m_points[(i) + (j+stride) * (m_size)],
                                       m_points[(i) + (j-stride) * (m_size)],
                                       roughness, f, depth);
              checkMaxMin(m_points[j*m_size + i]);
          }
      }

      stride>>=1;
      depth++;
    }
    delete [] edge;
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
/// divide the area is defined by the gradient y = x, so the first
/// triangle has relative vertex coordinates (0,0) (1,0) (1,1) and
/// the second triangle has vertex coordinates (0,0) (0,1) (1,1).
void Segment::getHeightAndNormal(float x, float y, float& h,
                                 WFMath::Vector<3> &normal) const
{
    // FIXME this ignores edges and corners
    assert(x <= m_res);
    assert(x >= 0.0f);
    assert(y <= m_res);
    assert(y >= 0.0f);
    
    // get index of the actual tile in the segment
    int tile_x = (int)F_FLOOR(x);
    int tile_y = (int)F_FLOOR(y);

    // work out the offset into that tile
    float off_x = x - tile_x;
    float off_y = y - tile_y;
 
    float h1=get(tile_x, tile_y);
    float h2=get(tile_x, tile_y+1);
    float h3=get(tile_x+1, tile_y+1);
    float h4=get(tile_x+1, tile_y);

    // square is broken into two triangles
    // top triangle |/
    if ((off_x - off_y) <= 0.f) {
        normal = WFMath::Vector<3>(h2-h3, h1-h2, 1.0f);

        //normal for intersection of both triangles
        if (off_x == off_y) {
            normal += WFMath::Vector<3>(h1-h4, h4-h3, 1.0f);
        }
        normal.normalize();
        h = h1 + (h3-h2) * off_x + (h2-h1) * off_y;
    } 
    // bottom triangle /|
    else {
        normal = WFMath::Vector<3>(h1-h4, h4-h3, 1.0f);
        normal.normalize();
        h = h1 + (h4-h1) * off_x + (h3-h4) * off_y;
    }
}

/// \brief Determine the intersection between an axis aligned box and
/// this segment.
///
/// @param bbox axis aligned box to be tested.
/// @param lx lower x coordinate of intersection area.
/// @param hx upper x coordinate of intersection area.
/// @param ly lower y coordinate of intersection area.
/// @param hy upper y coordinate of intersection area.
/// @return true if the box intersects with this Segment, false otherwise.
bool Segment::clipToSegment(const WFMath::AxisBox<2> &bbox,
                            int &lx, int &hx, int &ly, int &hy) const
{
    lx = I_ROUND(bbox.lowCorner()[0]); 
    if (lx > m_res) return false;
    if (lx < 0) lx = 0;
    
    hx = I_ROUND(bbox.highCorner()[0]); 
    if (hx < 0) return false;
    if (hx > m_res) hx = m_res;
    
    ly = I_ROUND(bbox.lowCorner()[1]); 
    if (ly > m_res) return false;
    if (ly < 0) ly = 0;
    
    hy = I_ROUND(bbox.highCorner()[1]); 
    if (hy < 0) return false;
    if (hy > m_res) hy = m_res;

    return true;
}

/// \brief Add a TerrainMod to this Segment.
///
/// Called from Terrain::addMod(). If this point data is already valid,
/// the modification will be applied directly.
int Segment::addMod(const TerrainMod *t) 
{
    m_modList.insert(t);
    invalidate();
    return 0;
}

/// \brief Update a TerrainMod in this Segment.
///
/// Called from Terrain::removeMod().
int Segment::updateMod(const TerrainMod * tm)
{
    // FIXME Are we really removing it?
    ModList::const_iterator I = m_modList.find(tm);
    if (I != m_modList.end()) {
        invalidate();
        return 0;
    }
    return -1;
}


/// \brief Remove a TerrainMod from this Segment.
///
/// Called from Terrain::removeMod().
int Segment::removeMod(const TerrainMod * tm)
{
    // FIXME Are we really removing it?
    ModList::iterator I = m_modList.find(tm);
    if (I != m_modList.end()) {
        m_modList.erase(I);
        invalidate();
        return 0;
    }
    return -1;
}

/// \brief Delete all the modifications applied to this Segment.
///
/// Usually called from the destructor. It is not normally necessary to call
/// this function from the application.
void Segment::clearMods() 
{
    if (m_modList.size() != 0) {
        m_modList.clear();
        invalidate();
    }
}

/// \brief Modify the heightfield data using the TerrainMod objects which
/// are attached to this Segment.
///
/// Usually called from Segment::populate(). It is not normally necessary to
/// call this function from the application.
void Segment::applyMod(const TerrainMod *t) 
{
    int lx,hx,ly,hy;
    WFMath::AxisBox<2> bbox=t->bbox();
    bbox.shift(WFMath::Vector<2>(-m_xRef, -m_yRef));
    if (clipToSegment(bbox, lx, hx, ly, hy)) {
        for (int i=ly; i<=hy; i++) {
            for (int j=lx; j<=hx; j++) {
                t->apply(m_points[i * m_size + j], j + m_xRef, i + m_yRef);
            }
        }
    }

    //currently mods dont fix the normals
    invalidate(false);
}

/// \brief Add an area to those that affect this segment.
///
/// Call from Terrain when an Area is added which is found to intersect this
/// segment.
/// @param ar the area to be added.
/// @return zero if the area was added, non-zero otherwise
int Segment::addArea(const Area* ar)
{
    m_areas.insert(Areastore::value_type(ar->getLayer(), ar));

    // If this segment has not been shaded at all yet, we have nothing
    // to do. A surface will be created for this area later when the
    // whole segment is done.
    if (m_surfaces.empty()) {
        return 0;
    }

    Segment::Surfacestore::const_iterator J = m_surfaces.find(ar->getLayer());
    if (J != m_surfaces.end()) {
        // segment already has a surface for this shader, mark it
        // for re-generation
        J->second->invalidate();
        return 0;
    }

    if (ar->getShader() == 0) {
        return 0;
    }
    
    m_surfaces[ar->getLayer()] = ar->getShader()->newSurface(*this);

    return 0;
}

int Segment::updateArea(const Area* area)
{
    Areastore::iterator I = m_areas.lower_bound(area->getLayer());
    Areastore::iterator Iend = m_areas.upper_bound(area->getLayer());
    for (; I != Iend; ++I) {
        if (I->second == area) {
            invalidateSurfaces();
            return 0;
        }
    }
    return -1;
}

/// \brief Remove an area from those that affect this segment.
int Segment::removeArea(const Area* area)
{
    Areastore::iterator I = m_areas.lower_bound(area->getLayer());
    Areastore::iterator Iend = m_areas.upper_bound(area->getLayer());
    for (; I != Iend; ++I) {
        if (I->second == area) {
            m_areas.erase(I);

            // TODO(alriddoch,2010-10-22):
            // Copy the code from AreaShader::checkIntersects
            // into Area::removeFromSegment or something, and then
            // work out what to do to determine what type of surface
            // we are dealing with.

            Segment::Surfacestore::const_iterator J = m_surfaces.find(area->getLayer());
            if (J != m_surfaces.end()) {
                // segment already has a surface for this shader, mark it
                // for re-generation
                J->second->invalidate();
            }

            return 0;
        }
    }
    return -1;
}

WFMath::AxisBox<2> Segment::getRect() const
{
    WFMath::Point<2> lp(m_xRef, m_yRef), 
        hp(lp.x() + m_res, lp.y() + m_res);
    return WFMath::AxisBox<2>(lp, hp);
}

WFMath::AxisBox<3> Segment::getBox() const
{
    WFMath::Point<3> lp(m_xRef, m_yRef, m_min), 
        hp(lp.x() + m_res, lp.y() + m_res, m_max);
    return WFMath::AxisBox<3>(lp, hp);
}

} // namespace Mercator
