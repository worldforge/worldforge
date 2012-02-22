// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iround.h"

#include <Mercator/Terrain.h>

#include <Mercator/Matrix.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>
#include <Mercator/Shader.h>
#include <Mercator/Area.h>
#include <Mercator/Surface.h>

#include <iostream>

#include <cstdio>

namespace Mercator {

const float Terrain::defaultLevel = 8;

/// \brief Construct a new Terrain object with optional options and resolution.
///
/// @param options a bitfield of option flags. Defaults to no flags set.
/// - DEFAULT value provided for no flags set.
/// - SHADED is set if shaders are going to be used on this terrain.
/// @param resolution the spacing between adjacent base points. Defaults to 64.
Terrain::Terrain(unsigned int options, unsigned int resolution) : m_options(options),
                                                                  m_res(resolution),
                                                                  m_spacing(resolution)
{

}

/// \brief Desctruct Terrain object, deleting contained objects.
///
/// All Segment objects are deleted, but Shader objects are not yet deleted.
/// Probably need to be fixed.
Terrain::~Terrain()
{
    Segmentstore::const_iterator I = m_segments.begin(); 
    Segmentstore::const_iterator Iend = m_segments.end();
    for (; I != Iend; ++I) {
        Segmentcolumn::const_iterator J = I->second.begin(); 
        Segmentcolumn::const_iterator Jend = I->second.end(); 
        for (; J != Jend; ++J) {
            Segment * s = J->second;
            if (s) delete s;
        }
    }
}

/// \brief Add a new Shader to the list for this terrain.
///
/// As each shader is added, surfaces are added to all existing segments
/// to store the result of the shader.
void Terrain::addShader(const Shader * t, int id)
{
    if (m_shaders.count(id)) {
        std::cerr << "WARNING: duplicate use of shader ID " << id << std::endl;
    }
    
    m_shaders[id] = t;
    
    Segmentstore::const_iterator I = m_segments.begin(); 
    Segmentstore::const_iterator Iend = m_segments.end(); 
    for (; I != Iend; ++I) {
        Segmentcolumn::const_iterator J = I->second.begin(); 
        Segmentcolumn::const_iterator Jend = I->second.end(); 
        for (; J != Jend; ++J) {
            Segment *seg=J->second;
            if (!t->checkIntersect(*seg)) {
                continue;
            }

            Segment::Surfacestore & sss = seg->getSurfaces();
            sss[id] = t->newSurface(*seg);
        }
    }
}

/// \brief remove a Shader from the list for this terrain.
///
/// As each shader is removed, surfaces are removed from existing segments
void Terrain::removeShader(const Shader * t, int id)
{

    m_shaders.erase(m_shaders.find(id));

    // Delete all surfaces for this shader
    Segmentstore::const_iterator I = m_segments.begin();
    Segmentstore::const_iterator Iend = m_segments.end();
    for (; I != Iend; ++I) {
        Segmentcolumn::const_iterator J = I->second.begin();
        Segmentcolumn::const_iterator Jend = I->second.end();
        for (; J != Jend; ++J) {
            Segment *seg=J->second;

            Segment::Surfacestore & sss = seg->getSurfaces();
            Segment::Surfacestore::iterator K = sss.find(id);
            if (K != sss.end()) {
                delete K->second;
                sss.erase(K);
            }
        }
    }
}



/// \brief Add the required Surface objects to a Segment.
///
/// If shading is enabled, each Segment has a set of Surface objects
/// corresponding to the Shader objects available for this terrain.
/// This function creates the necessary Surface objects, and adds them
/// to the list in the Segment object. At this point the Segment is
/// not yet populated with heightfield data, so the Surface cannot
/// be populated. A newly constructed surface does not allocate its
/// buffer memory, so there is very little overhead to creating it.
void Terrain::addSurfaces(Segment & seg)
{
    Segment::Surfacestore & sss = seg.getSurfaces();
    if (!sss.empty()) {
        std::cerr << "WARNING: Adding surfaces to a terrain segment which has surfaces"
                  << std::endl << std::flush;
        sss.clear();
    }
    
    Shaderstore::const_iterator I = m_shaders.begin();
    Shaderstore::const_iterator Iend = m_shaders.end();
    for (; I != Iend; ++I) {
        // shader doesn't touch this segment, skip
        if (!I->second->checkIntersect(seg)) {
            continue;
        }
        
        sss[I->first] = I->second->newSurface(seg);
    }
}

/// \brief Populate the Surface objects associated with a Segment.
///
/// This is called after a Segment has been populated with heightfield
/// data. The actual responsibility for populating the Surface objects
/// is in Segment::populateSurfaces().
void Terrain::shadeSurfaces(Segment & seg)
{
    seg.populateSurfaces();
}

/// \brief Get the height value at a given coordinate x,y.
///
/// This is a convenience function provided to quickly get a height
/// value at a given point. It always succeeds, as if no height data
/// is available it just returns the default value. If a Segment does
/// exist in the right place and is populated, the coords within that
/// Segment are determined, and the heightfield queried. This function
/// does not cause any data to be populated, and does not attempt to
/// do any interpolation to get an accurate height figure. For more
/// accurate results see Terrain::getHeightAndNormal.
float Terrain::get(float x, float y) const
{
    int ix = I_ROUND(std::floor(x / m_res));
    int iy = I_ROUND(std::floor(y / m_res));

    Segment * s = getSegment(ix, iy);
    if ((s == 0) || (!s->isValid())) {
        return Terrain::defaultLevel;
    }
    return s->get(I_ROUND(x) - (ix * m_res), I_ROUND(y) - (iy * m_res));
}

/// \brief Get an accurate height and normal vector at a given coordinate
/// x,y.
///
/// This is a more expensive function that Terrain::get() for getting an
/// accurate height value and surface normal at a given point. The main
/// body of functionality is in the Segment::getHeightAndNormal() function
/// called from here.
/// The height and normal are interpolated based on a model where each
/// tile of the heightfield comprises two triangles. If no heightfield data
/// is available for the given location, this function returns false, and
/// no data is returned.
/// @param x coordinate of point to be returned.
/// @param y coordinate of point to be returned.
/// @param h reference to variable which will be used to store the resulting
/// height value.
/// @param n reference to variable which will be used to store the resulting
/// normal value.
/// @return true if heightdata was available, false otherwise.
bool Terrain::getHeightAndNormal(float x, float y, float & h,
                                  WFMath::Vector<3> & n) const
{
    int ix = I_ROUND(std::floor(x / m_res));
    int iy = I_ROUND(std::floor(y / m_res));

    Segment * s = getSegment(ix, iy);
    if ((s == 0) || (!s->isValid())) {
        return false;
    }
    s->getHeightAndNormal(x - (ix * m_res), y - (iy * m_res), h, n);
    return true;
}

/// \brief Get the BasePoint at a given base point coordinate.
///
/// Get the BasePoint value for the given coordinate on the base
/// point grid.
/// @param x coordinate on the base point grid.
/// @param y coordinate on the base point grid.
/// @param z reference to varaible which will be used to store the
/// BasePoint data.
/// @return true if a BasePoint is defined at the given coordinate, false
/// otherwise.
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

/// \brief Set the BasePoint value at a given base point coordinate.
///
/// Set the BasePoint value for the given coordinate on the base
/// point grid.
/// If inserting this BasePoint completes the set of points required
/// to define one or more Segment objects which were not yet defined,
/// new Segment objects are created. If this replaces a point for one
/// or more Segment objects that were already defined, the contents of
/// those Segment objects are invalidated.
/// @param x coordinate on the base point grid.
/// @param y coordinate on the base point grid.
/// @param z BasePoint value to be used at the given coordinate.
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
                s = new Segment(i * m_res, j * m_res, m_res);
                Matrix<2, 2, BasePoint> & cp = s->getControlPoints();
                float min = existingPoint[ri][rj].height();
                float max = existingPoint[ri][rj].height();
                for(unsigned int k = 0; k < 2; ++k) {
                    for(unsigned int l = 0; l < 2; ++l) {
                        cp(k, l) = existingPoint[ri + k][rj + l];
                        min = std::min(cp(k, l).height(), min);
                        max = std::max(cp(k, l).height(), max);
                    }
                }
                s->setMinMax(min, max);
                
                Effectorstore::iterator I = m_effectors.begin();
                Effectorstore::iterator Iend = m_effectors.end();
                for (; I != Iend; ++I) {
                    I->first->addToSegment(*s);
                }

                // apply shaders last, after all other data is in place
                if (isShaded()) {
                    addSurfaces(*s);
                }
                
                m_segments[i][j] = s;
                continue;
            }
            s->setCornerPoint(ri ? 0 : 1, rj ? 0 : 1, z);
        }
    }
}

/// \brief Get the Segment at a given base point coordinate.
///
/// Get the Segment pointer for the given coordinate on the base
/// point grid. The Segment in question may not have been populated
/// with heightfield or surface data.
/// @param x coordinate on the base point grid.
/// @param y coordinate on the base point grid.
/// @return a valid pointer if a Segment is defined at the given coordinate,
/// zero otherwise.
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

/// \brief Add an effector to the terrain
void Terrain::addEffector(const Effector * eff)
{
    //work out which segments are overlapped by this effector
    //note that the bbox is expanded by one grid unit because
    //segments share edges. this ensures a mod along an edge
    //will affect both segments.

    m_effectors.insert(Effectorstore::value_type(eff, eff->bbox()));

    int lx=I_ROUND(std::floor((eff->bbox().lowCorner()[0] - 1.f) / m_res));
    int ly=I_ROUND(std::floor((eff->bbox().lowCorner()[1] - 1.f) / m_res));
    int hx=I_ROUND(std::ceil((eff->bbox().highCorner()[0] + 1.f) / m_res));
    int hy=I_ROUND(std::ceil((eff->bbox().highCorner()[1] + 1.f) / m_res));

    for (int i=lx;i<hx;++i) {
        for (int j=ly;j<hy;++j) {
            Segment *s=getSegment(i,j);
            if (s) {
                eff->addToSegment(*s);
            }
        } // of y loop
    } // of x loop
}

Terrain::Rect Terrain::updateEffector(const Effector * eff)
{
    Effectorstore::iterator I = m_effectors.find(eff);

    if (I == m_effectors.end()) {
        return Rect();
    }

    const Rect old_box = I->second;


    std::set<Segment*> removed, added, updated;

    int lx=I_ROUND(std::floor((old_box.lowCorner()[0] - 1) / m_res));
    int ly=I_ROUND(std::floor((old_box.lowCorner()[1] - 1) / m_res));
    int hx=I_ROUND(std::ceil((old_box.highCorner()[0] + 1) / m_res));
    int hy=I_ROUND(std::ceil((old_box.highCorner()[1] + 1) / m_res));

    for (int i=lx;i<hx;++i) {
        for (int j=ly;j<hy;++j) {
            Segment *s=getSegment(i,j);
            if (!s) {
                continue;
            }

            removed.insert(s);

        } // of y loop
    } // of x loop

    lx=I_ROUND(std::floor((eff->bbox().lowCorner()[0] - 1) / m_res));
    ly=I_ROUND(std::floor((eff->bbox().lowCorner()[1] - 1) / m_res));
    hx=I_ROUND(std::ceil((eff->bbox().highCorner()[0] + 1) / m_res));
    hy=I_ROUND(std::ceil((eff->bbox().highCorner()[1] + 1) / m_res));

    for (int i=lx;i<hx;++i) {
        for (int j=ly;j<hy;++j) {
            Segment *s=getSegment(i,j);
            if (!s) {
                continue;
            }

            std::set<Segment*>::iterator J = removed.find(s);
            if (J == removed.end()) {
                added.insert(s);
            } else {
                updated.insert(s);
                removed.erase(J);
            }
        } // of y loop
    } // of x loop

    std::set<Segment*>::iterator J = removed.begin();
    std::set<Segment*>::iterator Jend = removed.end();
    for (; J != Jend; ++J) {
        eff->removeFromSegment(**J);
    }

    J = added.begin();
    Jend = added.end();
    for (; J != Jend; ++J) {
        eff->addToSegment(**J);
    }

    J = updated.begin();
    Jend = updated.end();
    for (; J != Jend; ++J) {
        eff->updateToSegment(**J);
    }

    I->second = eff->bbox();
    return old_box;
}

/// \brief Remove an effector from the terrain
void Terrain::removeEffector(const Effector * eff)
{
    m_effectors.erase(eff);

    const Rect & eff_box = eff->bbox();

    int lx=I_ROUND(std::floor((eff_box.lowCorner()[0] - 1) / m_res));
    int ly=I_ROUND(std::floor((eff_box.lowCorner()[1] - 1) / m_res));
    int hx=I_ROUND(std::ceil((eff_box.highCorner()[0] + 1) / m_res));
    int hy=I_ROUND(std::ceil((eff_box.highCorner()[1] + 1) / m_res));

    for (int i=lx;i<hx;++i) {
        for (int j=ly;j<hy;++j) {
            Segment *s=getSegment(i,j);
            if (s) {
                eff->removeFromSegment(*s);
            }
        } // of y loop
    } // of x loop
}

/// \brief Add a modifier to the terrain.
///
/// Add a new TerrainMod object to the terrain, which defines a modification
/// to the terrain heightfield or surface data. The segments are responsible
/// for storing the TerrainMod objects, so the apropriate Segment objects
/// are found and the TerrainMode is passed to each in turn.
/// @param t reference to the TerrainMod object to be applied.
void Terrain::addMod(const TerrainMod * mod)
{
    addEffector(mod);
}

Terrain::Rect Terrain::updateMod(const TerrainMod * mod)
{
    return updateEffector(mod);
}

void Terrain::removeMod(const TerrainMod * mod)
{
    removeEffector(mod);
}

/// \brief Add an area modifier to the terrain.
///
/// Add a new Area object to the terrain, which defines a modification
/// to the surface.
void Terrain::addArea(const Area * area)
{
    int layer = area->getLayer();

    Shaderstore::const_iterator I = m_shaders.find(layer);
    if (I != m_shaders.end()) {
        area->setShader(I->second);
    }
    
    addEffector(area);
}

/// \brief Apply changes to an area modifier to the terrain.
Terrain::Rect Terrain::updateArea(const Area * area)
{
    return updateEffector(area);
}

/// \brief Remove an area modifier from the terrain.
///
/// Remove an existing Area object from the terrain, and mark all the
/// affected terrain surfaces as invalid.
void Terrain::removeArea(const Area * area)
{
    removeEffector(area);
}

} // namespace Mercator
