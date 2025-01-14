//
// C++ Implementation: FoliageLoader
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
#include "FoliageLoader.h"

#include "../Convert.h"
#include "../terrain/PlantAreaQuery.h"
#include "../terrain/PlantAreaQueryResult.h"
#include "../terrain/TerrainManager.h"
#include "../terrain/TerrainLayerDefinition.h"
#include "../terrain/PlantInstance.h"
#include "framework/Log.h"

using namespace Ember::OgreView::Terrain;



namespace Ember::OgreView::Environment {

FoliageLoader::FoliageLoader(Ogre::SceneManager& sceneMgr,
							 Terrain::TerrainManager& terrainManager,
							 const Terrain::TerrainLayer& terrainLayer,
							 const Terrain::TerrainFoliageDefinition& foliageDefinition,
							 ::Forests::PagedGeometry& pagedGeometry) :
		mTerrainManager(terrainManager),
		mTerrainLayer(terrainLayer),
		mFoliageDefinition(foliageDefinition),
		mPagedGeometry(pagedGeometry),
		mMinScale(1),
		mMaxScale(1),
		mLatestPlantsResult(nullptr),
		mDensityFactor(1) {
	mEntity = sceneMgr.createEntity(std::string("shrubbery_") + mFoliageDefinition.mPlantType, mFoliageDefinition.getParameter("mesh"));

	mMinScale = std::stof(mFoliageDefinition.getParameter("minScale"));
	mMaxScale = std::stof(mFoliageDefinition.getParameter("maxScale"));

}

FoliageLoader::~FoliageLoader() {
	if (mEntity) {
		mEntity->_getManager()->destroyEntity(mEntity);
	}
}

bool FoliageLoader::preparePage(::Forests::PageInfo& page) {
	if (mLatestPlantsResult && WFMath::Equal(mLatestPlantsResult->mQuery.mCenter.x, page.centerPoint.x) &&
		WFMath::Equal(mLatestPlantsResult->mQuery.mCenter.y, page.centerPoint.z)) {
		return true;
	} else {
		PlantAreaQuery query{mTerrainLayer, mFoliageDefinition.mPlantType, page.bounds, Ogre::Vector2(page.centerPoint.x, page.centerPoint.z)};
		sigc::slot<void(const Terrain::PlantAreaQueryResult&)> slot = sigc::mem_fun(*this, &FoliageLoader::plantQueryExecuted);

		mTerrainManager.getPlantsForArea(query, slot);
		return false;
	}
}

void FoliageLoader::loadPage(::Forests::PageInfo&) {
	Ogre::ColourValue colour(1, 1, 1, 1);
	int plantNo = 0;

	const PlantAreaQueryResult::PlantStore& store = mLatestPlantsResult->mStore;
	const int maxCount = (int) (store.size() * mDensityFactor);

	for (const auto& plantInstance: store) {
		if (plantNo == maxCount) {
			break;
		}
		addEntity(mEntity, plantInstance.position, Ogre::Quaternion(Ogre::Degree(plantInstance.orientation), Ogre::Vector3::UNIT_Y),
				  Ogre::Vector3(plantInstance.scale.x, plantInstance.scale.y, plantInstance.scale.x), colour);
		plantNo++;
	}
}

void FoliageLoader::plantQueryExecuted(const Terrain::PlantAreaQueryResult& queryResult) {
	mLatestPlantsResult = &queryResult;
	//Be sure to catch errors so that we always reset the mLatestPlantsResult field when done.
	try {
		mPagedGeometry.reloadGeometryPage(Ogre::Vector3(queryResult.mQuery.mCenter.x, 0, queryResult.mQuery.mCenter.y), true);
	} catch (const std::exception& ex) {
		logger->error("Error when reloading geometry: {}", ex.what());
	} catch (...) {
		logger->error("Unknown error when reloading geometry.");
	}
	mLatestPlantsResult = nullptr;

}

void FoliageLoader::setDensityFactor(float density) {
	mDensityFactor = density;
}

}



