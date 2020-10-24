// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iround.h"

#include "Terrain.h"

#include "Matrix.h"
#include "Segment.h"
#include "TerrainMod.h"
#include "Shader.h"
#include "Area.h"
#include "Surface.h"

#include <iostream>
#include <algorithm>

#include <cstdio>

namespace Mercator {

const unsigned int Terrain::DEFAULT;
const unsigned int Terrain::SHADED;
constexpr float Terrain::defaultLevel;


Terrain::Terrain(unsigned int options, int resolution) : m_options(options),
                                                                  m_res(resolution),
                                                                  m_spacing((float)resolution)
{
}

Terrain::~Terrain() = default;

void Terrain::addShader(const Shader * t, int id)
{
    if (m_shaders.count(id)) {
        std::cerr << "WARNING: duplicate use of shader ID " << id << std::endl;
    }

    m_shaders[id] = t;

    auto I = m_segments.begin();
    auto Iend = m_segments.end();
    for (; I != Iend; ++I) {
        auto J = I->second.begin();
        auto Jend = I->second.end();
        for (; J != Jend; ++J) {
            auto& seg=J->second;

            Segment::Surfacestore & sss = seg->getSurfaces();
            sss[id] = t->newSurface(*seg);
        }
    }
}

void Terrain::removeShader(const Shader * t, int id)
{

    m_shaders.erase(m_shaders.find(id));

    // Delete all surfaces for this shader
    auto I = m_segments.begin();
    auto Iend = m_segments.end();
    for (; I != Iend; ++I) {
        auto J = I->second.begin();
        auto Jend = I->second.end();
        for (; J != Jend; ++J) {
            auto& seg=J->second;

            Segment::Surfacestore & sss = seg->getSurfaces();
            auto K = sss.find(id);
            if (K != sss.end()) {
                sss.erase(K);
            }
        }
    }
}



void Terrain::addSurfaces(Segment & seg)
{
    Segment::Surfacestore & sss = seg.getSurfaces();
    if (!sss.empty()) {
        std::cerr << "WARNING: Adding surfaces to a terrain segment which has surfaces"
                  << std::endl << std::flush;
        sss.clear();
    }

    auto I = m_shaders.begin();
    auto Iend = m_shaders.end();
    for (; I != Iend; ++I) {
        // shader doesn't touch this segment, skip
        if (!I->second->checkIntersect(seg)) {
            continue;
        }

        sss[I->first] = I->second->newSurface(seg);
    }
}

void Terrain::shadeSurfaces(Segment & seg)
{
    seg.populateSurfaces();
}


float Terrain::get(float x, float z) const
{
    Segment * s = getSegmentAtIndex(posToIndex(x), posToIndex(z));
    if ((s == nullptr) || (!s->isValid())) {
        return Terrain::defaultLevel;
    }
    return s->get(I_ROUND(x) - s->getXRef(), I_ROUND(z) - s->getZRef());
}

bool Terrain::getHeight(float x, float z, float& h) const
{
    Segment * s = getSegmentAtIndex(posToIndex(x), posToIndex(z));
    if ((!s) || (!s->isValid())) {
        return false;
    }
    s->getHeight(x - s->getXRef(), z - s->getZRef(), h);
    return true;
}

bool Terrain::getHeightAndNormal(float x, float z, float & h,
                                  WFMath::Vector<3> & n) const
{
    Segment * s = getSegmentAtIndex(posToIndex(x), posToIndex(z));
    if ((!s) || (!s->isValid())) {
        return false;
    }
    s->getHeightAndNormal(x - s->getXRef(), z - s->getZRef(), h, n);
    return true;
}

bool Terrain::getBasePoint(int x, int z, BasePoint& y) const
{
    auto I = m_basePoints.find(x);
    if (I == m_basePoints.end()) {
        return false;
    }
    auto J = I->second.find(z);
    if (J == I->second.end()) {
        return false;
    }
    y = J->second;
    return true;
}

void Terrain::setBasePoint(int x, int z, const BasePoint& y)
{
    m_basePoints[x][z] = y;
    bool pointIsSet[3][3];
    BasePoint existingPoint[3][3];
    for(int i = x - 1, ri = 0; i < x + 2; ++i, ++ri) {
        for(int j = z - 1, rj = 0; j < z + 2; ++j, ++rj) {
            pointIsSet[ri][rj] = getBasePoint(i, j, existingPoint[ri][rj]);
        }
    }
    for(int i = x - 1, ri = 0; i < x + 1; ++i, ++ri) {
        for(int j = z - 1, rj = 0; j < z + 1; ++j, ++rj) {
            Segment * existingSegment = getSegmentAtIndex(i, j);
            if (!existingSegment) {
                bool complete = pointIsSet[ri][rj] &&
                                pointIsSet[ri + 1][rj + 1] &&
                                pointIsSet[ri + 1][rj] &&
                                pointIsSet[ri][rj + 1];
                if (!complete) {
                    continue;
                }
                auto newSegment = std::make_unique<Segment>(i * m_res, j * m_res, m_res);
                Matrix<2, 2, BasePoint> & cp = newSegment->getControlPoints();
                for(unsigned int k = 0; k < 2; ++k) {
                    for(unsigned int l = 0; l < 2; ++l) {
                        cp(k, l) = existingPoint[ri + k][rj + l];
                    }
                }

                for (auto& entry : m_terrainMods) {
                    auto& terrainMod = entry.second.terrainMod;
                    if (terrainMod->checkIntersects(*newSegment)) {
						newSegment->updateMod(entry.first, terrainMod.get());
                    }
                }

                // apply shaders last, after all other data is in place
                if (isShaded()) {
                    addSurfaces(*newSegment);
                }

				newSegment->setCornerPoint(ri ? 0 : 1, rj ? 0 : 1, y);
				m_segments[i][j] = std::move(newSegment);
            } else {
				existingSegment->setCornerPoint(ri ? 0 : 1, rj ? 0 : 1, y);
            }
        }
    }
}

Segment * Terrain::getSegmentAtIndex(int x, int z) const
{
    auto I = m_segments.find(x);
    if (I == m_segments.end()) {
        return nullptr;
    }
    auto J = I->second.find(z);
    if (J == I->second.end()) {
        return nullptr;
    }
    return J->second.get();
}

void Terrain::processSegments(const WFMath::AxisBox<2>& area,
        const std::function<void(Segment&, int, int)>& func) const
{
    int lx = I_ROUND(std::floor((area.lowCorner()[0]) / m_spacing));
    int lz = I_ROUND(std::floor((area.lowCorner()[1]) / m_spacing));
    int hx = I_ROUND(std::ceil((area.highCorner()[0]) / m_spacing));
    int hz = I_ROUND(std::ceil((area.highCorner()[1]) / m_spacing));

    for (int i = lx; i < hx; ++i) {
        for (int j = lz; j < hz; ++j) {
            Segment *s = getSegmentAtIndex(i, j);
            if (!s) {
                continue;
            }
            func(*s, i, j);
        }
    }
}


Terrain::Rect Terrain::updateMod(long id, std::unique_ptr<TerrainMod> mod)
{
    std::set<Segment*> removed, added, updated;

    auto I = m_terrainMods.find(id);

    Rect old_box;
    if (I != m_terrainMods.end()) {
        auto& entry = I->second;

        old_box = entry.rect;



        int lx=I_ROUND(std::floor((old_box.lowCorner()[0] - 1.f) / m_spacing));
        int lz=I_ROUND(std::floor((old_box.lowCorner()[1] - 1.f) / m_spacing));
        int hx=I_ROUND(std::ceil((old_box.highCorner()[0] + 1.f) / m_spacing));
        int hz=I_ROUND(std::ceil((old_box.highCorner()[1] + 1.f) / m_spacing));

        for (int i=lx;i<hx;++i) {
           for (int j=lz;j<hz;++j) {
               Segment *s=getSegmentAtIndex(i,j);
               if (!s) {
                   continue;
               }

               removed.insert(s);

           } // of y loop
        } // of x loop

        if (!mod) {
            m_terrainMods.erase(id);
        }
    }

    if (mod) {
		auto rect = mod->bbox();
        int lx=I_ROUND(std::floor((rect.lowCorner()[0] - 1.f) / m_spacing));
        int lz=I_ROUND(std::floor((rect.lowCorner()[1] - 1.f) / m_spacing));
        int hx=I_ROUND(std::ceil((rect.highCorner()[0] + 1.f) / m_spacing));
        int hz=I_ROUND(std::ceil((rect.highCorner()[1] + 1.f) / m_spacing));

        for (int i=lx;i<hx;++i) {
            for (int j=lz;j<hz;++j) {
                Segment *s=getSegmentAtIndex(i,j);
                if (!s) {
                    continue;
                }

                auto J = removed.find(s);
                if (J == removed.end()) {
                    added.insert(s);
                } else {
                    updated.insert(s);
                    removed.erase(J);
                }
            } // of y loop
        } // of x loop

		for (auto& segment : added) {
			if (mod->checkIntersects(*segment)) {
				segment->updateMod(id, mod.get());
			}
		}
		for (auto& segment : updated) {
			if (mod->checkIntersects(*segment)) {
				segment->updateMod(id, mod.get());
			} else {
				segment->updateMod(id, nullptr);
			}
		}

		m_terrainMods[id] = TerrainModEntry{std::move(mod), rect};
	}

    for (auto& segment : removed) {
        segment->updateMod(id, nullptr);
    }


    return old_box;
}

bool Terrain::hasMod(long id) const
{
    return m_terrainMods.find(id) != m_terrainMods.end();
}

const TerrainMod* Terrain::getMod(long id) const
{
    auto I = m_terrainMods.find(id);
    if (I != m_terrainMods.end()) {
        return I->second.terrainMod.get();
    }
    return nullptr;
}

void Terrain::addArea(const Area * area)
{
    int layer = area->getLayer();

    Shaderstore::const_iterator I = m_shaders.find(layer);
    if (I != m_shaders.end()) {
        area->setShader(I->second);
    }

    //work out which segments are overlapped by this effector
    //note that the bbox is expanded by one grid unit because
    //segments share edges. this ensures a mod along an edge
    //will affect both segments.

    m_terrainAreas.emplace(area, area->bbox());

    int lx=I_ROUND(std::floor((area->bbox().lowCorner()[0] - 1.f) / m_spacing));
    int lz=I_ROUND(std::floor((area->bbox().lowCorner()[1] - 1.f) / m_spacing));
    int hx=I_ROUND(std::ceil((area->bbox().highCorner()[0] + 1.f) / m_spacing));
    int hz=I_ROUND(std::ceil((area->bbox().highCorner()[1] + 1.f) / m_spacing));

    for (int i=lx;i<hx;++i) {
        for (int j=lz;j<hz;++j) {
            Segment *s=getSegmentAtIndex(i,j);
            if (s) {
                if (area->checkIntersects(*s)) {
                    s->addArea(area);
                }
            }
        } // of y loop
    } // of x loop
}

Terrain::Rect Terrain::updateArea(const Area * area)
{
    std::set<Segment*> removed, added, updated;

     auto I = m_terrainAreas.find(area);

     Rect old_box;
     if (I != m_terrainAreas.end()) {

         old_box = I->second;

         int lx=I_ROUND(std::floor((old_box.lowCorner()[0] - 1.f) / m_spacing));
         int lz=I_ROUND(std::floor((old_box.lowCorner()[1] - 1.f) / m_spacing));
         int hx=I_ROUND(std::ceil((old_box.highCorner()[0] + 1.f) / m_spacing));
         int hz=I_ROUND(std::ceil((old_box.highCorner()[1] + 1.f) / m_spacing));

         for (int i=lx;i<hx;++i) {
            for (int j=lz;j<hz;++j) {
                Segment *s=getSegmentAtIndex(i,j);
                if (!s) {
                    continue;
                }

                removed.insert(s);

            } // of y loop
         } // of x loop

         I->second = area->bbox();

     } else {
         m_terrainAreas.emplace(area, area->bbox());
     }



     int lx=I_ROUND(std::floor((area->bbox().lowCorner()[0] - 1.f) / m_spacing));
     int lz=I_ROUND(std::floor((area->bbox().lowCorner()[1] - 1.f) / m_spacing));
     int hx=I_ROUND(std::ceil((area->bbox().highCorner()[0] + 1.f) / m_spacing));
     int hz=I_ROUND(std::ceil((area->bbox().highCorner()[1] + 1.f) / m_spacing));

     for (int i=lx;i<hx;++i) {
         for (int j=lz;j<hz;++j) {
             Segment *s=getSegmentAtIndex(i,j);
             if (!s) {
                 continue;
             }

             auto J = removed.find(s);
             if (J == removed.end()) {
                 added.insert(s);
             } else {
                 updated.insert(s);
                 removed.erase(J);
             }
         } // of y loop
     } // of x loop

     for (auto& segment : removed) {
         segment->removeArea(area);
     }
     for (auto& segment : added) {
         if (area->checkIntersects(*segment)) {
             segment->addArea(area);
         }
     }
     for (auto& segment : updated) {
         if (area->checkIntersects(*segment)) {
             if (segment->updateArea(area) != 0) {
                 segment->addArea(area);
             }
         } else {
             segment->removeArea(area);
         }
     }

     return old_box;
}


void Terrain::removeArea(const Area * area)
{
    m_terrainAreas.erase(area);

    const Rect & eff_box = area->bbox();

    int lx=I_ROUND(std::floor((eff_box.lowCorner()[0] - 1.f) / m_spacing));
    int lz=I_ROUND(std::floor((eff_box.lowCorner()[1] - 1.f) / m_spacing));
    int hx=I_ROUND(std::ceil((eff_box.highCorner()[0] + 1.f) / m_spacing));
    int hz=I_ROUND(std::ceil((eff_box.highCorner()[1] + 1.f) / m_spacing));

    for (int i=lx;i<hx;++i) {
        for (int j=lz;j<hz;++j) {
            Segment *s=getSegmentAtIndex(i,j);
            if (s) {
                s->removeArea(area);
            }
        } // of y loop
    } // of x loop
}

bool Terrain::hasArea(const Area* a) const
{
    return m_terrainAreas.find(a) != m_terrainAreas.end();
}


} // namespace Mercator
