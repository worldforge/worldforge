/*
 Copyright (C) 2009 Erik Ogenvik <erik@ogenvik.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "TerrainUpdateTask.h"
#include "TerrainHandler.h"
#include "TerrainDefPoint.h"
#include "TerrainInfo.h"
#include "SegmentManager.h"
#include <Mercator/Terrain.h>

#include <utility>


namespace Ember::OgreView::Terrain {

TerrainUpdateTask::TerrainUpdateTask(Mercator::Terrain& terrain,
									 TerrainDefPointStore terrainPoints,
									 TerrainHandler& handler,
									 TerrainInfo& terrainInfo,
									 bool& hasTerrainInfo,
									 SegmentManager& segmentManager) :
		mTerrain(terrain),
		mTerrainPoints(std::move(terrainPoints)),
		mTerrainHandler(handler),
		mTerrainInfo(terrainInfo),
		mHasTerrainInfo(hasTerrainInfo),
		mSegmentManager(segmentManager) {
}

TerrainUpdateTask::~TerrainUpdateTask() = default;

void TerrainUpdateTask::executeTaskInBackgroundThread(Tasks::TaskExecutionContext& context) {
	int terrainRes = mTerrain.getResolution();
	for (const auto& mTerrainPoint: mTerrainPoints) {
		Mercator::BasePoint bp;
		const TerrainPosition& pos = mTerrainPoint.position;
		if (mTerrain.getBasePoint(static_cast<int> (pos.x()), static_cast<int> (pos.y()), bp) && (WFMath::Equal(mTerrainPoint.height, bp.height()))
			&& (WFMath::Equal(mTerrainPoint.roughness, bp.roughness()) && (WFMath::Equal(mTerrainPoint.falloff, bp.falloff())))) {
			logger->debug("Point [{},{}] unchanged", pos.x(), pos.y());
			continue;
		} else {
			logger->debug("Setting base point [{},{}] to height {}", pos.x(), pos.y(), mTerrainPoint.height);
		}
		bp.height() = mTerrainPoint.height;
		bp.roughness() = mTerrainPoint.roughness;
		bp.falloff() = mTerrainPoint.falloff;

		// FIXME Sort out roughness and falloff, and generally verify this code is the same as that in Terrain layer
		mTerrain.setBasePoint(static_cast<int> (pos.x()), static_cast<int> (pos.y()), bp);
		mUpdatedBasePoints.emplace_back(pos, bp);
		mUpdatedPositions.emplace_back(pos.x() * terrainRes, pos.y() * terrainRes);
	}
	mSegmentManager.syncWithTerrain();
}

bool TerrainUpdateTask::executeTaskInMainThread() {
	for (const auto& updatedBasePoint: mUpdatedBasePoints) {
		mTerrainInfo.setBasePoint(updatedBasePoint.first, updatedBasePoint.second);
	}
	mTerrainHandler.EventWorldSizeChanged.emit();

	if (!mHasTerrainInfo) {
		mHasTerrainInfo = true;
	} else {
		if (!mUpdatedPositions.empty()) {
			//if it's an update, we need to reload all pages and adjust all entity positions
			mTerrainHandler.reloadTerrain(mUpdatedPositions);
		}
	}
	return true;
}


}



