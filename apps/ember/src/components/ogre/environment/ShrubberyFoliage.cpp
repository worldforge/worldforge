//
// C++ Implementation: ShrubberyFoliage
//
// Description:
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2008
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#include "ShrubberyFoliage.h"

#include <utility>

#include "framework/Log.h"

#include "FoliageLoader.h"

#include "../Scene.h"
#include "../terrain/TerrainManager.h"

#include "BatchPage.h"

namespace Ember::OgreView::Environment {

ShrubberyFoliage::ShrubberyFoliage(Terrain::TerrainManager& terrainManager,
								   Terrain::TerrainLayer terrainLayer,
								   Terrain::TerrainFoliageDefinition foliageDefinition)
		: FoliageBase(terrainManager, std::move(terrainLayer), std::move(foliageDefinition)) {
}

ShrubberyFoliage::~ShrubberyFoliage() = default;

void ShrubberyFoliage::initialize() {
	mPagedGeometry = std::make_unique<::Forests::PagedGeometry>(&mTerrainManager.getScene().getMainCamera(), mTerrainManager.getFoliageBatchSize());

	mPagedGeometry->setInfinite();


	mPagedGeometry->addDetailLevel<Forests::BatchPage>(64, 32);

	mLoader = std::make_unique<FoliageLoader>(mTerrainManager.getScene().getSceneManager(),
											  mTerrainManager,
											  mTerrainLayer,
											  mFoliageDefinition,
											  *mPagedGeometry);
	mPagedGeometry->setPageLoader(mLoader.get());

	std::list<Forests::GeometryPageManager*> detailLevels = mPagedGeometry->getDetailLevels();
	for (auto& detailLevel: detailLevels) {
		DistanceStore tempDistance = {detailLevel->getFarRange(), detailLevel->getNearRange(), detailLevel->getTransition()};
		mDistanceStore.push_back(tempDistance);
	}
}

void ShrubberyFoliage::frameStarted() {
	if (mPagedGeometry) {
		try {
			mPagedGeometry->update();
		} catch (const std::exception&) {
			logger->error("Error when updating shrubbery for terrain layer {} and areaId {}.", mTerrainLayer.layerDef.mName, mTerrainLayer.layerDef.mAreaId);
			throw;
		}
	}
}

void ShrubberyFoliage::setDensity(float newGrassDensity) {
	mLoader->setDensityFactor(newGrassDensity);
	mPagedGeometry->reloadGeometry();
}

void ShrubberyFoliage::setFarDistance(float factor) {
	std::list<Forests::GeometryPageManager*> detailLevels = mPagedGeometry->getDetailLevels();

	auto J = mDistanceStore.begin();
	for (auto I = detailLevels.begin(); I != detailLevels.end() && J != mDistanceStore.end(); ++I) {
		(*I)->setFarRange(factor * J->farDistance);
		(*I)->setNearRange(factor * J->nearDistance);
		(*I)->setTransition(factor * J->transition);
		++J;
	}
	mPagedGeometry->reloadGeometry();
}

}



