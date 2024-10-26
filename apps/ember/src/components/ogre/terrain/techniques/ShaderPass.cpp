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

#include "ShaderPass.h"
#include "ShaderPassBlendMapBatch.h"
#include "Shader.h"

#include "components/ogre/terrain/TerrainPageSurfaceLayer.h"

#include <OgreRoot.h>
#include <OgreShadowCameraSetupPSSM.h>
#include <OgreTextureManager.h>

#include <utility>
#include <boost/algorithm/string/predicate.hpp>

namespace Ember::OgreView::Terrain::Techniques {


Ogre::GpuProgramPtr ShaderPass::fetchOrCreateSplattingFragmentProgram(SplattingFragmentConfig config) {
	auto& mgr = Ogre::GpuProgramManager::getSingleton();
	//If there are no valid layers we'll just use a simple white colour. This should normally not happen.
	if (config.layers == 0) {
		return mgr.getByName("SimpleWhiteFp", Ogre::RGN_DEFAULT);
	} else {

		std::stringstream ss;

		ss << "SplatFp/";
		if (config.offsetMapping) {
			ss << "OffsetMapping/";
		}
		ss << config.layers;
		if (!config.fog) {
			ss << "/NoFog";
		}
		if (!config.lightning) {
			ss << "/NoLighting";
		} else if (!config.shadows) {
			ss << "/NoShadows";
		}

		auto programName = ss.str();

		auto prog = mgr.getByName(programName, Ogre::RGN_DEFAULT);
		if (prog) {
			return prog;
		} else {
			prog = mgr.createProgram(programName, Ogre::RGN_DEFAULT, "glsl", Ogre::GPT_FRAGMENT_PROGRAM);
			prog->setSourceFile("common/base/Splat.frag");
			std::stringstream definesSS;
			definesSS << "BASE_LAYER=0";
			if (config.lightning) {
				definesSS << ",NUM_LIGHTS=3";
			} else {
				definesSS << ",NUM_LIGHTS=0";
			}
			if (config.offsetMapping) {
				definesSS << ",OFFSET_MAPPING=1";
			} else {
				definesSS << ",OFFSET_MAPPING=0";
			}
			if (config.shadows) {
				definesSS << ",SHADOW=1";
			} else {
				definesSS << ",SHADOW=0";
			}
			if (config.fog) {
				definesSS << ",FOG=1";
			} else {
				definesSS << ",FOG=0";
			}
			definesSS << ",NUM_LAYERS=" << std::to_string(config.layers);
			prog->setParameter("preprocessor_defines", definesSS.str());

			auto params = prog->getDefaultParameters();

			int samplerIndex = 0;

			if (config.lightning) {
				params->setNamedAutoConstant("ambientColour", Ogre::GpuProgramParameters::ACT_AMBIENT_LIGHT_COLOUR);
				params->setNamedAutoConstant("worldMatrix", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
				params->setNamedAutoConstant("numberOfActiveLights", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
				params->setNamedAutoConstant("lightPosition", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_ARRAY, 3);
				params->setNamedAutoConstant("lightDiffuse", Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_ARRAY, 3);

				params->setNamedConstant("normalTexture", samplerIndex++);
			}

			if (config.shadows) {
				params->setNamedAutoConstant("inverseShadowMapSize0", Ogre::GpuProgramParameters::ACT_INVERSE_TEXTURE_SIZE, samplerIndex);
				params->setNamedConstant("shadowMap0", samplerIndex++);
				params->setNamedAutoConstant("inverseShadowMapSize1", Ogre::GpuProgramParameters::ACT_INVERSE_TEXTURE_SIZE, samplerIndex);
				params->setNamedConstant("shadowMap1", samplerIndex++);
				params->setNamedAutoConstant("inverseShadowMapSize2", Ogre::GpuProgramParameters::ACT_INVERSE_TEXTURE_SIZE, samplerIndex);
				params->setNamedConstant("shadowMap2", samplerIndex++);
				params->setNamedAutoConstant("inverseShadowMapSize3", Ogre::GpuProgramParameters::ACT_INVERSE_TEXTURE_SIZE, samplerIndex);
				params->setNamedConstant("shadowMap3", samplerIndex++);
				params->setNamedAutoConstant("inverseShadowMapSize4", Ogre::GpuProgramParameters::ACT_INVERSE_TEXTURE_SIZE, samplerIndex);
				params->setNamedConstant("shadowMap4", samplerIndex++);

				params->setNamedConstant("fixedDepthBias", -0.0001f);
				params->setNamedConstant("gradientClamp", 0.0098f);
				params->setNamedConstant("gradientScaleBias", 0.01f);
			}

			if (config.fog) {
				params->setNamedAutoConstant("fogColour", Ogre::GpuProgramParameters::ACT_FOG_COLOUR);
				params->setNamedConstant("disableFogColour", 0);
			}

			if (config.offsetMapping) {
				params->setNamedAutoConstant("cameraPositionObjSpace", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
				params->setNamedConstant("scaleBias", Ogre::Vector2(0.01f, -0.00f));
			}

			for (int layer = 0; layer < config.layers; ++layer) {
				if (layer % 4 == 0) {
					int blendLevel = (layer / 4);
					params->setNamedConstant("blendMap" + std::to_string(blendLevel), samplerIndex++);
				}
				params->setNamedConstant("diffuseTexture" + std::to_string(layer), samplerIndex++);
				if (config.offsetMapping) {
					params->setNamedConstant("normalHeightTexture" + std::to_string(layer), samplerIndex++);
				}
			}
			return prog;
		}
	}
}

Ogre::TexturePtr ShaderPass::getCombinedBlendMapTexture(size_t passIndex, size_t batchIndex, std::set<std::string>& managedTextures) const {
	// we need an unique name for our alpha texture
	std::stringstream combinedBlendMapTextureNameSS;

	combinedBlendMapTextureNameSS << "terrain_" << mPosition.first << "_" << mPosition.second << "_combinedBlendMap_" << passIndex << "_" << batchIndex << "_" << mBlendMapPixelWidth;
	const Ogre::String combinedBlendMapName(combinedBlendMapTextureNameSS.str());
	Ogre::TexturePtr combinedBlendMapTexture;
	Ogre::TextureManager* textureMgr = Ogre::Root::getSingletonPtr()->getTextureManager();
	if (textureMgr->resourceExists(combinedBlendMapName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME)) {
		logger->debug("Using already created blendMap texture {}", combinedBlendMapName);
		combinedBlendMapTexture = static_cast<Ogre::TexturePtr>(textureMgr->getByName(combinedBlendMapName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
		if (!combinedBlendMapTexture->isLoaded()) {
			combinedBlendMapTexture->createInternalResources();
		}
		return combinedBlendMapTexture;
	}
	logger->debug("Creating new blendMap texture {} with size {}", combinedBlendMapName, mBlendMapPixelWidth);
	int flags = Ogre::TU_DYNAMIC_WRITE_ONLY;
	// automipmapping seems to cause some trouble on Windows, at least in OpenGL on Nvidia cards
	// Thus we'll disable it. The performance impact shouldn't be significant.
#ifndef _WIN32
	flags |= Ogre::TU_AUTOMIPMAP;
#endif // ifndef _WIN32
	combinedBlendMapTexture = textureMgr->createManual(combinedBlendMapName,
													   Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
													   Ogre::TEX_TYPE_2D,
													   mBlendMapPixelWidth,
													   mBlendMapPixelWidth,
													   (int) textureMgr->getDefaultNumMipmaps(),
													   Ogre::PF_B8G8R8A8, flags);
	managedTextures.insert(combinedBlendMapName);
	combinedBlendMapTexture->createInternalResources();
	return combinedBlendMapTexture;
}

ShaderPass::ShaderPass(Ogre::SceneManager& sceneManager, int blendMapPixelWidth, TerrainIndex position, bool useNormalMapping) :
		mScales{},
		mSceneManager(sceneManager),
		mBlendMapPixelWidth(blendMapPixelWidth),
		mPosition(std::move(position)),
		mShadowLayers(0),
		mUseNormalMapping(useNormalMapping) {
	for (float& scale: mScales) {
		scale = 0.0;
	}
}

ShaderPass::~ShaderPass() = default;


ShaderPassBlendMapBatch* ShaderPass::getCurrentBatch() {
	auto I = mBlendMapBatches.rbegin();
	if (mBlendMapBatches.empty() || (*I)->getLayers().size() >= 4) {
		mBlendMapBatches.emplace_back(createNewBatch());
		return mBlendMapBatches.back().get();
	} else {
		return I->get();
	}
}

std::unique_ptr<ShaderPassBlendMapBatch> ShaderPass::createNewBatch() {
	return std::make_unique<ShaderPassBlendMapBatch>(*this, getBlendMapPixelWidth());
}

void ShaderPass::addLayer(const TerrainPageGeometry& geometry, const TerrainPageSurfaceLayer* layer) {
	getCurrentBatch()->addLayer(geometry, layer);

	mScales[mLayers.size()] = layer->getScale();
	mLayers.push_back(layer);
}

bool ShaderPass::finalize(Ogre::Pass& pass, std::set<std::string>& managedTextures, bool useShadows, bool useLighting) const {
	logger->debug("Creating terrain material pass with: NormalMapping={} Shadows={} Lighting={}", mUseNormalMapping, useShadows, useLighting);

	SplattingFragmentConfig fragmentConfig{
			.lightning=useLighting,
			.shadows=useShadows,
			.offsetMapping=mUseNormalMapping,
			.fog=true,
			.layers=(int) mLayers.size()
	};
	if (useLighting) {
		logger->debug("Adding normal map texture unit state.");
		Ogre::TextureUnitState* normalMapTextureUnitState = pass.createTextureUnitState();

		// Set up an alias for the normal texture. This way the terrain implementation can generate the normal texture at a later time and link it to this material.
		// With the Ogre Terrain Component, this is set up in OgreTerrainMaterialGeneratorEmber.cpp.
		normalMapTextureUnitState->setName(Techniques::Shader::NORMAL_TEXTURE_ALIAS);
		normalMapTextureUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	}

	if (useShadows) {
		for (int i = 0; i < mShadowLayers; ++i) {
			Ogre::TextureUnitState* textureUnitState = pass.createTextureUnitState();

			textureUnitState->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
			textureUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
			textureUnitState->setTextureBorderColour(Ogre::ColourValue(1.0, 1.0, 1.0, 1.0));
		}
		logger->debug("Added {} shadow layers.", mShadowLayers);
	}


	size_t i = 0;
	// add our blendMap textures first
	for (auto& batch: mBlendMapBatches) {
		batch->finalize(pass, getCombinedBlendMapTexture(pass.getIndex(), i++, managedTextures), mUseNormalMapping);
	}

	pass.setSceneBlending(Ogre::SBF_ONE, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);

	if (!useLighting) {
		pass.setMaxSimultaneousLights(0);
	} else {
		pass.setMaxSimultaneousLights(3);
	}

	auto fragmentProgram = fetchOrCreateSplattingFragmentProgram(fragmentConfig);

	try {
		logger->debug("Using fragment program {} for terrain page.", fragmentProgram->getName());
		pass.setGpuProgram(Ogre::GpuProgramType::GPT_FRAGMENT_PROGRAM, fragmentProgram);
	} catch (const std::exception& ex) {
		logger->warn("Error when setting fragment program '{}': {}", fragmentProgram->getName(), ex.what());
		return false;
	}
	if (!pass.hasFragmentProgram()) {
		return false;
	}
	try {
		Ogre::GpuProgramParametersSharedPtr fpParams = pass.getFragmentProgramParameters();
		fpParams->setIgnoreMissingParams(true);
		fpParams->setNamedConstant("scales", mScales.data(), (mLayers.size() - 1) / 4 + 1);

		if (useShadows) {
			auto* pssmSetup = dynamic_cast<Ogre::PSSMShadowCameraSetup*>(mSceneManager.getShadowCameraSetup().get());
			if (pssmSetup) {
				Ogre::Vector4 splitPoints;
				Ogre::PSSMShadowCameraSetup::SplitPointList splitPointList = pssmSetup->getSplitPoints();
				for (int splitPointIndex = 0; splitPointIndex < 3; splitPointIndex++) {
					splitPoints[splitPointIndex] = splitPointList[splitPointIndex];
				}

				fpParams->setNamedConstant("pssmSplitPoints", splitPoints);
			}

		}
	} catch (const std::exception& ex) {
		logger->warn("Error when setting fragment program parameters: {}", ex.what());
		return false;
	}

	// add vertex shader for fog
	std::string lightningVpProgram = "Lighting/NormalTexture/";
	if (useShadows) {
		lightningVpProgram += "ShadowVp";
	} else {
		lightningVpProgram += "SimpleVp";
	}

	if (mSceneManager.getFogMode() != Ogre::FOG_EXP2) {
		logger->error("Fog mode is different, but using vertex program {} for terrain material pass.", lightningVpProgram);
	}

	logger->debug("Using vertex program {} for terrain material pass.", lightningVpProgram);
	pass.setVertexProgram(lightningVpProgram);

	return true;
}

bool ShaderPass::hasRoomForLayer(const TerrainPageSurfaceLayer* layer) {
	//TODO: calculate this once
	Ogre::ushort numberOfTextureUnitsOnCard = std::min(static_cast<Ogre::ushort>(OGRE_MAX_TEXTURE_LAYERS), Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->getNumTextureUnits());

	//We'll project the number of taken units if we should add another pass.
	//Later on we'll compare this with the actual number it texture units available.
	size_t projectedTakenUnits = 1; // One unit is always used by the global normal texture
	projectedTakenUnits += mShadowLayers; // Shadow textures
	// A blend map texture for every 4 layers
	// Make sure to always have 1 for 1 layer, 2 for 5 layers etc.
	projectedTakenUnits += mBlendMapBatches.size();
	if (!mBlendMapBatches.empty() && mBlendMapBatches.back()->getLayers().size() == 4) {
		//If the last batch if full we should need to create a new one; thus we'll increase the number of units
		projectedTakenUnits++;
	}

	projectedTakenUnits += mLayers.size();
	if (mUseNormalMapping) {
		projectedTakenUnits += mLayers.size();
	}

	projectedTakenUnits++;
	if (mUseNormalMapping) {
		projectedTakenUnits++;
	}


	return numberOfTextureUnitsOnCard > projectedTakenUnits;
}

void ShaderPass::addShadowLayer() {
	mShadowLayers++;
}

unsigned int ShaderPass::getBlendMapPixelWidth() const {
	return mBlendMapPixelWidth;
}

Ogre::TexturePtr resolveTexture(const std::string& texture) {
	if (texture.empty()) {
		return {};
	}
	auto& textureManager = Ogre::TextureManager::getSingleton();
	auto result = textureManager.getByName(texture, "world");
	auto alternativeExtension = boost::algorithm::ends_with(texture, ".png") ? ".dds" : ".png";
	auto alternativeTextureName = texture.substr(0, texture.length() - 4) + alternativeExtension;
	if (result) {
		return result;
	} else {
		result = textureManager.getByName(alternativeTextureName, "world");
		if (result) {
			return std::static_pointer_cast<Ogre::Texture>(result);
		}
	}
	//Check if the file exists anyway, but just isn't loaded.
	auto openResourceResult = Ogre::ResourceGroupManager::getSingleton().openResource(texture, "world", nullptr, false);
	if (openResourceResult) {
		openResourceResult->close();
		return textureManager.load(texture, "world");
	}
	openResourceResult = Ogre::ResourceGroupManager::getSingleton().openResource(alternativeTextureName, "world", nullptr, false);
	if (openResourceResult) {
		openResourceResult->close();
		return textureManager.load(alternativeTextureName, "world");
	}
	return {};
}

}





