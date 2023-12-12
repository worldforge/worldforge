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


namespace Mercator {

const unsigned int Terrain::DEFAULT;
const unsigned int Terrain::SHADED;


Terrain::Terrain(unsigned int options, int resolution) : m_options(options),
														 m_res(resolution),
														 m_spacing((float) resolution) {
}

Terrain::~Terrain() = default;

void Terrain::addShader(const Shader* t, int id) {
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
			auto& seg = J->second;

			Segment::Surfacestore& sss = seg->getSurfaces();
			sss[id] = t->newSurface(*seg);
		}
	}
}

void Terrain::removeShader(const Shader*, int id) {

	m_shaders.erase(m_shaders.find(id));

	// Delete all surfaces for this shader
	auto I = m_segments.begin();
	auto Iend = m_segments.end();
	for (; I != Iend; ++I) {
		auto J = I->second.begin();
		auto Jend = I->second.end();
		for (; J != Jend; ++J) {
			auto& seg = J->second;

			Segment::Surfacestore& sss = seg->getSurfaces();
			auto K = sss.find(id);
			if (K != sss.end()) {
				sss.erase(K);
			}
		}
	}
}


void Terrain::addSurfaces(Segment& seg) {
	Segment::Surfacestore& sss = seg.getSurfaces();
	if (!sss.empty()) {
		std::cerr << "WARNING: Adding surfaces to a terrain segment which has surfaces"
				  << std::endl;
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

void Terrain::shadeSurfaces(Segment& seg) {
	seg.populateSurfaces();
}


float Terrain::get(float x, float z) const {
	Segment* s = getSegmentAtIndex(posToIndex(x), posToIndex(z));
	if ((s == nullptr) || (!s->isValid())) {
		return Terrain::defaultLevel;
	}
	return s->get(I_ROUND(x) - s->getXRef(), I_ROUND(z) - s->getZRef());
}

bool Terrain::getHeight(float x, float z, float& h) const {
	Segment* s = getSegmentAtIndex(posToIndex(x), posToIndex(z));
	if ((!s) || (!s->isValid())) {
		return false;
	}
	s->getHeight(x - (float)s->getXRef(), z - (float)s->getZRef(), h);
	return true;
}

bool Terrain::getHeightAndNormal(float x, float z, float& h,
								 WFMath::Vector<3>& n) const {
	Segment* s = getSegmentAtIndex(posToIndex(x), posToIndex(z));
	if ((!s) || (!s->isValid())) {
		return false;
	}
	s->getHeightAndNormal(x - (float)s->getXRef(), z - (float)s->getZRef(), h, n);
	return true;
}

bool Terrain::getBasePoint(int x, int z, BasePoint& y) const {
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

void Terrain::setBasePoint(int x, int z, const BasePoint& y) {
	m_basePoints[x][z] = y;
	bool pointIsSet[3][3];
	BasePoint existingPoint[3][3];
	for (int i = x - 1, ri = 0; i < x + 2; ++i, ++ri) {
		for (int j = z - 1, rj = 0; j < z + 2; ++j, ++rj) {
			pointIsSet[ri][rj] = getBasePoint(i, j, existingPoint[ri][rj]);
		}
	}
	for (int i = x - 1, ri = 0; i < x + 1; ++i, ++ri) {
		for (int j = z - 1, rj = 0; j < z + 1; ++j, ++rj) {
			Segment* existingSegment = getSegmentAtIndex(i, j);
			if (!existingSegment) {
				bool complete = pointIsSet[ri][rj] &&
								pointIsSet[ri + 1][rj + 1] &&
								pointIsSet[ri + 1][rj] &&
								pointIsSet[ri][rj + 1];
				if (!complete) {
					continue;
				}
				auto newSegment = std::make_unique<Segment>(i * m_res, j * m_res, m_res);
				Matrix<2, 2, BasePoint>& cp = newSegment->getControlPoints();
				for (unsigned int k = 0; k < 2; ++k) {
					for (unsigned int l = 0; l < 2; ++l) {
						cp(k, l) = existingPoint[ri + k][rj + l];
					}
				}

				for (auto& entry: m_terrainMods) {
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

Segment* Terrain::getSegmentAtIndex(int x, int z) const {
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
							  const std::function<void(Segment&, int, int)>& func) const {
	int lx = I_ROUND(std::floor((area.lowCorner()[0]) / m_spacing));
	int lz = I_ROUND(std::floor((area.lowCorner()[1]) / m_spacing));
	int hx = I_ROUND(std::ceil((area.highCorner()[0]) / m_spacing));
	int hz = I_ROUND(std::ceil((area.highCorner()[1]) / m_spacing));

	for (int i = lx; i < hx; ++i) {
		for (int j = lz; j < hz; ++j) {
			Segment* s = getSegmentAtIndex(i, j);
			if (!s) {
				continue;
			}
			func(*s, i, j);
		}
	}
}


Terrain::Rect Terrain::updateMod(long id, std::unique_ptr<TerrainMod> mod) {
	std::set<Segment*> removed, added, updated;
	std::unique_ptr<TerrainMod> old_mod;
	auto I = m_terrainMods.find(id);

	Rect old_box;
	if (I != m_terrainMods.end()) {
		auto& entry = I->second;

		old_box = entry.rect;
		//Make sure old mod survives within this method.
		old_mod = std::move(entry.terrainMod);


		int lx = I_ROUND(std::floor((old_box.lowCorner()[0] - 1.f) / m_spacing));
		int lz = I_ROUND(std::floor((old_box.lowCorner()[1] - 1.f) / m_spacing));
		int hx = I_ROUND(std::ceil((old_box.highCorner()[0] + 1.f) / m_spacing));
		int hz = I_ROUND(std::ceil((old_box.highCorner()[1] + 1.f) / m_spacing));

		for (int i = lx; i < hx; ++i) {
			for (int j = lz; j < hz; ++j) {
				Segment* s = getSegmentAtIndex(i, j);
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
		int lx = I_ROUND(std::floor((rect.lowCorner()[0] - 1.f) / m_spacing));
		int lz = I_ROUND(std::floor((rect.lowCorner()[1] - 1.f) / m_spacing));
		int hx = I_ROUND(std::ceil((rect.highCorner()[0] + 1.f) / m_spacing));
		int hz = I_ROUND(std::ceil((rect.highCorner()[1] + 1.f) / m_spacing));

		for (int i = lx; i < hx; ++i) {
			for (int j = lz; j < hz; ++j) {
				Segment* s = getSegmentAtIndex(i, j);
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

		for (auto& segment: added) {
			if (mod->checkIntersects(*segment)) {
				segment->updateMod(id, mod.get());
			}
		}
		for (auto& segment: updated) {
			if (mod->checkIntersects(*segment)) {
				segment->updateMod(id, mod.get());
			} else {
				segment->updateMod(id, nullptr);
			}
		}

		m_terrainMods[id] = TerrainModEntry{std::move(mod), rect};
	}

	for (auto& segment: removed) {
		segment->updateMod(id, nullptr);
	}


	return old_box;
}

bool Terrain::hasMod(long id) const {
	return m_terrainMods.find(id) != m_terrainMods.end();
}

const TerrainMod* Terrain::getMod(long id) const {
	auto I = m_terrainMods.find(id);
	if (I != m_terrainMods.end()) {
		return I->second.terrainMod.get();
	}
	return nullptr;
}

const Area* Terrain::getArea(long id) const {
	auto I = m_terrainAreas.find(id);
	if (I != m_terrainAreas.end()) {
		return I->second.terrainArea.get();
	}
	return nullptr;
}


Terrain::Rect Terrain::updateArea(long id, std::unique_ptr<Area> area) {
	std::set<Segment*> removed, added, updated;
	std::unique_ptr<Area> old_area;

	auto I = m_terrainAreas.find(id);

	Rect old_box;
	if (I != m_terrainAreas.end()) {
		auto& entry = I->second;

		old_box = entry.rect;
		//Make sure old area survives within this method.
		old_area = std::move(entry.terrainArea);


		int lx = I_ROUND(std::floor((old_box.lowCorner()[0] - 1.f) / m_spacing));
		int lz = I_ROUND(std::floor((old_box.lowCorner()[1] - 1.f) / m_spacing));
		int hx = I_ROUND(std::ceil((old_box.highCorner()[0] + 1.f) / m_spacing));
		int hz = I_ROUND(std::ceil((old_box.highCorner()[1] + 1.f) / m_spacing));

		for (int i = lx; i < hx; ++i) {
			for (int j = lz; j < hz; ++j) {
				Segment* s = getSegmentAtIndex(i, j);
				if (!s) {
					continue;
				}

				removed.insert(s);

			} // of y loop
		} // of x loop

		if (!area) {
			m_terrainAreas.erase(id);
		}
	}

	if (area) {
		auto rect = area->bbox();
		int lx = I_ROUND(std::floor((rect.lowCorner()[0] - 1.f) / m_spacing));
		int lz = I_ROUND(std::floor((rect.lowCorner()[1] - 1.f) / m_spacing));
		int hx = I_ROUND(std::ceil((rect.highCorner()[0] + 1.f) / m_spacing));
		int hz = I_ROUND(std::ceil((rect.highCorner()[1] + 1.f) / m_spacing));

		for (int i = lx; i < hx; ++i) {
			for (int j = lz; j < hz; ++j) {
				Segment* s = getSegmentAtIndex(i, j);
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

		const Shader* shader = nullptr;
		auto shaderI = m_shaders.find(area->getLayer());
		if (shaderI != m_shaders.end()) {
			shader = shaderI->second;
		}

		for (auto& segment: added) {
			if (area->checkIntersects(*segment)) {
				segment->updateArea(id, area.get(), shader);
			}
		}
		for (auto& segment: updated) {
			if (area->checkIntersects(*segment)) {
				segment->updateArea(id, area.get(), shader);
			} else {
				segment->updateArea(id, nullptr, nullptr);
			}
		}

		m_terrainAreas[id] = TerrainAreaEntry{std::move(area), rect};
	}

	for (auto& segment: removed) {
		segment->updateArea(id, nullptr, nullptr);
	}


	return old_box;
}


} // namespace Mercator
