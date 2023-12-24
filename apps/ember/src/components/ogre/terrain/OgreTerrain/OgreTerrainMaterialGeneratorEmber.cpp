/*
 Copyright (C) 2013 Samuel Kogler

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

#include "OgreTerrainMaterialGeneratorEmber.h"
#include "components/ogre/TerrainPageDataProvider.h"
#include "../techniques/Shader.h"
#include <OgreMaterialManager.h>
#include <OgreTerrainGroup.h>
#include <OgreTechnique.h>

using namespace Ogre;

namespace {
Ember::OgreView::IPageDataProvider::OgreIndex calculateTerrainIndex(const Terrain& terrain,
																	const TerrainGroup& terrainGroup) {
	auto position = (terrain.getPosition() - terrainGroup.getOrigin()) / (Ogre::Real) terrain.getWorldSize();
	return {(long) position.x, (long) -position.z};
}
}


namespace Ember::OgreView::Terrain {
const std::string OgreTerrainMaterialGeneratorEmber::ERROR_MATERIAL = "/common/primitives/texture/error";


OgreTerrainMaterialGeneratorEmber::OgreTerrainMaterialGeneratorEmber(IPageDataProvider& dataProvider,
										 const TerrainGroup& terrainGroup,
										 sigc::signal<void(const Ogre::TRect<Ogre::Real>&)>& terrainShownSignal) :
		mDataProvider(dataProvider),
		mTerrainGroup(terrainGroup),
		mTerrainShownSignal(terrainShownSignal) {
	//Check that our error material exists
	mErrorMaterialTemplate = Ogre::MaterialManager::getSingleton().getByName(ERROR_MATERIAL);
	assert(mErrorMaterialTemplate);
}

void OgreTerrainMaterialGeneratorEmber::requestOptions(Ogre::Terrain* terrain) {
	terrain->_setMorphRequired(false);
	terrain->_setNormalMapRequired(true);
	terrain->_setLightMapRequired(false, false);

	auto terrainIndex = calculateTerrainIndex(*terrain, mTerrainGroup);
	// Allocate in main thread so no race conditions
	std::unique_ptr<IPageData> pageData = mDataProvider.getPageData(terrainIndex);
	if (!pageData || !pageData->getCompositeMapMaterial() || pageData->getCompositeMapMaterial()->getTechniques().empty()) {
		terrain->_setCompositeMapRequired(false);
	} else {
		terrain->_setCompositeMapRequired(true);
	}
}

Ogre::MaterialPtr OgreTerrainMaterialGeneratorEmber::generate(const Ogre::Terrain* terrain) {

	if (!terrain) {
		//This could happen if the terrain is shutting down.
		return mErrorMaterialTemplate;
	}

	auto index = calculateTerrainIndex(*terrain, mTerrainGroup);

	std::unique_ptr<IPageData> pageData = mDataProvider.getPageData(index);

	if (!pageData) {
		logger->warn("Could not find corresponding page data for OgreTerrain at [{}|{}]", index.first, index.second);
		return getOrCreateMaterialClone(mErrorMaterialTemplate, terrain->getMaterialName());
	}

	logger->info("Loading material for terrain page: [{}|{}]", index.first, index.second);

	auto mat = pageData->getMaterial();

	if (!mat) {
		logger->warn("Terrain material was not found.");
		return getOrCreateMaterialClone(mErrorMaterialTemplate, terrain->getMaterialName());
	}

	auto newMaterial = getOrCreateMaterialClone(mat, terrain->getMaterialName());

	auto compositeMap = terrain->getCompositeMap();
	auto normalMap = terrain->getTerrainNormalMap();

	for (auto technique: newMaterial->getTechniques()) {
		for (auto pass: technique->getPasses()) {
			for (auto tus: pass->getTextureUnitStates()) {
				if (tus->getName() == Techniques::Shader::NORMAL_TEXTURE_ALIAS) {
					tus->setTexture(normalMap);
				} else if (tus->getName() == Techniques::Shader::COMPOSITE_MAP_ALIAS) {
					if (!compositeMap) {
						logger->warn("A composite map was required for for terrain page: [{}|{}] but could not be found.", index.first, index.second);
					} else {
						tus->setTexture(compositeMap);
					}
				}
			}
		}
	}

	auto bbox = terrain->getWorldAABB();
	Ogre::TRect<Ogre::Real> rect(bbox.getMinimum().x, bbox.getMinimum().z, bbox.getMaximum().x, bbox.getMaximum().z);
	mTerrainShownSignal(rect);

	return newMaterial;
}

Ogre::MaterialPtr OgreTerrainMaterialGeneratorEmber::generateForCompositeMap(const Ogre::Terrain* terrain) {

	auto materialName = terrain->getMaterialName() + "_comp";
	auto index = calculateTerrainIndex(*terrain, mTerrainGroup);

	logger->debug("Loading composite map material for terrain page: [{}|{}]", index.first, index.second);

	std::unique_ptr<IPageData> pageData(mDataProvider.getPageData(index));
	if (!pageData) {
		logger->warn("Could not find corresponding page data for OgreTerrain at [{}|{}]", index.first, index.second);
		return getOrCreateMaterialClone(mErrorMaterialTemplate, materialName);
	}

	Ogre::MaterialPtr mat = pageData->getCompositeMapMaterial();

	if (mat) {
		return getOrCreateMaterialClone(mat, materialName);
	} else {
		logger->warn("Composite map material was not found. This might happen if the page is currently being unloaded.");
		return getOrCreateMaterialClone(mErrorMaterialTemplate, materialName);
	}
}

Ogre::MaterialPtr OgreTerrainMaterialGeneratorEmber::getOrCreateMaterialClone(const Ogre::MaterialPtr& templateMaterial, const std::string& materialName) {

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);
	if (mat) {
		//If the material copy already existed, make sure it's updated to the latest version of the template material.
		*mat = *templateMaterial;
		return mat;
	}

	return templateMaterial->clone(materialName);
}


//void OgreTerrainMaterialGeneratorEmber::_renderCompositeMap(size_t size, const Ogre::Rect& rect, const Ogre::MaterialPtr& mat, const Ogre::TexturePtr& destCompositeMap) {
//	//Just perform a sanity check to avoid crashes from divide-by-zero
//	if (rect.width() > 0 && rect.height() > 0) {
//		TerrainMaterialGenerator::_renderCompositeMap(size, rect, mat, destCompositeMap);
//	}
//}
} // namespace Ember::OgreView::Terrain


