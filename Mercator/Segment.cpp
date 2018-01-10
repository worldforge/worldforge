// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iround.h"

#include "Segment.h"
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


/// \brief Construct an empty segment with the given resolution.
///
/// Generally it is not necessary to call this from outside the Mercator
/// library Segment objects are created as required. The Segment is
/// constructed without allocating any storage for heightfield or surface
/// normal data. The m_min and m_max members are initialised to extreme
/// values, and should be set to appropriate using setMinMax() as soon as
/// possible after construction. Similarly the control points should be
/// set soon after construction.
Segment::Segment(int x, int z, unsigned int resolution) :
                            m_res(resolution), m_size(m_res+1),
                            m_xRef(x), m_zRef(z),
                            m_heightMap(resolution), m_normals(nullptr)
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
    delete [] m_normals;
    
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
    m_heightMap.allocate();
    populateHeightMap(m_heightMap);

    for (auto& entry : m_terrainMods) {
        applyMod(entry.second);
    }
}

void Segment::populateHeightMap(HeightMap& heightMap)
{
    heightMap.fill2d(m_controlPoints(0, 0), m_controlPoints(1, 0),
           m_controlPoints(1, 1), m_controlPoints(0, 1));
}


/// \brief Mark the contents of this Segment as stale.
///
/// This is called internally whenever changes occur that mean that the
/// heightfield and surface normal data are no longer valid.
/// If surface normal storage is deallocated, and if the points argument
/// is true the heightfield storage is also deallocated.
void Segment::invalidate(bool points)
{
    if (points) {
        m_heightMap.invalidate();
    }
    if (m_normals) {
        delete [] m_normals;
        m_normals = nullptr;
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
    assert(m_heightMap.isValid());
	assert(m_size != 0);
	assert(m_res == m_size - 1);

    if (m_normals == nullptr) {
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
           np[j * m_size * 3 + i * 3 + 1] = 1.0;
           np[j * m_size * 3 + i * 3 + 2] = (h4 - h2) / 2.f;
        }
    }

    //edges have one axis pegged to 0
    
    //top and bottom boundary
    for (int i=1; i < m_res; ++i) {
        h1 = m_heightMap.get(i - 1, 0);
        h2 = m_heightMap.get(i + 1, 0);
        
        np[i * 3]     = (h1 - h2) / 2.f;
        np[i * 3 + 1] = 1.0;
        np[i * 3 + 2] = 0.0;
 
        h1 = m_heightMap.get(i - 1, m_res);
        h2 = m_heightMap.get(i + 1, m_res);
        
        np[m_res * m_size * 3 + i * 3]     = (h1 - h2) / 2.f;
        np[m_res * m_size * 3 + i * 3 + 1] = 1.0f;
        np[m_res * m_size * 3 + i * 3 + 2] = 0.0f;
    }
    
    //left and right boundary
    for (int j=1; j < m_res; ++j) {
        h1 = m_heightMap.get(0, j - 1);
        h2 = m_heightMap.get(0, j + 1);
        
        np[j * m_size * 3]     = 0;
        np[j * m_size * 3 + 1] = 1.f;
        np[j * m_size * 3 + 2] = (h1 - h2) / 2.f;
 
        h1 = m_heightMap.get(m_res, j - 1);
        h2 = m_heightMap.get(m_res, j + 1);

        np[j * m_size * 3 + m_res * 3]     = 0.f;
        np[j * m_size * 3 + m_res * 3 + 1] = 1.f;
        np[j * m_size * 3 + m_res * 3 + 2] = (h1 - h2) / 2.f;
    }

    //corners - these are all treated as flat
    //so the normal points straight up
    np[0] = 0.f;
    np[1] = 1.f;
    np[2] = 0.f;

    np[m_res * m_size * 3]     = 0.f;
    np[m_res * m_size * 3 + 1] = 1.f;
    np[m_res * m_size * 3 + 2] = 0.f;

    np[m_res * 3]     = 0.f;
    np[m_res * 3 + 1] = 1.f;
    np[m_res * 3 + 2] = 0.f;
    
    np[m_res * m_size * 3 + m_res * 3]     = 0.f;
    np[m_res * m_size * 3 + m_res * 3 + 1] = 1.f;
    np[m_res * m_size * 3 + m_res * 3 + 2] = 0.f;
}

/// \brief Populate the surfaces associated with this Segment.
///
/// Call Surface::populate() for each Surface in turn.
void Segment::populateSurfaces()
{
    for (const auto& entry : m_surfaces) {
        if (entry.second->m_shader.checkIntersect(*this)) {
            entry.second->populate();
        }
    }
}

void Segment::getHeight(float x, float y, float &h) const
{
    m_heightMap.getHeight(x, y, h);
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
void Segment::getHeightAndNormal(float x, float z, float& h,
                                 WFMath::Vector<3> &normal) const
{
    m_heightMap.getHeightAndNormal(x, z, h, normal);
}

/// \brief Determine the intersection between an axis aligned box and
/// this segment.
///
/// @param bbox axis aligned box to be tested.
/// @param lx lower x coordinate of intersection area.
/// @param hx upper x coordinate of intersection area.
/// @param lz lower z coordinate of intersection area.
/// @param hz upper z coordinate of intersection area.
/// @return true if the box intersects with this Segment, false otherwise.
bool Segment::clipToSegment(const WFMath::AxisBox<2> &bbox,
                            int &lx, int &hx, int &lz, int &hz) const
{
    lx = I_ROUND(bbox.lowCorner()[0]); 
    if (lx > m_res) return false;
    if (lx < 0) lx = 0;
    
    hx = I_ROUND(bbox.highCorner()[0]); 
    if (hx < 0) return false;
    if (hx > m_res) hx = m_res;
    
    lz = I_ROUND(bbox.lowCorner()[1]);
    if (lz > m_res) return false;
    if (lz < 0) lz = 0;
    
    hz = I_ROUND(bbox.highCorner()[1]);
    if (hz < 0) return false;
    if (hz > m_res) hz = m_res;

    return true;
}

void Segment::updateMod(long id, const TerrainMod *t)
{
    if (t) {
        m_terrainMods[id] = t;
    } else {
        m_terrainMods.erase(id);
    }
    invalidate();
}

/// \brief Delete all the modifications applied to this Segment.
///
/// Usually called from the destructor. It is not normally necessary to call
/// this function from the application.
void Segment::clearMods() 
{
    if (!m_terrainMods.empty()) {
        m_terrainMods.clear();
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
    int lx,hx,lz,hz;
    float* points = m_heightMap.getData();
    WFMath::AxisBox<2> bbox=t->bbox();
    bbox.shift(WFMath::Vector<2>(-m_xRef, -m_zRef));
    if (clipToSegment(bbox, lx, hx, lz, hz)) {
        for (int i=lz; i<=hz; i++) {
            for (int j=lx; j<=hx; j++) {
                float& h = points[i * m_size + j];
                t->apply(h, j + m_xRef, i + m_zRef);
                m_heightMap.checkMaxMin(h);
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
    auto I = m_areas.lower_bound(area->getLayer());
    auto Iend = m_areas.upper_bound(area->getLayer());
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
    auto I = m_areas.lower_bound(area->getLayer());
    auto Iend = m_areas.upper_bound(area->getLayer());
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
    WFMath::Point<2> lp(m_xRef, m_zRef),
        hp(lp.x() + m_res, lp.y() + m_res);
    return WFMath::AxisBox<2>(lp, hp);
}

} // namespace Mercator
